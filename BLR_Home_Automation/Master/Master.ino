#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WebSocketsServer.h>
#include <HomeOta.h>
#include <EntertainmentRelays.h>
#include "config.h"
#include "dashboard.h"
#include "types.h"

WebServer server(80);
WebSocketsServer updates(81);
HomeOta ota;
const int entertainmentPins[] = {LIVING_TV, LIVING_AMP, LIVING_LED};
EntertainmentRelays entertainment(entertainmentPins, LIVING_ACTIVE_LOW);

const uint8_t LOCAL_NODE = 255;  // Local relays are not in the remote-node array.
const uint8_t WALL_NODE = 0;
const uint8_t HEATER_NODE = 1;

Node nodes[] = {
  {"wall", IPAddress(), 0, 0, 0, false},
  {"heater", IPAddress(), 0, 0, 0, false}
};

const Appliance appliances[] = {
  {"TV", LOCAL_NODE, 0}, {"Amplifier", LOCAL_NODE, 1}, {"LED strip", LOCAL_NODE, 2},
  {"Fan", WALL_NODE, 0}, {"Light", WALL_NODE, 1}, {"Socket", WALL_NODE, 2},
  {"Socket 2", WALL_NODE, 3}, {"Water heater", HEATER_NODE, 0}
};

bool apRunning = false;
bool previouslyConnected = false;
bool routerConfigured = false;
bool ready = false;
uint32_t lastApAttempt = 0;
uint32_t lastReconnect = 0;
const uint32_t MAX_TIMER_SECONDS = MAX_RELAY_TIMER_SECONDS;
uint32_t socketTimer = 0;
uint32_t socketTimerReportedAt = 0;
WiFiClient receiverProbe;
String receiverResponse;
const uint32_t RECEIVER_PROBE_INTERVAL_MS = 2000;
const uint8_t RECEIVER_FAILURE_LIMIT = 2;
uint32_t lastReceiverProbe = 0;
uint32_t receiverProbeStarted = 0;
uint32_t lastReceiverFailureLog = 0;
String lastReceiverFailure;
uint8_t receiverFailures = 0;
bool receiverProbePending = false;
bool receiverOnline = false;

bool valid(const char* value) {
  return value && value[0] && strstr(value, "REPLACE_") == nullptr;
}

void notifyClients() {
  updates.broadcastTXT("refresh");
}

void setReceiverOnline(bool online) {
  if (receiverOnline == online) return;
  receiverOnline = online;
  Serial.println(online ? "[INFO] Receiver web service online" :
                          "[WARN] Receiver web service offline");
  notifyClients();
}

void finishReceiverProbe(bool online, const char* reason) {
  receiverProbe.stop();
  receiverProbePending = false;
  receiverResponse = "";
  if (online) {
    receiverFailures = 0;
    lastReceiverFailure = "";
    setReceiverOnline(true);
    return;
  }
  if (receiverFailures < RECEIVER_FAILURE_LIMIT) ++receiverFailures;
  const uint32_t now = millis();
  if (lastReceiverFailure != reason ||
      uint32_t(now - lastReceiverFailureLog) >= 30000UL) {
    Serial.printf("[WARN] Receiver probe failed: %s\n", reason);
    lastReceiverFailure = reason;
    lastReceiverFailureLog = now;
  }
  if (receiverFailures >= RECEIVER_FAILURE_LIMIT) setReceiverOnline(false);
}

void maintainReceiver() {
  const uint32_t now = millis();
  if (WiFi.status() != WL_CONNECTED || !(entertainment.stateMask() & 2)) {
    if (receiverProbePending) receiverProbe.stop();
    receiverProbePending = false;
    receiverResponse = "";
    receiverFailures = 0;
    lastReceiverFailure = "";
    setReceiverOnline(false);
    lastReceiverProbe = now - RECEIVER_PROBE_INTERVAL_MS;
    return;
  }
  if (receiverProbePending) {
    for (unsigned budget = 0; budget < 256 && receiverProbe.available(); ++budget) {
      const int value = receiverProbe.read();
      if (value < 0) break;
      receiverResponse += static_cast<char>(value);
      if (receiverResponse.length() > 1024) {
        finishReceiverProbe(false, "response too large");
        return;
      }
    }
    const int separator = receiverResponse.indexOf("\r\n\r\n");
    if (separator >= 0) {
      const String body = receiverResponse.substring(separator + 4);
      if (body.length() >= 11) {
        const bool ok = (receiverResponse.startsWith("HTTP/1.1 200 ") ||
                         receiverResponse.startsWith("HTTP/1.0 200 ")) &&
                        body == "AV-Receiver";
        finishReceiverProbe(ok, "unexpected HTTP status or health response");
        return;
      }
    }
    if (!receiverProbe.connected() && !receiverProbe.available()) {
      finishReceiverProbe(false, "connection closed before complete response");
    } else if (uint32_t(now - receiverProbeStarted) >= 2000UL) {
      finishReceiverProbe(false, "response timed out");
    }
    return;
  }
  if (uint32_t(now - lastReceiverProbe) < RECEIVER_PROBE_INTERVAL_MS) return;
  lastReceiverProbe = now;
  if (!receiverProbe.connect(RECEIVER_IP, 80, 500)) {
    finishReceiverProbe(false, "connection to receiver failed");
    return;
  }
  const String request = String("GET /api/health HTTP/1.0\r\nHost: ") +
                         RECEIVER_IP.toString() + "\r\nConnection: close\r\n\r\n";
  if (receiverProbe.print(request) != request.length()) {
    finishReceiverProbe(false, "could not send health request");
    return;
  }
  receiverResponse = "";
  receiverProbeStarted = millis();
  receiverProbePending = true;
}

