#pragma once

#include "FR6311Commands.h"
#include "FR6311Transport.h"

struct AudioState {
  fr6311::Settings hardware;
  uint8_t masterAttenuation;
  uint8_t minimumAttenuation;
  uint8_t startupAttenuation;
  uint8_t volumeStep;
  uint16_t rampIntervalMs;
  bool restoreLastVolume;
  bool masterMuted;
  int8_t trims[6];
};

// Call begin, apply, and tick from the Arduino loop task only.
class AudioControl {
 public:
  static AudioState defaults();
  static bool valid(const AudioState &state);
  static void selectMultichannelInput(AudioState &state);
  static fr6311::Settings effective(const AudioState &state);
  bool begin(const AudioState &state);
  bool apply(const AudioState &state);
  bool tick();
  bool isRamping() const;
  const AudioState &state() const { return state_; }
  const fr6311::Settings &transmitted() const { return transmitted_; }

 private:
  void sendFrames(const uint32_t (&frames)[4]);
  FR6311Transport transport_;
  AudioState state_{};
  fr6311::Settings transmitted_{};
  uint32_t sent_[4]{};
  uint32_t lastRampStepAt_ = 0;
  bool initialized_ = false;
};
