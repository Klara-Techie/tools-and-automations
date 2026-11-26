#pragma once

#include <RelayHardware.h>

const uint32_t MAX_RELAY_TIMER_SECONDS = 604800UL;

class EntertainmentRelays {
 public:
  EntertainmentRelays(const int (&pins)[3], bool activeLow)
      : pins_{pins[0], pins[1], pins[2]}, activeLow_(activeLow) {}

  bool begin() {
    configured_ = 0;
    for (size_t i = 0; i < 3; ++i) {
      if (!validRelayPin(pins_[i], false)) {
        Serial.println("[ERROR] Invalid entertainment relay GPIO; outputs disabled");
        return false;
      }
      for (size_t j = 0; j < i; ++j) {
        if (pins_[i] >= 0 && pins_[i] == pins_[j]) {
          Serial.println("[ERROR] Duplicate entertainment relay GPIO; outputs disabled");
          return false;
        }
      }
    }
    for (size_t i = 0; i < 3; ++i) {
      if (pins_[i] < 0) continue;
      digitalWrite(pins_[i], activeLow_ ? HIGH : LOW);
      pinMode(pins_[i], OUTPUT);
      configured_ |= 1UL << i;
    }
    off();
    return true;
  }

  uint32_t configuredMask() const { return configured_; }
  uint32_t stateMask() const { return state_; }

  bool setRelay(size_t channel, bool on) {
    if (channel >= 3 || !(configured_ & (1UL << channel))) {
      Serial.println("[WARN] Entertainment relay is not configured");
      return false;
    }
    timerDuration_[channel] = 0;
    if (on) state_ |= 1UL << channel;
    else state_ &= ~(1UL << channel);
    digitalWrite(pins_[channel], (on == activeLow_) ? LOW : HIGH);
    Serial.printf("[INFO] Entertainment channel %u %s\n",
                  static_cast<unsigned>(channel), on ? "ON" : "OFF");
    return true;
  }

  bool setAll(bool on) {
    if (configured_ != 7) {
      Serial.println("[WARN] Entertainment group is not fully configured");
      return false;
    }
    allTimerDuration_ = 0;
    for (size_t i = 0; i < 3; ++i) setRelay(i, on);
    return true;
  }

  void off() {
    allTimerDuration_ = 0;
    for (size_t i = 0; i < 3; ++i) {
      timerDuration_[i] = 0;
      if (configured_ & (1UL << i)) setRelay(i, false);
    }
    state_ = 0;
  }

  // Target 3 is the group OFF timer; zero seconds cancels.
  bool setTimer(size_t target, uint32_t seconds) {
    if (target > 3 || seconds > MAX_RELAY_TIMER_SECONDS) {
      Serial.println("[WARN] Invalid entertainment timer");
      return false;
    }
    const uint32_t required = target == 3 ? 7UL : 1UL << target;
    if ((configured_ & required) != required) {
      Serial.println("[WARN] Entertainment timer channel is not configured");
      return false;
    }
    if (target == 3) {
      for (size_t i = 0; i < 3; ++i) timerDuration_[i] = 0;
      allTimerStartedAt_ = millis();
      allTimerDuration_ = seconds * 1000UL;
    } else {
      timerStartedAt_[target] = millis();
      timerDuration_[target] = seconds * 1000UL;
    }
    return true;
  }

  uint32_t remainingSeconds(size_t target) const {
    if (target > 3) {
      Serial.println("[WARN] Invalid entertainment timer target");
      return 0;
    }
    const uint32_t duration = target == 3 ? allTimerDuration_ : timerDuration_[target];
    const uint32_t start = target == 3 ? allTimerStartedAt_ : timerStartedAt_[target];
    if (!duration) return 0;
    const uint32_t elapsed = millis() - start;
    return elapsed >= duration ? 0 : (duration - elapsed + 999UL) / 1000UL;
  }

  bool updateTimers() {
    bool changed = false;
    const uint32_t now = millis();
    for (size_t i = 0; i < 3; ++i) {
      if (timerDuration_[i] && uint32_t(now - timerStartedAt_[i]) >= timerDuration_[i]) {
        setRelay(i, false);
        changed = true;
      }
    }
    if (allTimerDuration_ && uint32_t(now - allTimerStartedAt_) >= allTimerDuration_) {
      off();
      changed = true;
    }
    return changed;
  }

 private:
  const int pins_[3];
  const bool activeLow_;
  uint32_t configured_ = 0;
  uint32_t state_ = 0;
  uint32_t timerStartedAt_[3] = {};
  uint32_t timerDuration_[3] = {};
  uint32_t allTimerStartedAt_ = 0;
  uint32_t allTimerDuration_ = 0;
};
