#include "config.h"
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <HomeNodeNetwork.h>
#include <HomeOta.h>
#include "KnobLights.h"

#if !defined(ESP8266)
#error The IR node requires an ESP8266.
#endif

struct IRCommand {
  const char* name;
  uint32_t code;
};

const IRCommand commands[] = {
  {"optical", 0x10EF42BD}, {"hdmi", 0x10EF02FD},
  {"tv", 0x10EF52AD}, {"usb", 0x10EF629D},
  {"bluetooth", 0x10EFA25D}, {"aux", 0x10EF22DD},
  {"mute", 0x10EFF00F}, {"power", 0x10EF827D},
  {"master_plus", 0x10EF906F}, {"master_minus", 0x10EFA05F},
  {"front_plus", 0x10EF40BF}, {"front_minus", 0x10EFC03F},
  {"centre_plus", 0x10EF50AF}, {"centre_minus", 0x10EF609F},
  {"surround_plus", 0x10EF00FF}, {"surround_minus", 0x10EF807F},
  {"sub_plus", 0x10EFD02F}, {"sub_minus", 0x10EFE01F},
  {"reset", 0x10EF1AE5}, {"up", 0x10EF926D},
  {"down", 0x10EFB04F}, {"left", 0x10EF728D},
  {"right", 0x10EF32CD}, {"ok", 0x10EFB24D},
  {"info", 0x10EFD827}, {"mode", 0x10EF9867},
  {"amode", 0x10EF12ED}, {"tone", 0x10EFC837},
  {"test", 0x10EF30CF}
};

ESP8266WebServer server(80);
IRsend irsend(IR_LED_PIN >= 0 ? IR_LED_PIN : 0);
bool irReady = false;
bool networkReady = false;
HomeOta ota;
KnobLights knobLights;
uint32_t stateMask() { return knobLights.ready() && knobLights.settings().on ? 2 : 0; }
uint32_t configuredMask() { return (irReady ? 1 : 0) | (knobLights.ready() ? 2 : 0); }
String lightStatus() { return "&" + knobSettingsBody(knobLights.settings()); }
HomeNodeNetwork network("ir", AP_SSID, AP_PASSWORD,
                        NODE_PASSWORD, AP_IP, stateMask, configuredMask,
                        lightStatus);

bool authorize() {
  if (!server.authenticate("node", NODE_PASSWORD)) {
    server.requestAuthentication();
    return false;
  }
  if (server.header("X-Node-Request") != "1") {
    server.send(403, "text/plain", "Missing request header");
    return false;
  }
  return true;
}

void handleKnobLights() {
  if (!authorize()) {
    Serial.println("[WARN] Knob lighting request rejected: authorization/header");
    return;
  }
  if (!knobLights.ready()) {
    Serial.println("[WARN] Knob lighting request rejected: LEDs not configured");
    server.send(409, "text/plain", "Knob LEDs not configured");
    return;
  }
  KnobLightSettings settings;
  if (!parseKnobLights(server.arg("on"), server.arg("color"),
                       server.arg("brightness"), server.arg("pattern"), settings)) {
    Serial.println("[WARN] Knob lighting request rejected: invalid settings");
    server.send(400, "text/plain", "Invalid knob lighting settings");
    return;
  }
  knobLights.apply(settings);
  Serial.printf("[INFO] Knob lights %s: pattern=%s, color=#%s, brightness=%u%%\n",
                settings.on ? "ON" : "OFF", KNOB_PATTERNS[settings.pattern],
                knobColorHex(settings.color).c_str(),
                static_cast<unsigned>(settings.brightness));
  server.send(200, "text/plain", "OK");
}

void handleIR() {
  if (!authorize()) return;
  if (!irReady) {
    server.send(409, "text/plain", "IR output is not configured");
    return;
  }
  const String name = server.arg("command");
  for (const IRCommand& command : commands) {
    if (name == command.name) {
      irsend.sendNEC(command.code, 32);
      Serial.printf("[INFO] Sent IR command: %s\n", command.name);
      server.send(200, "text/plain", "Sent");
      return;
    }
  }
  server.send(400, "text/plain", "Unknown IR command");
}

void setup() {
  Serial.begin(115200);
  knobLights.begin();
  irReady = IR_LED_PIN >= 0 && IR_LED_PIN <= 16 &&
            !(IR_LED_PIN >= 6 && IR_LED_PIN <= 11) &&
            IR_LED_PIN != 1 && IR_LED_PIN != 3;
  if (irReady) {
    irsend.begin();
  } else {
    Serial.println("[WARN] IR GPIO not configured or invalid; IR disabled");
  }
  server.collectHeaders("X-Node-Request");
  server.on("/ir", HTTP_POST, handleIR);
  server.on("/knob-lights", HTTP_POST, handleKnobLights);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  networkReady = network.begin();
  if (networkReady) {
    server.begin();
    ota.configure("home-amp-remote", OTA_PASSWORD, []() { knobLights.off(); });
  }
}

void loop() {
  knobLights.tick();
  if (networkReady) {
    server.handleClient();
    network.tick();
    ota.tick(WiFi.status() == WL_CONNECTED ? WiFi.localIP() : IPAddress(0, 0, 0, 0));
  }
  yield();
}
