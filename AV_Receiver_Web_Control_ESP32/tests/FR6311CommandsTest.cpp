#include "../FR6311Commands.h"

#include <cassert>
#include <cstdint>
#include <iostream>

static unsigned field(uint32_t frame, unsigned first, unsigned width) {
  unsigned value = 0;
  for (unsigned i = 0; i < width; ++i) {
    value = (value << 1) | ((frame >> (first + i)) & 1U);
  }
  return value;
}

int main() {
  fr6311::Settings s{};
  s.surroundMulti = true;
  uint32_t f[4];
  assert(fr6311::encode(s, f));
  assert(field(f[0], 21, 3) == 0);
  assert(field(f[1], 20, 4) == 1);
  assert(field(f[2], 20, 4) == 2);
  assert(field(f[3], 20, 4) == 3);

  const unsigned selectors[] = {0, 2, 4, 6, 1};
  for (unsigned input = 0; input <= 4; ++input) {
    s.input = input;
    assert(fr6311::encode(s, f));
    assert(field(f[0], 0, 3) == selectors[input]);
    assert(field(f[0], 3, 1) == (input == 4 ? 1U : 0U));
  }
  s.recEnabled = true;
  assert(!fr6311::encode(s, f));
  s.input = 1;
  assert(fr6311::encode(s, f));
  assert(field(f[0], 3, 1) == 1);

  for (int tone = -14; tone <= 14; tone += 2) {
    s.bass = tone;
    s.treble = tone;
    assert(fr6311::encode(s, f));
    unsigned code = tone < 0 ? -tone / 2 : 8 + tone / 2;
    assert(field(f[0], 9, 4) == code);
    assert(field(f[0], 13, 4) == code);
  }
  s.toneBypass = true;
  assert(fr6311::encode(s, f));
  assert(field(f[0], 9, 4) == 0);

  for (unsigned channel = 0; channel < 6; ++channel) {
    for (unsigned attenuation = 0; attenuation <= 99; ++attenuation) {
      s.channels[channel].attenuation = attenuation;
      assert(fr6311::encode(s, f));
      assert(field(f[1 + channel / 2], channel % 2 ? 13 : 3, 7) == attenuation);
    }
    for (unsigned gain = 0; gain <= 14; gain += 2) {
      s.channels[channel].gain = gain;
      assert(fr6311::encode(s, f));
      assert(field(f[1 + channel / 2], channel % 2 ? 10 : 0, 3) == gain / 2);
    }
    s.channels[channel].muted = true;
    assert(fr6311::encode(s, f));
    assert(field(f[1 + channel / 2], channel % 2 ? 13 : 3, 7) == 0x64);
  }
  s.channels[0].gain = 3;
  assert(!fr6311::encode(s, f));
  s.channels[0].gain = 0;
  s.bass = 1;
  assert(!fr6311::encode(s, f));
  s.bass = 0;
  s.adcAttenuation = 6;
  assert(!fr6311::encode(s, f));
  s.surroundMulti = false;
  assert(!fr6311::encode(s, f));
  std::cout << "FR6311 command tests passed\n";
}