Node* findNode(const String& id) {
  for (Node& node : nodes) if (id == node.id) return &node;
  return nullptr;
}

bool readUnsigned(const String& text, uint32_t& result) {
  if (!text.length() || text.length() > 7) return false;
  result = 0;
  for (size_t i = 0; i < text.length(); ++i) {
    if (text[i] < '0' || text[i] > '9') return false;
    result = result * 10 + (text[i] - '0');
  }
  return true;
}

uint32_t socketTimerRemaining() {
  const uint32_t elapsed = uint32_t(millis() - socketTimerReportedAt) / 1000UL;
  return socketTimer > elapsed ? socketTimer - elapsed : 0;
}

bool adminAuthorized() {
  if (server.authenticate(ADMIN_USER, ADMIN_PASSWORD)) return true;
  server.requestAuthentication();
  return false;
}

bool browserPostAuthorized() {
  if (!adminAuthorized()) return false;
  if (server.header("X-Requested-With") == "XMLHttpRequest") return true;
  server.send(403, "text/plain", "Missing request header");
  return false;
}

void handleRegister() {
  if (!server.authenticate("node", NODE_PASSWORD)) {
    server.requestAuthentication();
    return;
  }
  if (server.header("X-Node-Request") != "1") {
    server.send(403, "text/plain", "Missing request header");
    return;
  }
  Node* node = findNode(server.arg("node"));
  uint32_t state, configured;
  if (!node || !readUnsigned(server.arg("state"), state) ||
      !readUnsigned(server.arg("configured"), configured) ||
      state > 31 || configured > 31 || (state & ~configured)) {
    server.send(400, "text/plain", "Invalid registration");
    return;
  }
  uint32_t receivedSocketTimer = 0;
  if (node == &nodes[WALL_NODE] &&
      (!readUnsigned(server.arg("t2"), receivedSocketTimer) ||
       receivedSocketTimer > MAX_TIMER_SECONDS)) {
    server.send(400, "text/plain", "Invalid socket timer status");
    return;
  }
  const IPAddress address = server.client().remoteIP();
  bool changed = !node->online || node->state != state ||
                 node->configured != configured || node->address != address;
  if (node == &nodes[WALL_NODE]) {
    if (socketTimer != receivedSocketTimer) changed = true;
    socketTimer = receivedSocketTimer;
    socketTimerReportedAt = millis();
  }
  node->address = address;
  node->state = state;
  node->configured = configured;
  node->lastSeen = millis();
  node->online = true;
  server.send(200, "text/plain", "OK");
  if (changed) notifyClients();
}

void handleState() {
  if (!adminAuthorized()) return;
  if (entertainment.updateTimers()) notifyClients();
  String json = "{\"ap\":";
  json += apRunning && WiFi.status() != WL_CONNECTED ? "true" : "false";
  json += ",\"devices\":[";
  for (size_t i = 0; i < sizeof(appliances) / sizeof(appliances[0]); ++i) {
    const Appliance& a = appliances[i];
    const bool local = a.node == LOCAL_NODE;
    const Node* node = local ? nullptr : &nodes[a.node];
    const uint32_t bit = 1UL << a.channel;
    if (i) json += ',';
    json += "{\"id\":" + String(i) + ",\"name\":\"" + a.name + "\",\"online\":";
    json += local || node->online ? "true" : "false";
    json += ",\"configured\":";
    json += ((local ? entertainment.configuredMask() : node->configured) & bit) ? "true" : "false";
    json += ",\"on\":";
    json += ((local ? entertainment.stateMask() : node->state) & bit) ? "true" : "false";
    if (local) {
      json += ",\"timerRemaining\":";
      json += entertainment.remainingSeconds(a.channel);
    } else if (a.node == WALL_NODE && a.channel == 2) {
      json += ",\"timerRemaining\":";
      json += socketTimerRemaining();
    }
    json += '}';
  }
  json += "],\"allTimerRemaining\":";
  json += entertainment.remainingSeconds(3);
  json += ",\"receiverOnline\":";
  json += receiverOnline && WiFi.status() == WL_CONNECTED &&
          (entertainment.stateMask() & 2) ? "true" : "false";
  json += "}";
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", json);
}

