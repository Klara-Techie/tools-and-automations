#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <WebServer.h>
#include <WiFi.h>
#include <esp_system.h>

#include "AudioControl.h"
#include "HomeNetwork.h"
#include "PresetManager.h"
#include "StateJson.h"
#include "WebPage.h"

AudioControl audio;
HomeNetwork homeNetwork;
PresetManager presets;
Preferences storage;
WebServer server(80);
String token;
String warning;
uint32_t revision = 0;
uint32_t changedAt = 0;
bool dirty = false;
bool audioFault = false;
String audioWarning;
String presetWarning;

void saveIfDue();

String randomHex(size_t bytes) {
  static const char digits[] = "0123456789abcdef";
  String result;
  for (size_t i = 0; i < bytes; ++i) {
    uint8_t value;
    esp_fill_random(&value, 1);
    result += digits[value >> 4];
    result += digits[value & 15];
  }
  return result;
}

void reply(int code, const char *type, const String &body) {
  server.sendHeader("Cache-Control", "no-store");
  server.sendHeader("X-Content-Type-Options", "nosniff");
  server.sendHeader("X-Frame-Options", "DENY");
  server.send(code, type, body);
}

bool validHost() {
  const String host = server.header("Host");
  const String ap = WiFi.softAPIP().toString();
  if (host == ap || host == ap + ":80") return true;
  if (WiFi.status() != WL_CONNECTED) return false;
  const String home = WiFi.localIP().toString();
  return host == home || host == home + ":80";
}

bool authorized() {
  return validHost() && server.header("X-Receiver-Token") == token;
}

void handleState() {
  if (!validHost()) {
    reply(403, "text/plain", "Invalid host");
    return;
  }
  DynamicJsonDocument doc(8192);
  doc["revision"] = revision;
  doc["token"] = token;
  String combinedWarning = audioWarning;
  if (presetWarning.length()) {
    if (combinedWarning.length()) combinedWarning += " ";
    combinedWarning += presetWarning;
  }
  if (warning.length()) {
    if (combinedWarning.length()) combinedWarning += " ";
    combinedWarning += warning;
  }
  doc["warning"] = combinedWarning;
  doc["pendingSave"] = dirty;
  doc["ramping"] = audio.isRamping();
  doc["homeStatus"] = homeNetwork.status();
  doc["homeConfigured"] = homeNetwork.configured();
  doc["homeAddress"] = WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : "";
  JsonArray presetNames = doc.createNestedArray("presets");
  for (unsigned i = 0; i < presets.count(); ++i) presetNames.add(presets.at(i).name);
  writeState(doc.createNestedObject("state"), audio.state());
  JsonArray channels = doc.createNestedArray("transmitted");
  for (const fr6311::Channel &channel : audio.transmitted().channels) {
    JsonObject item = channels.createNestedObject();
    item["attenuation"] = channel.attenuation;
    item["muted"] = channel.muted;
  }
  if (doc.overflowed()) {
    reply(500, "text/plain", "State response exceeds available memory");
    return;
  }
  String body;
  serializeJson(doc, body);
  reply(200, "application/json", body);
}

bool persistAudioChange() {
  ++revision;
  dirty = true;
  changedAt = millis() - 2000;
  saveIfDue();
  return !dirty;
}

void handleUpdate() {
  if (!authorized()) {
    reply(403, "text/plain", "Not authorized");
    return;
  }
  if (audioFault) {
    reply(503, "text/plain", "Audio control fault; restart required");
    return;
  }
  String body = server.arg("plain");
  if (body.length() > 4096) {
    reply(413, "text/plain", "Request too large");
    return;
  }
  DynamicJsonDocument doc(8192);
  if (deserializeJson(doc, body) || !doc["revision"].is<uint32_t>() ||
      doc["revision"].as<uint32_t>() != revision) {
    reply(409, "text/plain", "Invalid or stale request; reload the state");
    return;
  }
  AudioState next;
  if (!readState(doc["state"].as<JsonObjectConst>(), next)) {
    reply(400, "text/plain", "Invalid or unsupported settings");
    return;
  }
  if (!next.hardware.leftRightMulti || !next.hardware.surroundMulti ||
      next.hardware.recEnabled) {
    reply(400, "text/plain", "Only the 5.1 multichannel input is supported");
    return;
  }
  if (!audio.apply(next)) {
    reply(500, "text/plain", "Audio command rejected");
    return;
  }
  if (!persistAudioChange()) {
    reply(500, "text/plain", "Target accepted, but saving failed; retrying");
    return;
  }
  reply(200, "text/plain", "Target accepted and saved");
}

