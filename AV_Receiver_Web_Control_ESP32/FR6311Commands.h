#pragma once

#include <stdint.h>

namespace fr6311 {

struct Channel {
  uint8_t attenuation;
  uint8_t gain;
  bool muted;
};

struct Settings {
  uint8_t input;
  bool recEnabled;
  uint8_t recGain;
  uint8_t adcAttenuation;
  bool leftRightMulti;
  bool surroundMulti;
  int8_t bass;
  int8_t treble;
  bool toneBypass;
  uint8_t inputGain;
  Channel channels[6];
};

// Channel order is L, R, C, SW, SL, SR.
bool valid(const Settings &settings);
bool encode(const Settings &settings, uint32_t (&frames)[4]);

}  // namespace fr6311
