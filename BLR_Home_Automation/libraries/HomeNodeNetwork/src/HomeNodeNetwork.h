#pragma once

#include <Arduino.h>
#include <base64.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>
#include <HTTPClient.h>
#endif

class HomeNodeNetwork {
 public:
  typedef uint32_t (*MaskFunction)();
  typedef String (*ExtraStatusFunction)();

  HomeNodeNetwork(const char* node, const char* apSsid,
                  const char* apPassword, const char* password, IPAddress apIp,
                  MaskFunction state, MaskFunction configured,
                  ExtraStatusFunction extraStatus = nullptr)
      : node_(node), apSsid_(apSsid), apPassword_(apPassword), password_(password),
        apIp_(apIp), state_(state), configured_(configured),
        extraStatus_(extraStatus) {}

  bool begin() {
    if (!valid(apSsid_) || !valid(apPassword_) || strlen(apPassword_) < 8 ||
        !valid(password_) || apIp_ == IPAddress(0, 0, 0, 0)) {
      Serial.println("[ERROR] Configure master hotspot credentials, AP_IP and node password in config.h");
      return false;
    }
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);
    startConnection();
    enabled_ = true;
    return true;
  }

  void requestStatusUpdate() {
    statusUpdateRequested_ = true;
  }

  void tick() {
    if (!enabled_) return;
    const uint32_t now = millis();
    if (disconnecting_) {
      if (WiFi.status() == WL_CONNECTED) {
        if (uint32_t(now - attemptStart_) >= 3000) {
          Serial.println("[WARN] Waiting for old Wi-Fi connection to disconnect; retrying");
          WiFi.disconnect();
          attemptStart_ = now;
        }
        return;
      }
      WiFi.begin(apSsid_, apPassword_);
      disconnecting_ = false;
      attemptStart_ = now;
      return;
    }
    const bool connected = WiFi.status() == WL_CONNECTED &&
                           WiFi.localIP() != IPAddress(0, 0, 0, 0);
    if (!connected) {
      if (wasConnected_) {
        Serial.println("[WARN] Master hotspot disconnected; reconnecting");
        startConnection();
      } else if (uint32_t(now - attemptStart_) >= 15000) {
        Serial.println("[WARN] Master hotspot connection timed out; retrying");
        startConnection();
      }
      return;
    }

    if (!wasConnected_) {
      wasConnected_ = true;
      registered_ = false;
      lastRegistration_ = now - 4000;
      Serial.printf("[INFO] Connected to master hotspot %s, IP: ", apSsid_);
      Serial.println(WiFi.localIP());
      Serial.printf("[INFO] Gateway: %s, RSSI: %d dBm, master: %s\n",
                    WiFi.gatewayIP().toString().c_str(), static_cast<int>(WiFi.RSSI()),
                    apIp_.toString().c_str());
    }
    if (registrationPending_) {
      pollRegistration();
      return;
    }
    if (!statusUpdateRequested_ && uint32_t(now - lastRegistration_) < 4000) return;
    lastRegistration_ = now;
    statusUpdateRequested_ = false;

    registrationClient_.setTimeout(1000);
#if defined(ESP8266)
    const bool connectedToMaster = registrationClient_.connect(apIp_, 80);
    const int connectionError = HTTPC_ERROR_CONNECTION_FAILED;
#else
    const bool connectedToMaster = registrationClient_.connect(apIp_, 80, 1000);
    const int connectionError = HTTPC_ERROR_CONNECTION_REFUSED;
#endif
    if (!connectedToMaster) {
      finishRegistration(connectionError);
      return;
    }
    String body = String("node=") + node_ + "&state=" + state_() +
                  "&configured=" + configured_();
    if (extraStatus_) body += extraStatus_();
    const String request = String("POST /internal/register HTTP/1.0\r\nHost: ") +
        apIp_.toString() + "\r\nAuthorization: Basic " +
        base64::encode(String("node:") + password_) +
        "\r\nContent-Type: application/x-www-form-urlencoded\r\n" +
        "X-Node-Request: 1\r\nConnection: close\r\nContent-Length: " +
        String(body.length()) + "\r\n\r\n" + body;
    if (registrationClient_.print(request) != request.length()) {
      finishRegistration(HTTPC_ERROR_SEND_PAYLOAD_FAILED);
      return;
    }
    statusLength_ = 0;
    responseCode_ = 0;
    responseBytes_ = 0;
    headerHasData_ = false;
    registrationStarted_ = millis();
    registrationPending_ = true;
  }

  static bool valid(const char* value) {
    return value && value[0] && strstr(value, "REPLACE_") == nullptr;
  }

 private:
  void pollRegistration() {
    // Keep serving incoming commands while the master processes registration.
    for (unsigned budget = 0; budget < 256 && registrationClient_.available(); ++budget) {
      const int value = registrationClient_.read();
      if (value < 0) break;
      if (++responseBytes_ > 2048) {
        finishRegistration(HTTPC_ERROR_NO_HTTP_SERVER);
        return;
      }
      if (value == '\r') continue;
      if (value == '\n') {
        if (!responseCode_) {
          if (statusLength_ < 12 ||
              (strncmp(statusLine_, "HTTP/1.0 ", 9) != 0 &&
               strncmp(statusLine_, "HTTP/1.1 ", 9) != 0) ||
              statusLine_[9] < '1' || statusLine_[9] > '5' ||
              statusLine_[10] < '0' || statusLine_[10] > '9' ||
              statusLine_[11] < '0' || statusLine_[11] > '9' ||
              (statusLength_ > 12 && statusLine_[12] != ' ')) {
            finishRegistration(HTTPC_ERROR_NO_HTTP_SERVER);
            return;
          }
          responseCode_ = (statusLine_[9] - '0') * 100 +
                          (statusLine_[10] - '0') * 10 + statusLine_[11] - '0';
        } else if (!headerHasData_) {
          finishRegistration(responseCode_);
          return;
        }
        headerHasData_ = false;
      } else if (!responseCode_) {
        if (statusLength_ >= sizeof(statusLine_) - 1) {
          finishRegistration(HTTPC_ERROR_NO_HTTP_SERVER);
          return;
        }
        statusLine_[statusLength_++] = static_cast<char>(value);
      } else {
        headerHasData_ = true;
      }
    }
    if (!registrationClient_.connected() && !registrationClient_.available()) {
      finishRegistration(HTTPC_ERROR_CONNECTION_LOST);
    } else if (uint32_t(millis() - registrationStarted_) >= 5000) {
      finishRegistration(HTTPC_ERROR_READ_TIMEOUT);
    }
  }

  void finishRegistration(int result) {
    registrationClient_.stop();
    registrationPending_ = false;
    const String url = String("http://") + apIp_.toString() + "/internal/register";
    if (result == 200) {
      if (!registered_) Serial.printf("[INFO] Registered with master: %s\n", url.c_str());
      registered_ = true;
    } else {
      registered_ = false;
      if (result > 0) {
        Serial.printf("[WARN] Registration rejected by %s: HTTP %d; check credentials/configuration\n",
                      url.c_str(), result);
      } else {
        Serial.printf("[WARN] Registration failed to %s: %d (%s)\n",
                      url.c_str(), result, HTTPClient::errorToString(result).c_str());
      }
    }
  }

  void startConnection() {
    registrationClient_.stop();
    registrationPending_ = false;
    wasConnected_ = false;
    registered_ = false;
    disconnecting_ = true;
    WiFi.disconnect();
    attemptStart_ = millis();
    Serial.println("[INFO] Connecting to master hotspot");
  }

  const char* node_;
  const char* apSsid_;
  const char* apPassword_;
  const char* password_;
  IPAddress apIp_;
  MaskFunction state_;
  MaskFunction configured_;
  ExtraStatusFunction extraStatus_;
  WiFiClient registrationClient_;
  char statusLine_[128] = {};
  size_t statusLength_ = 0;
  unsigned responseBytes_ = 0;
  int responseCode_ = 0;
  uint32_t attemptStart_ = 0;
  uint32_t lastRegistration_ = 0;
  uint32_t registrationStarted_ = 0;
  bool headerHasData_ = false;
  bool registrationPending_ = false;
  bool statusUpdateRequested_ = false;
  bool wasConnected_ = false;
  bool disconnecting_ = false;
  bool registered_ = false;
  bool enabled_ = false;
};