void handlePreset() {
  if (!authorized()) { reply(403, "text/plain", "Not authorized"); return; }
  if (audioFault) { reply(503, "text/plain", "Audio control fault; restart required"); return; }
  String body = server.arg("plain");
  if (body.length() > 1024) { reply(413, "text/plain", "Request too large"); return; }
  DynamicJsonDocument doc(2048);
  if (deserializeJson(doc, body) || !doc["revision"].is<uint32_t>() ||
      doc["revision"].as<uint32_t>() != revision || !doc["action"].is<const char *>()) {
    reply(409, "text/plain", "Invalid or stale request; reload the state");
    return;
  }
  String action = doc["action"].as<String>();
  if (!doc["name"].is<const char *>() ||
      (action == "rename" && !doc["newName"].is<const char *>())) {
    reply(400, "text/plain", "Invalid preset name");
    return;
  }
  String name = doc["name"].as<String>();
  String error;
  if (action == "apply") {
    AudioState next = audio.state();
    if (!presets.apply(name, next, error)) { reply(400, "text/plain", error); return; }
    if (!audio.apply(next)) { reply(500, "text/plain", "Audio command rejected"); return; }
    if (!persistAudioChange()) {
      reply(500, "text/plain", "Preset applied, but saving settings failed; retrying");
      return;
    }
    reply(200, "text/plain", "Preset applied and saved");
    return;
  }

  PresetManager updated = presets;
  bool ok = false;
  if (action == "create") ok = updated.create(name, audio.state(), error);
  else if (action == "update") ok = updated.update(name, audio.state(), error);
  else if (action == "rename") ok = updated.rename(name, doc["newName"].as<String>(), error);
  else if (action == "delete") ok = updated.remove(name, error);
  else error = "Unknown preset action";
  if (!ok) { reply(400, "text/plain", error); return; }
  if (!updated.save(storage)) {
    reply(500, "text/plain", "Could not save presets");
    return;
  }
  presets = updated;
  presetWarning = "";
  ++revision;
  reply(200, "text/plain", "Presets saved");
}

void handleNetwork() {
  if (!authorized()) { reply(403, "text/plain", "Not authorized"); return; }
  String body = server.arg("plain");
  if (body.length() > 1024) { reply(413, "text/plain", "Request too large"); return; }
  DynamicJsonDocument doc(2048);
  if (deserializeJson(doc, body) || !doc["action"].is<const char *>()) {
    reply(400, "text/plain", "Invalid network settings");
    return;
  }
  String error;
  const String action = doc["action"].as<String>();
  bool ok = false;
  if (action == "save" && doc["ssid"].is<const char *>() &&
      doc["password"].is<const char *>()) {
    ok = homeNetwork.configure(storage, doc["ssid"].as<String>(),
                               doc["password"].as<String>(), error);
  } else if (action == "clear") {
    ok = homeNetwork.clear(storage, error);
  } else {
    error = "Invalid network settings";
  }
  if (!ok) { reply(400, "text/plain", error); return; }
  ++revision;
  reply(200, "text/plain", "Network settings saved");
}

