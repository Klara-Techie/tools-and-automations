#pragma once

#include <Arduino.h>

const char* const KNOB_PATTERNS[] = {
  "solid", "breathing", "rainbow", "chase", "comet", "color_cycle",
  "aurora", "ocean", "fire", "candle", "twinkle", "pendulum"
};
const size_t KNOB_PATTERN_COUNT = sizeof(KNOB_PATTERNS) / sizeof(KNOB_PATTERNS[0]);

struct KnobLightSettings {
  bool on = false;
  uint32_t color = 0x8be0d5;
  uint8_t brightness = 40;
  uint8_t pattern = 0;
};

inline bool parseKnobLights(const String& on, const String& color,
                           const String& brightness, const String& pattern,
                           KnobLightSettings& result) {
  if ((on != "0" && on != "1") || color.length() != 6 ||
      !brightness.length() || brightness.length() > 3) return false;
  uint32_t rgb = 0;
  for (size_t i = 0; i < color.length(); ++i) {
    const char c = color[i];
    const int digit = c >= '0' && c <= '9' ? c - '0' :
                      c >= 'a' && c <= 'f' ? c - 'a' + 10 :
                      c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1;
    if (digit < 0) return false;
    rgb = (rgb << 4) | digit;
  }
  unsigned level = 0;
  for (size_t i = 0; i < brightness.length(); ++i) {
    if (brightness[i] < '0' || brightness[i] > '9') return false;
    level = level * 10 + brightness[i] - '0';
  }
  if (level > 100) return false;
  for (size_t i = 0; i < KNOB_PATTERN_COUNT; ++i) {
    if (pattern != KNOB_PATTERNS[i]) continue;
    result.on = on == "1";
    result.color = rgb;
    result.brightness = level;
    result.pattern = i;
    return true;
  }
  return false;
}

inline String knobColorHex(uint32_t color) {
  char value[7];
  snprintf(value, sizeof(value), "%06lx", static_cast<unsigned long>(color));
  return String(value);
}

inline String knobSettingsBody(const KnobLightSettings& settings) {
  return String("on=") + (settings.on ? "1" : "0") +
         "&color=" + knobColorHex(settings.color) +
         "&brightness=" + settings.brightness +
         "&pattern=" + KNOB_PATTERNS[settings.pattern];
}