int postToNode(Node& node, const String& path, const String& body, String& response) {
  WiFiClient client;
  HTTPClient http;
  const String url = String("http://") + node.address.toString() + path;
  if (!http.begin(client, url)) return -1;
  http.setTimeout(1500);
  http.setAuthorization("node", NODE_PASSWORD);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("X-Node-Request", "1");
  const int code = http.POST(body);
  if (code > 0) response = http.getString();
  http.end();
  return code;
}

void handleRelay() {
  if (!browserPostAuthorized()) return;
  uint32_t index;
  const String on = server.arg("on");
  if (!readUnsigned(server.arg("id"), index) ||
      index >= sizeof(appliances) / sizeof(appliances[0]) ||
      (on != "0" && on != "1")) {
    server.send(400, "text/plain", "Invalid appliance or state");
    return;
  }
  const Appliance& a = appliances[index];
  if (a.node == LOCAL_NODE) {
    if (!entertainment.setRelay(a.channel, on == "1")) {
      server.send(409, "text/plain", "Entertainment relay not configured");
      return;
    }
    server.send(200, "text/plain", "OK");
    notifyClients();
    return;
  }
  Node& node = nodes[a.node];
  if (!node.online || !(node.configured & (1UL << a.channel))) {
    server.send(409, "text/plain", "Device offline or not configured");
    return;
  }
  String response;
  const int code = postToNode(node, "/relay",
                             String("channel=") + a.channel + "&on=" + on, response);
  uint32_t state;
  if (code != 200 || !readUnsigned(response, state) || state > 31) {
    server.send(502, "text/plain", "Device did not confirm; check its state");
    return;
  }
  node.state = state;
  if (a.node == WALL_NODE && a.channel == 2) {
    socketTimer = 0;
    socketTimerReportedAt = millis();
  }
  server.send(200, "text/plain", "OK");
  notifyClients();
}

void handleAll() {
  if (!browserPostAuthorized()) return;
  const String on = server.arg("on");
  if (on != "0" && on != "1") {
    server.send(400, "text/plain", "Invalid state");
    return;
  }
  if (!entertainment.setAll(on == "1")) {
    server.send(409, "text/plain", "Entertainment relays not configured");
    return;
  }
  server.send(200, "text/plain", "OK");
  notifyClients();
}

void handleTimer() {
  if (!browserPostAuthorized()) return;
  const String target = server.arg("target");
  const String action = server.arg("action");
  const bool all = target == "all";
  const bool socket = target == "5";
  if ((!all && !socket && (target.length() != 1 || target[0] < '0' || target[0] > '2')) ||
      (action != "start" && action != "cancel")) {
    server.send(400, "text/plain", "Invalid timer request");
    return;
  }
  uint32_t seconds = 0;
  if (action == "start" &&
      (!readUnsigned(server.arg("seconds"), seconds) ||
       seconds < 1 || seconds > MAX_TIMER_SECONDS)) {
    server.send(400, "text/plain", "Timer must be 1-604800 seconds");
    return;
  }
  if (!socket) {
    const uint32_t required = all ? 7UL : 1UL << (target[0] - '0');
    if (seconds && !(entertainment.stateMask() & required)) {
      server.send(409, "text/plain", all ?
                  "Turn on at least one entertainment device before starting a timer" :
                  "Turn on the device before starting a timer");
      return;
    }
    if (!entertainment.setTimer(all ? 3 : target[0] - '0', seconds)) {
      server.send(409, "text/plain", "Entertainment timer channel not configured");
      return;
    }
    server.send(200, "text/plain", "OK");
    notifyClients();
    return;
  }
  Node& node = nodes[WALL_NODE];
  if (!node.online || !(node.configured & 4)) {
    server.send(409, "text/plain", "Timer device offline or not configured");
    return;
  }
  if (seconds && !(node.state & 4)) {
    server.send(409, "text/plain", "Turn on the socket before starting a timer");
    return;
  }
  String response;
  const String body = "target=2&action=" + action + "&seconds=" + seconds;
  const int code = postToNode(node, "/timer", body, response);
  if (code != 200) {
    if (code == 409) server.send(409, "text/plain", response);
    else server.send(502, "text/plain", "Timer not confirmed; check device status");
    return;
  }
  socketTimer = seconds;
  socketTimerReportedAt = millis();
  server.send(200, "text/plain", "OK");
  notifyClients();
}

