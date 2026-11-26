#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif
#include <HomeNodeNetwork.h>
#include <HomeOta.h>
#include <EntertainmentRelays.h>

#if (NODE_KIND == 1 || NODE_KIND == 2) && !defined(ESP32)
#error The entertainment and room nodes require an ESP32.
#endif
#if NODE_KIND == 3 && !defined(ESP8266)
#error The heater node requires an ESP8266.
#endif

#if NODE_KIND == 1
const char* NODE_NAME = "living";
const bool ACTIVE_LOW = LIVING_ACTIVE_LOW;
const int relayPins[] = {LIVING_TV, LIVING_AMP, LIVING_LED};
const int switchPins[] = {-1, -1, -1};
#elif NODE_KIND == 2
const char* NODE_NAME = "wall";
const bool ACTIVE_LOW = WALL_ACTIVE_LOW;
const int relayPins[] = {WALL_FAN, WALL_LIGHT, WALL_SOCKET1, WALL_SOCKET2};
const int switchPins[] = {WALL_FAN_SWITCH, WALL_LIGHT_SWITCH, WALL_SOCKET1_SWITCH,
                          WALL_SOCKET2_SWITCH};
#elif NODE_KIND == 3
const char* NODE_NAME = "heater";
const bool ACTIVE_LOW = HEATER_ACTIVE_LOW;
const int relayPins[] = {HEATER_RELAY};
const int switchPins[] = {-1};
#else
#error Select a supported relay node sketch.
#endif

const size_t CHANNELS = sizeof(relayPins) / sizeof(relayPins[0]);
const uint32_t HEATER_LIMIT_MS = 60UL * 60UL * 1000UL;
const uint32_t MAX_TIMER_SECONDS = MAX_RELAY_TIMER_SECONDS;
#if NODE_KIND == 1
EntertainmentRelays entertainment(relayPins, ACTIVE_LOW);
#else
bool states[CHANNELS] = {};
#endif
bool inputStable[CHANNELS] = {};
bool inputSample[CHANNELS] = {};
uint32_t inputChangedAt[CHANNELS] = {};
uint32_t heaterStarted = 0;
bool hardwareValid = false;
bool networkReady = false;
HomeOta ota;
#if NODE_KIND == 2
uint32_t timerStartedAt[CHANNELS] = {};
uint32_t timerDuration[CHANNELS] = {};
#endif

#if defined(ESP8266)
ESP8266WebServer server(80);
#else
WebServer server(80);
#endif

uint32_t stateMask() {
#if NODE_KIND == 1
  return entertainment.stateMask();
#else
  uint32_t mask = 0;
  for (size_t i = 0; i < CHANNELS; ++i) {
    if (states[i]) mask |= 1UL << i;
  }
  return mask;
#endif
}

uint32_t configuredMask() {
#if NODE_KIND == 1
  return entertainment.configuredMask();
#else
  uint32_t mask = 0;
  if (!hardwareValid) return mask;
  for (size_t i = 0; i < CHANNELS; ++i) {
    if (relayPins[i] >= 0) mask |= 1UL << i;
  }
  return mask;
#endif
}

#if NODE_KIND == 1 || NODE_KIND == 2
uint32_t remainingSeconds(uint32_t start, uint32_t duration) {
  if (!duration) return 0;
  const uint32_t elapsed = millis() - start;
  if (elapsed >= duration) return 0;
  return (duration - elapsed + 999UL) / 1000UL;
}

String timerStatus() {
#if NODE_KIND == 1
  return String("&t0=") + entertainment.remainingSeconds(0) +
         "&t1=" + entertainment.remainingSeconds(1) +
         "&t2=" + entertainment.remainingSeconds(2) +
         "&ta=" + entertainment.remainingSeconds(3);
#else
  return String("&t2=") + remainingSeconds(timerStartedAt[2], timerDuration[2]);
#endif
}
#endif

HomeNodeNetwork network(NODE_NAME, AP_SSID, AP_PASSWORD,
                        NODE_PASSWORD, AP_IP, stateMask, configuredMask
#if NODE_KIND == 1 || NODE_KIND == 2
                        , timerStatus
#endif
);

