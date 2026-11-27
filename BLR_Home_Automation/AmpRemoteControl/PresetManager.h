#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "AudioControl.h"

struct AudioPreset {
  String name;
  int8_t trims[6]{};
  uint8_t gains[6]{};
  bool mutes[6]{};
  int8_t bass = 0;
  int8_t treble = 0;
  bool bypass = false;
};

class PresetManager {
 public:
  static const unsigned kMaximum = 16;
  bool load(Preferences &storage);
  bool save(Preferences &storage) const;
  unsigned count() const { return count_; }
  const AudioPreset &at(unsigned index) const { return presets_[index]; }
  bool create(const String &name, const AudioState &state, String &error);
  bool update(const String &name, const AudioState &state, String &error);
  bool rename(const String &name, const String &newName, String &error);
  bool remove(const String &name, String &error);
  bool apply(const String &name, AudioState &state, String &error) const;

 private:
  int find(const String &name) const;
  static bool validName(const String &name);
  static void capture(AudioPreset &preset, const AudioState &state);
  AudioPreset presets_[kMaximum];
  unsigned count_ = 0;
};