void startHotspot() {
  lastApAttempt = millis();
  const int channel = WiFi.status() == WL_CONNECTED ? WiFi.channel() : 1;
  if (!WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0)) ||
      !WiFi.softAP(AP_SSID, AP_PASSWORD, channel, false, 8)) {
    Serial.println("[ERROR] Failed to start master hotspot; retrying");
    return;
  }
  apRunning = true;
  Serial.print("[INFO] Always-on master hotspot address: ");
  Serial.println(WiFi.softAPIP());
}

void maintainWiFi() {
  const uint32_t now = millis();
  if (!apRunning && uint32_t(now - lastApAttempt) >= 5000) startHotspot();
  if (!routerConfigured) return;
  if (WiFi.status() == WL_CONNECTED) {
    if (!previouslyConnected) {
      Serial.print("[INFO] Router address: ");
      Serial.println(WiFi.localIP());
      previouslyConnected = true;
      notifyClients();
    }
    return;
  }
  if (previouslyConnected) {
    previouslyConnected = false;
    Serial.println("[WARN] Router disconnected; master hotspot stays available");
    notifyClients();
  }
  if (uint32_t(now - lastReconnect) >= 30000) {
    lastReconnect = now;
    if (!WiFi.reconnect()) {
      Serial.println("[WARN] Router reconnect could not start; master hotspot stays available");
    }
  }
}

void setup() {
  Serial.begin(115200);
  entertainment.begin();
  if (!valid(HOME_SSID) || !valid(HOME_PASSWORD) || !valid(AP_SSID) ||
      !valid(AP_PASSWORD) || strlen(AP_PASSWORD) < 8 ||
      !valid(ADMIN_USER) || !valid(ADMIN_PASSWORD) || !valid(NODE_PASSWORD) ||
      strcmp(HOME_SSID, AP_SSID) == 0) {
    Serial.println("[ERROR] Configure unique SSIDs and credentials in config.h");
    return;
  }
  const char* headers[] = {"X-Requested-With", "X-Node-Request"};
  server.collectHeaders(headers, 2);
  server.on("/", HTTP_GET, []() {
    if (adminAuthorized()) server.send_P(200, "text/html", DASHBOARD);
  });
  server.on("/receiver", HTTP_GET, []() {
    if (!adminAuthorized()) return;
    if (!receiverOnline || WiFi.status() != WL_CONNECTED ||
        !(entertainment.stateMask() & 2)) {
      Serial.printf("[WARN] Receiver navigation blocked: online=%d, home Wi-Fi=%d, amplifier relay=%d\n",
                    receiverOnline ? 1 : 0, WiFi.status() == WL_CONNECTED ? 1 : 0,
                    (entertainment.stateMask() & 2) ? 1 : 0);
      server.send(503, "text/plain", "Receiver is offline or amplifier power is off");
      return;
    }
    server.sendHeader("Location", String("http://") + RECEIVER_IP.toString() + "/");
    server.send(302, "text/plain", "Opening receiver");
  });
  server.on("/api/state", HTTP_GET, handleState);
  server.on("/api/relay", HTTP_POST, handleRelay);
  server.on("/api/all", HTTP_POST, handleAll);
  server.on("/api/timer", HTTP_POST, handleTimer);
  server.on("/internal/register", HTTP_POST, handleRegister);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });

  WiFi.persistent(false);
  if (!WiFi.mode(WIFI_AP_STA)) {
    Serial.println("[ERROR] Failed to enable master hotspot and router Wi-Fi");
    return;
  }
  WiFi.setAutoReconnect(true);
  startHotspot();
  routerConfigured = WiFi.config(MASTER_IP, ROUTER_GATEWAY, ROUTER_SUBNET, ROUTER_GATEWAY);
  if (routerConfigured) {
    WiFi.begin(HOME_SSID, HOME_PASSWORD);
  } else {
    Serial.println("[ERROR] Router static IP configuration failed; master hotspot only");
  }
  lastReconnect = millis();
  server.begin();
  updates.begin();
  ota.configure("home-master", OTA_PASSWORD, []() {
    entertainment.off();
    notifyClients();
  });
  ready = true;
}

void loop() {
  if (entertainment.updateTimers() && ready) notifyClients();
  if (!ready) {
    delay(1000);
    return;
  }
  maintainWiFi();
  maintainReceiver();
  ota.tick(apRunning ? WiFi.softAPIP() :
           (WiFi.status() == WL_CONNECTED ? WiFi.localIP() : IPAddress(0, 0, 0, 0)));
  server.handleClient();
  updates.loop();
  const uint32_t now = millis();
  for (Node& node : nodes) {
    if (node.online && uint32_t(now - node.lastSeen) > 16000) {
      node.online = false;
      notifyClients();
    }
  }
  delay(1);
}