bool validateHardware() {
  for (size_t i = 0; i < CHANNELS; ++i) {
    if (!validRelayPin(relayPins[i], false) || !validRelayPin(switchPins[i], true) ||
        (switchPins[i] >= 0 && relayPins[i] < 0)) return false;
    for (size_t j = 0; j < CHANNELS; ++j) {
      if (relayPins[i] >= 0 && switchPins[j] == relayPins[i]) return false;
      if (i != j && relayPins[i] >= 0 && relayPins[i] == relayPins[j]) return false;
      if (i != j && switchPins[i] >= 0 && switchPins[i] == switchPins[j]) return false;
    }
  }
  return true;
}

void setRelay(size_t channel, bool on) {
#if NODE_KIND == 1
  entertainment.setRelay(channel, on);
#else
  if (!hardwareValid || channel >= CHANNELS || relayPins[channel] < 0) return;
#if NODE_KIND == 1 || NODE_KIND == 2
  timerDuration[channel] = 0;
#endif
  // Repeated ON requests do not extend the heater's safety timeout.
  if (NODE_KIND == 3 && on && !states[channel]) heaterStarted = millis();
  states[channel] = on;
  digitalWrite(relayPins[channel], (on == ACTIVE_LOW) ? LOW : HIGH);
  Serial.printf("[INFO] Channel %u %s\n", static_cast<unsigned>(channel), on ? "ON" : "OFF");
#if NODE_KIND == 2
  network.requestStatusUpdate();
#endif
#endif
}

bool authorize() {
  if (!server.authenticate("node", NODE_PASSWORD)) {
    server.requestAuthentication();
    return false;
  }
  if (server.header("X-Node-Request") == "1") return true;
  server.send(403, "text/plain", "Missing request header");
  return false;
}

void handleRelay() {
  if (!authorize()) return;
  const String channelText = server.arg("channel");
  const String onText = server.arg("on");
  if (channelText.length() != 1 || channelText[0] < '0' ||
      channelText[0] > '9' || (onText != "0" && onText != "1")) {
    server.send(400, "text/plain", "Invalid request");
    return;
  }
  const size_t channel = channelText[0] - '0';
  if (channel >= CHANNELS || !(configuredMask() & (1UL << channel))) {
    server.send(409, "text/plain", "Channel not configured");
    return;
  }
  setRelay(channel, onText == "1");
  server.send(200, "text/plain", String(stateMask()));
}

#if NODE_KIND == 1
void handleAll() {
  if (!authorize()) return;
  const String on = server.arg("on");
  if (on != "0" && on != "1") {
    server.send(400, "text/plain", "Invalid state");
    return;
  }
  if ((configuredMask() & 7) != 7) {
    server.send(409, "text/plain", "Living room channels not configured");
    return;
  }
  entertainment.setAll(on == "1");
  server.send(200, "text/plain", String(stateMask()));
}
#endif

