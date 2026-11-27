#include "HomeNetwork.h"
#include "config.h"

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

void HomeNetwork::begin() {
  WiFi.persistent(false);
  WiFi.setAutoReconnect(false);
  const String ssid = HOME_SSID;
  const String password = HOME_PASSWORD;
  if (ssid.startsWith("REPLACE_") || password.startsWith("REPLACE_") ||
      !validCredentials(ssid, password)) {
    status_ = "Home Wi-Fi settings in config.h are invalid";
    Serial.println("[ERROR] Home Wi-Fi settings in config.h are invalid");
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
