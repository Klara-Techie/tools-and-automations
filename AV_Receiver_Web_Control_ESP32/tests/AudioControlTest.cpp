#include "../AudioControl.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {
uint32_t nowMs = 0;
std::vector<uint32_t> writes;

unsigned field(uint32_t frame, unsigned first, unsigned width) {
  unsigned value = 0;
  for (unsigned i = 0; i < width; ++i) {
    value = (value << 1) | ((frame >> (first + i)) & 1U);
  }
  return value;
}

void advance(AudioControl &audio, uint32_t milliseconds) {
  nowMs += milliseconds;
  assert(audio.tick());
}

AudioState nearMute() {
  AudioState state = AudioControl::defaults();
  state.masterAttenuation = 97;
  state.startupAttenuation = 97;
  state.rampIntervalMs = 20;
  return state;
}
}

unsigned long millis() { return nowMs; }
void FR6311Transport::begin() {}
void FR6311Transport::write(uint32_t frame) { writes.push_back(frame); }

int main() {
  const AudioState defaults = AudioControl::defaults();
  assert(defaults.masterAttenuation == 58);
  assert(defaults.startupAttenuation == 58);
  assert(defaults.hardware.bass == 0 && defaults.hardware.treble == 0);
  assert(defaults.hardware.inputGain == 0);
  assert(defaults.hardware.leftRightMulti && defaults.hardware.surroundMulti);
  uint32_t defaultFrames[4];
  assert(fr6311::encode(defaults.hardware, defaultFrames));
  assert((defaultFrames[0] & (1UL << 8)) != 0);
  assert((defaultFrames[0] & (1UL << 17)) != 0);
  AudioState previous = defaults;
  previous.hardware.leftRightMulti = false;
  previous.hardware.surroundMulti = false;
  previous.hardware.recEnabled = true;
  AudioControl::selectMultichannelInput(previous);
  assert(previous.hardware.leftRightMulti && previous.hardware.surroundMulti);
  assert(!previous.hardware.recEnabled);
  for (const fr6311::Channel &channel : defaults.hardware.channels) {
    assert(channel.gain == 0);
  }

  AudioControl audio;
  AudioState state = nearMute();
  assert(audio.begin(state));
  assert(writes.size() == 4);
  assert(!audio.isRamping());

  AudioState unsupported = state;
  unsupported.hardware.leftRightMulti = false;
  assert(!audio.apply(unsupported));
  unsupported = state;
  unsupported.hardware.surroundMulti = false;
  assert(!audio.apply(unsupported));
  unsupported = state;
  unsupported.hardware.recEnabled = true;
  assert(!audio.apply(unsupported));
  assert(writes.size() == 4);

  writes.clear();
  state.masterMuted = false;
  assert(audio.apply(state));
  assert(writes.size() == 3);
  for (unsigned i = 0; i < 6; ++i) {
    assert(!audio.transmitted().channels[i].muted);
    assert(audio.transmitted().channels[i].attenuation == 99);
  }
  assert(audio.isRamping());
  writes.clear();
  advance(audio, 19);
  assert(writes.empty());
  advance(audio, 1);
  assert(writes.size() == 3);
  for (unsigned i = 0; i < 6; ++i) {
    assert(audio.transmitted().channels[i].attenuation == 98);
  }

  // A new target reverses the existing ramp instead of starting another one.
  writes.clear();
  state.masterAttenuation = 99;
  assert(audio.apply(state));
  assert(writes.empty());
  advance(audio, 20);
  assert(audio.transmitted().channels[0].attenuation == 99);
  assert(!audio.isRamping());
  writes.clear();
  advance(audio, 20);
  assert(writes.empty());

  // A new interval takes effect without skipping 1 dB steps.
  state.masterAttenuation = 97;
  state.rampIntervalMs = 40;
  assert(audio.apply(state));
  writes.clear();
  advance(audio, 19);
  assert(writes.empty());
  advance(audio, 1);
  assert(audio.transmitted().channels[0].attenuation == 98);

  // A tightened volume limit is enforced immediately, before any ramp.
  writes.clear();
  state.masterAttenuation = 99;
  state.startupAttenuation = 99;
  state.minimumAttenuation = 99;
  assert(audio.apply(state));
  assert(writes.size() == 3);
  assert(audio.transmitted().channels[0].attenuation == 99);
  assert(!audio.isRamping());

  // Muting one channel precedes a simultaneous input change.
  state.minimumAttenuation = 0;
  state.startupAttenuation = 97;
  state.masterAttenuation = 97;
  assert(audio.apply(state));
  writes.clear();
  state.hardware.channels[0].muted = true;
  state.hardware.input = 2;
  assert(audio.apply(state));
  assert(writes.size() >= 2);
  assert(field(writes[0], 20, 4) == 1);
  assert(field(writes[0], 3, 7) == 0x64);
  assert(field(writes[1], 21, 3) == 0);

  // Master mute interrupts all channels immediately; future ticks stay muted.
  writes.clear();
  state.masterMuted = true;
  assert(audio.apply(state));
  for (const fr6311::Channel &channel : audio.transmitted().channels) {
    assert(channel.muted);
  }
  writes.clear();
  advance(audio, 1000);
  assert(writes.empty());
  assert(!audio.isRamping());

  // Changes while muted update only the target. Unmute starts at -99 dB.
  state.masterAttenuation = 50;
  assert(audio.apply(state));
  assert(writes.empty());
  state.masterMuted = false;
  state.hardware.channels[0].muted = false;
  assert(audio.apply(state));
  assert(audio.transmitted().channels[0].attenuation == 99);
  assert(audio.isRamping());

  AudioState invalid = state;
  invalid.rampIntervalMs = 4;
  writes.clear();
  assert(!audio.apply(invalid));
  assert(writes.empty());
  invalid = state;
  invalid.rampIntervalMs = 1001;
  assert(!audio.apply(invalid));
  assert(writes.empty());
  invalid = state;
  invalid.hardware.adcAttenuation = 6;
  assert(!audio.apply(invalid));
  assert(writes.empty());

  // Channels stop independently at their trimmed targets.
  nowMs = 100;
  AudioControl trimmed;
  state = nearMute();
  state.rampIntervalMs = 5;
  state.trims[1] = 1;
  state.trims[2] = -1;
  assert(trimmed.begin(state));
  state.masterMuted = false;
  assert(trimmed.apply(state));
  advance(trimmed, 5);
  advance(trimmed, 5);
  assert(trimmed.transmitted().channels[0].attenuation == 97);
  assert(trimmed.transmitted().channels[1].attenuation == 98);
  assert(trimmed.transmitted().channels[2].attenuation == 97);
  assert(trimmed.isRamping());
  advance(trimmed, 5);
  assert(trimmed.transmitted().channels[0].attenuation == 97);
  assert(trimmed.transmitted().channels[1].attenuation == 98);
  assert(trimmed.transmitted().channels[2].attenuation == 96);
  assert(!trimmed.isRamping());

  // Increasing output gain first reduces the channel level, then ramps back.
  state.hardware.channels[0].gain = 2;
  writes.clear();
  assert(trimmed.apply(state));
  assert(trimmed.transmitted().channels[0].attenuation == 99);
  assert(trimmed.transmitted().channels[0].gain == 2);
  assert(writes.size() == 2);
  assert(field(writes[0], 3, 7) == 99);
  assert(field(writes[0], 0, 3) == 0);
  assert(field(writes[1], 0, 3) == 1);
  advance(trimmed, 5);
  assert(trimmed.transmitted().channels[0].attenuation == 98);

  // Unsigned elapsed time remains correct when millis() wraps.
  nowMs = UINT32_MAX - 10;
  AudioControl wrapping;
  state = nearMute();
  assert(wrapping.begin(state));
  state.masterMuted = false;
  assert(wrapping.apply(state));
  advance(wrapping, 19);
  assert(wrapping.transmitted().channels[0].attenuation == 99);
  advance(wrapping, 1);
  assert(wrapping.transmitted().channels[0].attenuation == 98);

  std::cout << "Audio control tests passed\n";
}
