#include "HomeNetwork.h"
#include "config.h"

#include <ArduinoJson.h>

namespace {
const IPAddress kReceiverAddress = RECEIVER_IP;
const uint32_t kConnectionTimeoutMs = 20000;
const uint32_t kRetryDelayMs = 30000;

bool sameNetwork(IPAddress first, IPAddress second, IPAddress mask) {
  for (unsigned i = 0; i < 4; ++i) {
    if ((first[i] & mask[i]) != (second[i] & mask[i])) return false;
  }
  return true;
}

bool usableHost(IPAddress address, IPAddress gateway, IPAddress mask) {
  bool networkAddress = true;
  bool broadcastAddress = true;
  for (unsigned i = 0; i < 4; ++i) {
    const uint8_t hostMask = static_cast<uint8_t>(~mask[i]);
    const uint8_t host = address[i] & hostMask;
    if (host != 0) networkAddress = false;
    if (host != hostMask) broadcastAddress = false;
  }
  return address != gateway && !networkAddress && !broadcastAddress;
}
}

bool HomeNetwork::validCredentials(const String &ssid, const String &password) {
  if (ssid.length() == 0 || ssid.length() > 32 ||
      password.length() < 8 || password.length() > 64) return false;
  if (password.length() == 64) {
    for (unsigned i = 0; i < password.length(); ++i) {
      char c = password[i];
      if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F'))) return false;
    }
  }
  return true;
}

void HomeNetwork::begin(Preferences &storage) {
  WiFi.persistent(false);
  WiFi.setAutoReconnect(false);
  if (!storage.isKey("network")) return;
  String encoded = storage.getString("network", "");
  DynamicJsonDocument doc(1024);
  if (deserializeJson(doc, encoded) || !doc["ssid"].is<const char *>() ||
      !doc["password"].is<const char *>()) {
    status_ = "Saved home Wi-Fi settings are invalid";
    return;
  }
  String ssid = doc["ssid"].as<String>();
  String password = doc["password"].as<String>();
  if (!validCredentials(ssid, password)) {
    status_ = "Saved home Wi-Fi settings are invalid";
    return;
  }
  ssid_ = ssid;
  password_ = password;
  discover();
}

void HomeNetwork::discover() {
  WiFi.disconnect(false, false);
  const IPAddress zero(0, 0, 0, 0);
  if (!WiFi.config(zero, zero, zero)) {
    retry("Cannot start DHCP; retrying");
    return;
  }
  WiFi.begin(ssid_.c_str(), password_.c_str());
  phase_ = Phase::Discovering;
  phaseAt_ = millis();
  status_ = "Connecting to home Wi-Fi";
}

void HomeNetwork::retry(const char *message) {
  WiFi.disconnect(false, false);
  phase_ = Phase::Retry;
  phaseAt_ = millis();
  status_ = message;
}

void HomeNetwork::tick() {
  const uint32_t elapsed = millis() - phaseAt_;
  if (phase_ == Phase::Retry) {
    if (elapsed >= kRetryDelayMs) discover();
    return;
  }
  if (phase_ == Phase::Connected) {
    if (WiFi.status() != WL_CONNECTED) retry("Home Wi-Fi disconnected; retrying");
    return;
  }
  if (phase_ != Phase::Discovering && phase_ != Phase::FixedAddress) return;
  if (WiFi.status() == WL_CONNECTED) {
    if (phase_ == Phase::FixedAddress) {
      if (WiFi.localIP() != kReceiverAddress) {
        retry("Fixed address could not be assigned; retrying");
        return;
      }
      phase_ = Phase::Connected;
      status_ = "Home Wi-Fi connected";
      return;
    }
    const IPAddress gateway = WiFi.gatewayIP();
    const IPAddress mask = WiFi.subnetMask();
    const IPAddress dns = WiFi.dnsIP(0);
    const IPAddress zero(0, 0, 0, 0);
    if (gateway == zero || mask == zero ||
        !sameNetwork(kReceiverAddress, gateway, mask) ||
        !usableHost(kReceiverAddress, gateway, mask)) {
      retry("Fixed address is not usable on the discovered home subnet");
      return;
    }
    WiFi.disconnect(false, false);
    if (!WiFi.config(kReceiverAddress, gateway, mask, dns)) {
      retry("Cannot configure the fixed address; retrying");
      return;
    }
    WiFi.begin(ssid_.c_str(), password_.c_str());
    phase_ = Phase::FixedAddress;
    phaseAt_ = millis();
    status_ = "Applying fixed home address";
    return;
  }
  if (elapsed >= kConnectionTimeoutMs) retry("Home Wi-Fi connection timed out; retrying");
}

bool HomeNetwork::configure(Preferences &storage, const String &ssid,
                            const String &password, String &error) {
  if (!validCredentials(ssid, password)) {
    error = "SSID must be 1-32 bytes; password must be 8-63 bytes or 64 hex digits.";
    return false;
  }
  DynamicJsonDocument doc(1024);
  doc["ssid"] = ssid;
  doc["password"] = password;
  String encoded;
  if (doc.overflowed() || !serializeJson(doc, encoded) ||
      storage.putString("network", encoded) != encoded.length()) {
    error = "Could not save home Wi-Fi settings.";
    return false;
  }
  ssid_ = ssid;
  password_ = password;
  discover();
  return true;
}

bool HomeNetwork::clear(Preferences &storage, String &error) {
  if (storage.isKey("network") && !storage.remove("network")) {
    error = "Could not remove home Wi-Fi settings.";
    return false;
  }
  WiFi.disconnect(false, false);
  ssid_ = "";
  password_ = "";
  phase_ = Phase::Unconfigured;
  status_ = "Home Wi-Fi not configured";
  return true;
}
