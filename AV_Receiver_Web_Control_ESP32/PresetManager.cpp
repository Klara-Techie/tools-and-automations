#include "PresetManager.h"

#include <ArduinoJson.h>

namespace {
bool readNumber(JsonVariantConst v, int lo, int hi, int &result) {
  if (!v.is<int>()) return false;
  int n = v.as<int>();
  if (n < lo || n > hi) return false;
  result = n;
  return true;
}
}

bool PresetManager::validName(const String &name) {
  if (name.length() < 1 || name.length() > 32 ||
      name[0] == ' ' || name[name.length() - 1] == ' ') return false;
  for (unsigned i = 0; i < name.length(); ++i) {
    if (name[i] < 32 || name[i] > 126) return false;
  }
  return true;
}

int PresetManager::find(const String &name) const {
  for (unsigned i = 0; i < count_; ++i) {
    if (presets_[i].name == name) return static_cast<int>(i);
  }
  return -1;
}

void PresetManager::capture(AudioPreset &preset, const AudioState &state) {
  preset.bass = state.hardware.bass;
  preset.treble = state.hardware.treble;
  preset.bypass = state.hardware.toneBypass;
  for (unsigned i = 0; i < 6; ++i) {
    preset.trims[i] = state.trims[i];
    preset.gains[i] = state.hardware.channels[i].gain;
    preset.mutes[i] = state.hardware.channels[i].muted;
  }
}

bool PresetManager::create(const String &name, const AudioState &state, String &error) {
  if (!validName(name)) { error = "Use 1-32 printable characters without outer spaces."; return false; }
  if (find(name) >= 0) { error = "Preset name already exists."; return false; }
  if (count_ >= kMaximum) { error = "Preset limit reached."; return false; }
  presets_[count_] = AudioPreset{};
  presets_[count_].name = name;
  capture(presets_[count_], state);
  ++count_;
  return true;
}

bool PresetManager::update(const String &name, const AudioState &state, String &error) {
  int index = find(name);
  if (index < 0) { error = "Preset not found."; return false; }
  capture(presets_[index], state);
  return true;
}

bool PresetManager::rename(const String &name, const String &newName, String &error) {
  int index = find(name);
  if (index < 0) { error = "Preset not found."; return false; }
  if (!validName(newName)) { error = "Use 1-32 printable characters without outer spaces."; return false; }
  if (newName != name && find(newName) >= 0) { error = "Preset name already exists."; return false; }
  presets_[index].name = newName;
  return true;
}

bool PresetManager::remove(const String &name, String &error) {
  int index = find(name);
  if (index < 0) { error = "Preset not found."; return false; }
  for (unsigned i = index; i + 1 < count_; ++i) presets_[i] = presets_[i + 1];
  --count_;
  presets_[count_] = AudioPreset{};
  return true;
}

bool PresetManager::apply(const String &name, AudioState &state, String &error) const {
  int index = find(name);
  if (index < 0) { error = "Preset not found."; return false; }
  const AudioPreset &p = presets_[index];
  state.hardware.bass = p.bass;
  state.hardware.treble = p.treble;
  state.hardware.toneBypass = p.bypass;
  for (unsigned i = 0; i < 6; ++i) {
    state.trims[i] = p.trims[i];
    state.hardware.channels[i].gain = p.gains[i];
    state.hardware.channels[i].muted = p.mutes[i];
  }
  if (!AudioControl::valid(state)) { error = "Preset contains invalid settings."; return false; }
  return true;
}

bool PresetManager::save(Preferences &storage) const {
  DynamicJsonDocument doc(12288);
  JsonArray list = doc.to<JsonArray>();
  for (unsigned i = 0; i < count_; ++i) {
    const AudioPreset &p = presets_[i];
    JsonObject item = list.createNestedObject();
    item["name"] = p.name;
    item["bass"] = p.bass;
    item["treble"] = p.treble;
    item["bypass"] = p.bypass;
    JsonArray trims = item.createNestedArray("trims");
    JsonArray gains = item.createNestedArray("gains");
    JsonArray mutes = item.createNestedArray("mutes");
    for (unsigned j = 0; j < 6; ++j) {
      trims.add(p.trims[j]);
      gains.add(p.gains[j]);
      mutes.add(p.mutes[j]);
    }
  }
  if (doc.overflowed()) return false;
  String encoded;
  if (!serializeJson(doc, encoded)) return false;
  return storage.putString("presets", encoded) == encoded.length();
}

bool PresetManager::load(Preferences &storage) {
  if (!storage.isKey("presets")) { *this = PresetManager{}; return true; }
  String encoded = storage.getString("presets", "");
  if (encoded.length() == 0 || encoded.length() > 8192) return false;
  DynamicJsonDocument doc(12288);
  if (deserializeJson(doc, encoded) || !doc.is<JsonArray>()) return false;
  JsonArrayConst list = doc.as<JsonArrayConst>();
  if (list.size() > kMaximum) return false;
  PresetManager parsed;
  for (JsonObjectConst item : list) {
    if (!item["name"].is<const char *>() || !item["bypass"].is<bool>()) return false;
    String name = item["name"].as<String>();
    if (!validName(name) || parsed.find(name) >= 0) return false;
    AudioPreset &p = parsed.presets_[parsed.count_];
    p.name = name;
    int n;
    if (!readNumber(item["bass"], -14, 14, n) || n % 2) return false;
    p.bass = n;
    if (!readNumber(item["treble"], -14, 14, n) || n % 2) return false;
    p.treble = n;
    p.bypass = item["bypass"].as<bool>();
    JsonArrayConst trims = item["trims"].as<JsonArrayConst>();
    JsonArrayConst gains = item["gains"].as<JsonArrayConst>();
    JsonArrayConst mutes = item["mutes"].as<JsonArrayConst>();
    if (trims.size() != 6 || gains.size() != 6 || mutes.size() != 6) return false;
    for (unsigned i = 0; i < 6; ++i) {
      if (!readNumber(trims[i], -99, 99, n)) return false;
      p.trims[i] = n;
      if (!readNumber(gains[i], 0, 14, n) || n % 2) return false;
      p.gains[i] = n;
      if (!mutes[i].is<bool>()) return false;
      p.mutes[i] = mutes[i].as<bool>();
    }
    ++parsed.count_;
  }
  *this = parsed;
  return true;
}
