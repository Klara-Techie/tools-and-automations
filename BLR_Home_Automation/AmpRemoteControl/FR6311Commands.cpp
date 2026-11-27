#include "FR6311Commands.h"

namespace fr6311 {
namespace {

uint32_t bits(uint8_t value, uint8_t width, uint8_t first) {
  uint32_t result = 0;
  for (uint8_t i = 0; i < width; ++i) {
    result |= static_cast<uint32_t>((value >> (width - 1 - i)) & 1U)
              << (first + i);
  }
  return result;
}

uint8_t toneCode(int8_t db) {
  return db >= 0 ? static_cast<uint8_t>(8 + db / 2)
                 : static_cast<uint8_t>(-db / 2);
}

uint32_t channelBits(const Channel &channel, uint8_t first) {
  const uint8_t volume = channel.muted ? 0x64 : channel.attenuation;
  return bits(channel.gain / 2, 3, first) |
         bits(volume, 7, first + 3);
}

}  // namespace

bool valid(const Settings &s) {
  if (s.input > 4 || s.recGain > 6 || s.recGain % 2 != 0 ||
      s.adcAttenuation != 0 ||
      (s.input == 4 && s.recEnabled) || s.bass < -14 || s.bass > 14 ||
      s.bass % 2 != 0 || s.treble < -14 || s.treble > 14 ||
      s.treble % 2 != 0 || s.inputGain > 14 || s.inputGain % 2 != 0) {
    return false;
  }
  for (const Channel &channel : s.channels) {
    if (channel.attenuation > 99 || channel.gain > 14 ||
        channel.gain % 2 != 0) {
      return false;
    }
  }
  return true;
}

bool encode(const Settings &s, uint32_t (&frames)[4]) {
  if (!valid(s)) return false;

  const uint8_t selectorCodes[] = {0, 2, 4, 6, 1};
  uint32_t a = bits(selectorCodes[s.input], 3, 0);
  if (s.recEnabled || s.input == 4) a |= 1UL << 3;
  a |= bits(s.recGain / 2, 2, 4);
  a |= bits(s.adcAttenuation / 6, 2, 6);
  if (s.leftRightMulti) a |= 1UL << 8;
  a |= bits(s.toneBypass ? 0 : toneCode(s.bass), 4, 9);
  a |= bits(toneCode(s.treble), 4, 13);
  if (s.surroundMulti) a |= 1UL << 17;
  a |= bits(s.inputGain / 2, 3, 18);

  frames[0] = a;
  frames[1] = channelBits(s.channels[0], 0) |
              channelBits(s.channels[1], 10) | (1UL << 23);
  frames[2] = channelBits(s.channels[2], 0) |
              channelBits(s.channels[3], 10) | (1UL << 22);
  frames[3] = channelBits(s.channels[4], 0) |
              channelBits(s.channels[5], 10) | (3UL << 22);
  return true;
}

}  // namespace fr6311