void saveIfDue() {
  if (!dirty || millis() - changedAt < 2000) return;
  DynamicJsonDocument doc(4096);
  writeState(doc.to<JsonObject>(), audio.state());
  String serialized;
  serializeJson(doc, serialized);
  if (storage.putString("settings", serialized) != serialized.length()) {
    warning = "Settings could not be saved; retrying.";
    Serial.println(warning);
    changedAt = millis();
    return;
  }
  dirty = false;
  warning = "";
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("WARNING: Prototype; verify serial timing and audio before connecting an amplifier.");
  if (!storage.begin("receiver", false)) {
    Serial.println("ERROR: Cannot open persistent storage; firmware stopped.");
    while (true) delay(1000);
  }
  if (!presets.load(storage)) {
    presets = PresetManager{};
    presetWarning = "Saved presets are invalid; no presets loaded.";
    Serial.println(presetWarning);
  }
  AudioState initial = AudioControl::defaults();
  String saved = storage.getString("settings", "");
  if (saved.length()) {
    DynamicJsonDocument doc(4096);
    if (deserializeJson(doc, saved) || !readState(doc.as<JsonObjectConst>(), initial)) {
      initial = AudioControl::defaults();
      warning = "Saved settings are invalid; muted defaults loaded.";
      Serial.println(warning);
    }
  }
  if (!initial.hardware.leftRightMulti || !initial.hardware.surroundMulti ||
      initial.hardware.recEnabled) {
    AudioControl::selectMultichannelInput(initial);
    Serial.println("Saved routing changed to 5.1 multichannel; REC disabled.");
  }
  if (!initial.restoreLastVolume) initial.masterAttenuation = initial.startupAttenuation;
  AudioState mutedInitial = initial;
  mutedInitial.masterMuted = true;
  if (!audio.begin(mutedInitial)) {
    Serial.println("ERROR: Invalid initial audio state; firmware stopped.");
    while (true) delay(1000);
  }

  WiFi.mode(WIFI_AP_STA);
  String password = storage.getString("apPassword", "");
  if (password.length() != 16) {
    password = randomHex(8);
    if (storage.putString("apPassword", password) != password.length()) {
      Serial.println("ERROR: Cannot save Wi-Fi password; firmware stopped.");
      while (true) delay(1000);
    }
  }
  token = randomHex(16);
  if (!WiFi.softAP("AV-Receiver", password.c_str())) {
    Serial.println("ERROR: Could not start receiver access point.");
    while (true) delay(1000);
  }
  Serial.println("Connect to Wi-Fi: AV-Receiver");
  Serial.print("Wi-Fi password: ");
  Serial.println(password);
  Serial.print("Open http://");
  Serial.println(WiFi.softAPIP());
  const char *headers[] = {"Host", "X-Receiver-Token"};
  server.collectHeaders(headers, 2);
  server.on("/", HTTP_GET, []() {
    if (!validHost()) {
      reply(403, "text/plain", "Invalid host");
      return;
    }
    server.sendHeader("Cache-Control", "no-store");
    server.sendHeader("X-Frame-Options", "DENY");
    server.send_P(200, "text/html; charset=utf-8", kWebPage);
  });
  server.on("/api/state", HTTP_GET, handleState);
  server.on("/api/state", HTTP_POST, handleUpdate);
  server.on("/api/presets", HTTP_POST, handlePreset);
  server.on("/api/network", HTTP_POST, handleNetwork);
  server.onNotFound([]() { reply(404, "text/plain", "Not found"); });
  server.begin();
  homeNetwork.begin(storage);
  if (!initial.masterMuted && !audio.apply(initial)) {
    audioFault = true;
    audioWarning = "Unable to restore sound; receiver remains muted. Restart required.";
    Serial.println(audioWarning);
  }
}

void loop() {
  server.handleClient();
  homeNetwork.tick();
  if (!audioFault && !audio.tick()) {
    audioFault = true;
    AudioState safe = audio.state();
    safe.masterMuted = true;
    if (audio.apply(safe)) {
      ++revision;
      dirty = true;
      changedAt = millis();
      audioWarning = "Audio control fault; mute command sent. Restart required.";
    } else {
      audioWarning = "Audio control fault; mute could not be sent. Disconnect the amplifier.";
    }
    Serial.println(audioWarning);
  }
  saveIfDue();
  delay(1);
}
