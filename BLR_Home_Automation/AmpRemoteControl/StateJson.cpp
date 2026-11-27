#include "StateJson.h"

namespace {
bool number(JsonVariantConst value, int minimum, int maximum, int &out) {
  if (!value.is<int>()) return false;
  const int n = value.as<int>();
  if (n < minimum || n > maximum) return false;
  out = n;
  return true;
}

bool flag(JsonVariantConst value, bool &out) {
  if (!value.is<bool>()) return false;
  out = value.as<bool>();
  return true;
}
}

void writeState(JsonObject o, const AudioState &s) {
  o["master"] = s.masterAttenuation;
  o["limit"] = s.minimumAttenuation;
  o["startup"] = s.startupAttenuation;
  o["step"] = s.volumeStep;
  o["rampMs"] = s.rampIntervalMs;
  o["restore"] = s.restoreLastVolume;
  o["mute"] = s.masterMuted;
  o["input"] = s.hardware.input;
  o["rec"] = s.hardware.recEnabled;
  o["recGain"] = s.hardware.recGain;
  o["adc"] = s.hardware.adcAttenuation;
  o["lrMulti"] = s.hardware.leftRightMulti;
  o["surroundMulti"] = s.hardware.surroundMulti;
  o["bass"] = s.hardware.bass;
  o["treble"] = s.hardware.treble;
  o["bypass"] = s.hardware.toneBypass;
  o["inputGain"] = s.hardware.inputGain;
  JsonArray trims = o.createNestedArray("trims");
  JsonArray gains = o.createNestedArray("gains");
  JsonArray mutes = o.createNestedArray("mutes");
  for (unsigned i = 0; i < 6; ++i) {
    trims.add(s.trims[i]);
    gains.add(s.hardware.channels[i].gain);
    mutes.add(s.hardware.channels[i].muted);
  }
}

bool readState(JsonObjectConst o, AudioState &state) {
  AudioState s = AudioControl::defaults();
  int n;
#define READ_NUMBER(key, member, lo, hi) \
  if (!number(o[key], lo, hi, n)) return false; \
  member = n
#define READ_FLAG(key, member) if (!flag(o[key], member)) return false
  READ_NUMBER("master", s.masterAttenuation, 0, 99);
  READ_NUMBER("limit", s.minimumAttenuation, 0, 99);
  READ_NUMBER("startup", s.startupAttenuation, 0, 99);
  READ_NUMBER("step", s.volumeStep, 1, 10);
  if (o.containsKey("rampMs")) {
    READ_NUMBER("rampMs", s.rampIntervalMs, 5, 1000);
  }
  READ_FLAG("restore", s.restoreLastVolume);
  READ_FLAG("mute", s.masterMuted);
  READ_NUMBER("input", s.hardware.input, 0, 4);
  READ_FLAG("rec", s.hardware.recEnabled);
  READ_NUMBER("recGain", s.hardware.recGain, 0, 6);
  // The format and setting tables disagree on the ADC bit positions.
  READ_NUMBER("adc", s.hardware.adcAttenuation, 0, 0);
  READ_FLAG("lrMulti", s.hardware.leftRightMulti);
  READ_FLAG("surroundMulti", s.hardware.surroundMulti);
  READ_NUMBER("bass", s.hardware.bass, -14, 14);
  READ_NUMBER("treble", s.hardware.treble, -14, 14);
  READ_FLAG("bypass", s.hardware.toneBypass);
  READ_NUMBER("inputGain", s.hardware.inputGain, 0, 14);
  JsonArrayConst trims = o["trims"].as<JsonArrayConst>();
  JsonArrayConst gains = o["gains"].as<JsonArrayConst>();
  JsonArrayConst mutes = o["mutes"].as<JsonArrayConst>();
  if (trims.size() != 6 || gains.size() != 6 || mutes.size() != 6) return false;
  for (unsigned i = 0; i < 6; ++i) {
    if (!number(trims[i], -99, 99, n)) return false;
    s.trims[i] = n;
    if (!number(gains[i], 0, 14, n)) return false;
    s.hardware.channels[i].gain = n;
    if (!flag(mutes[i], s.hardware.channels[i].muted)) return false;
  }
#undef READ_NUMBER
#undef READ_FLAG
  if (!AudioControl::valid(s)) return false;
  state = s;
  return true;
}