#if NODE_KIND == 1 || NODE_KIND == 2
void handleTimer() {
  if (!authorize()) return;
  const String target = server.arg("target");
  const String action = server.arg("action");
  const bool all = target == "all";
#if NODE_KIND == 1
  if (!all && (target.length() != 1 || target[0] < '0' || target[0] > '2')) {
#else
  if (target != "2") {
#endif
    server.send(400, "text/plain", "Invalid timer target");
    return;
  }
  if (action != "start" && action != "cancel") {
    server.send(400, "text/plain", "Invalid timer action");
    return;
  }
  const uint32_t required = all ? 7UL : (1UL << (target[0] - '0'));
  if ((configuredMask() & required) != required) {
    server.send(409, "text/plain", "Timer channel not configured");
    return;
  }
  uint32_t seconds = 0;
  if (action == "start") {
    const String value = server.arg("seconds");
    if (!value.length() || value.length() > 7) {
      server.send(400, "text/plain", "Timer must be 1-604800 seconds");
      return;
    }
    for (size_t i = 0; i < value.length(); ++i) {
      if (value[i] < '0' || value[i] > '9') {
        server.send(400, "text/plain", "Invalid timer duration");
        return;
      }
      seconds = seconds * 10 + value[i] - '0';
    }
    if (seconds < 1 || seconds > MAX_TIMER_SECONDS) {
      server.send(400, "text/plain", "Timer must be 1-604800 seconds");
      return;
    }
  }
  if (seconds && !(stateMask() & required)) {
    server.send(409, "text/plain", "Turn on a target device before starting a timer");
    return;
  }
#if NODE_KIND == 1
  entertainment.setTimer(all ? 3 : target[0] - '0', seconds);
#else
  {
    const size_t channel = target[0] - '0';
    timerStartedAt[channel] = millis();
    timerDuration[channel] = seconds * 1000UL;
  }
#endif
  server.send(200, "text/plain", "OK");
}

void updateTimers() {
#if NODE_KIND == 1
  entertainment.updateTimers();
#else
  const uint32_t now = millis();
  for (size_t i = 0; i < CHANNELS; ++i) {
    if (timerDuration[i] && uint32_t(now - timerStartedAt[i]) >= timerDuration[i]) {
      timerDuration[i] = 0;
      setRelay(i, false);
    }
  }
#endif
}
#endif

void setup() {
  Serial.begin(115200);
#if NODE_KIND == 1
  hardwareValid = entertainment.begin();
#else
  hardwareValid = validateHardware();
  if (!hardwareValid) {
    Serial.println("[ERROR] Invalid or duplicate GPIO configuration; outputs disabled");
  } else {
    for (size_t i = 0; i < CHANNELS; ++i) {
      if (relayPins[i] >= 0) {
        digitalWrite(relayPins[i], ACTIVE_LOW ? HIGH : LOW);
        pinMode(relayPins[i], OUTPUT);
      }
      if (switchPins[i] >= 0) {
        pinMode(switchPins[i], INPUT_PULLUP);
        inputStable[i] = inputSample[i] = digitalRead(switchPins[i]);
      }
    }
  }
#endif
#if defined(ESP8266)
  server.collectHeaders("X-Node-Request");
#else
  const char* headers[] = {"X-Node-Request"};
  server.collectHeaders(headers, 1);
#endif
  server.on("/relay", HTTP_POST, handleRelay);
#if NODE_KIND == 1
  server.on("/all", HTTP_POST, handleAll);
#endif
#if NODE_KIND == 1 || NODE_KIND == 2
  server.on("/timer", HTTP_POST, handleTimer);
#endif
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  networkReady = network.begin();
  if (networkReady) {
    server.begin();
    ota.configure(OTA_HOSTNAME, OTA_PASSWORD, []() {
#if NODE_KIND == 1
      entertainment.off();
#else
      for (size_t i = 0; i < CHANNELS; ++i) setRelay(i, false);
#endif
    });
  }
}

void loop() {
  if (hardwareValid) {
#if NODE_KIND == 1 || NODE_KIND == 2
    updateTimers();
#endif
#if NODE_KIND == 3
    const uint32_t now = millis();
    if (NODE_KIND == 3 && states[0] && uint32_t(now - heaterStarted) >= HEATER_LIMIT_MS) {
      setRelay(0, false);
      Serial.println("[WARN] Heater automatic shutoff");
    }
#elif NODE_KIND == 2
    const uint32_t now = millis();
    for (size_t i = 0; i < CHANNELS; ++i) {
      if (switchPins[i] < 0) continue;
      const bool reading = digitalRead(switchPins[i]);
      if (reading != inputSample[i]) {
        inputSample[i] = reading;
        inputChangedAt[i] = now;
      }
      if (reading != inputStable[i] && uint32_t(now - inputChangedAt[i]) >= 50) {
        inputStable[i] = reading;
        setRelay(i, !states[i]);
      }
    }
#endif
  }
  if (networkReady) {
    server.handleClient();
    network.tick();
    ota.tick(WiFi.status() == WL_CONNECTED ? WiFi.localIP() : IPAddress(0, 0, 0, 0));
  }
  yield();
}
