#pragma once

#include <Arduino.h>
#include <ArduinoOTA.h>

class HomeOta {
 public:
  void configure(const char* hostname, const char* password,
                 void (*prepareUpdate)() = nullptr) {
    if (!hostname || !hostname[0] || !password || strlen(password) < 8 ||
        strstr(password, "REPLACE_")) {
      Serial.println("[ERROR] OTA disabled: configure a hostname and password of at least 8 characters");
      return;
    }
    prepareUpdate_ = prepareUpdate;
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.setPassword(password);
    ArduinoOTA.onStart([this]() {
      if (prepareUpdate_) prepareUpdate_();
      Serial.println("[INFO] OTA update started");
    });
    ArduinoOTA.onEnd([]() { Serial.println("[INFO] OTA update finished; restarting"); });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("[ERROR] OTA update failed (code %u)\n", static_cast<unsigned>(error));
    });
    enabled_ = true;
  }

  void tick(const IPAddress& address) {
    if (!enabled_) return;
    if (address != address_) {
      if (active_) ArduinoOTA.end();
      active_ = false;
      address_ = address;
      if (address != IPAddress(0, 0, 0, 0)) {
        ArduinoOTA.begin();
        active_ = true;
        Serial.print("[INFO] OTA service starting at ");
        Serial.println(address);
      }
    }
    if (active_) ArduinoOTA.handle();
  }

 private:
  void (*prepareUpdate_)() = nullptr;
  IPAddress address_;
  bool enabled_ = false;
  bool active_ = false;
};
