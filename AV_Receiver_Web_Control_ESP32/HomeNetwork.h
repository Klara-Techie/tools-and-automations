#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

class HomeNetwork {
 public:
  void begin(Preferences &storage);
  void tick();
  bool configure(Preferences &storage, const String &ssid, const String &password,
                 String &error);
  bool clear(Preferences &storage, String &error);
  bool configured() const { return ssid_.length() != 0; }
  const String &status() const { return status_; }

 private:
  enum class Phase { Unconfigured, Discovering, FixedAddress, Connected, Retry };
  void discover();
  void retry(const char *message);
  static bool validCredentials(const String &ssid, const String &password);
  String ssid_;
  String password_;
  String status_ = "Home Wi-Fi not configured";
  Phase phase_ = Phase::Unconfigured;
  uint32_t phaseAt_ = 0;
};
