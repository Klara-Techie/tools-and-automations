#include "AudioControl.h"

AudioState AudioControl::defaults() {
  AudioState state{};
  state.hardware.input = 1;
  state.hardware.leftRightMulti = true;
  state.hardware.surroundMulti = true;
  state.masterAttenuation = 58;
  state.startupAttenuation = 58;
  state.volumeStep = 1;
  state.rampIntervalMs = 20;
  state.restoreLastVolume = true;
  state.masterMuted = true;
  return state;
}

bool AudioControl::valid(const AudioState &s) {
  if (s.masterAttenuation > 99 || s.minimumAttenuation > 99 ||
      s.startupAttenuation > 99 || s.volumeStep < 1 || s.volumeStep > 10 ||
      s.rampIntervalMs < 5 || s.rampIntervalMs > 1000 ||
      s.masterAttenuation < s.minimumAttenuation ||
      s.startupAttenuation < s.minimumAttenuation ||
      s.hardware.adcAttenuation != 0 ||
      !fr6311::valid(s.hardware)) {
    return false;
  }
  for (int8_t trim : s.trims) {
    if (trim < -99 || trim > 99) return false;
  }
  return true;
}

void AudioControl::selectMultichannelInput(AudioState &state) {
  state.hardware.leftRightMulti = true;
  state.hardware.surroundMulti = true;
  state.hardware.recEnabled = false;
}

namespace {
bool supportedRoute(const AudioState &state) {
  return state.hardware.leftRightMulti && state.hardware.surroundMulti &&
         !state.hardware.recEnabled;
}
}

fr6311::Settings AudioControl::effective(const AudioState &s) {
  fr6311::Settings hardware = s.hardware;
  for (unsigned i = 0; i < 6; ++i) {
    int attenuation = static_cast<int>(s.masterAttenuation) + s.trims[i];
    if (attenuation < s.minimumAttenuation) attenuation = s.minimumAttenuation;
    if (attenuation > 99) attenuation = 99;
    hardware.channels[i].attenuation = static_cast<uint8_t>(attenuation);
    hardware.channels[i].muted |= s.masterMuted;
  }
  return hardware;
}

bool AudioControl::begin(const AudioState &state) {
  transport_.begin();
  const fr6311::Settings initial = effective(state);
  uint32_t frames[4];
  if (!state.masterMuted || !valid(state) || !supportedRoute(state) ||
      !fr6311::encode(initial, frames)) return false;
  // Initialize the volume formats muted before enabling the input path.
  for (unsigned i = 1; i < 4; ++i) {
    transport_.write(frames[i]);
    sent_[i] = frames[i];
  }
  transport_.write(frames[0]);
  sent_[0] = frames[0];
  state_ = state;
  transmitted_ = initial;
  lastRampStepAt_ = millis();
  initialized_ = true;
  return true;
}

void AudioControl::sendFrames(const uint32_t (&frames)[4]) {
  for (unsigned i = 0; i < 4; ++i) {
    if (frames[i] != sent_[i]) {
      transport_.write(frames[i]);
      sent_[i] = frames[i];
    }
  }
}

bool AudioControl::apply(const AudioState &state) {
  if (!initialized_ || !valid(state) || !supportedRoute(state)) return false;

  const fr6311::Settings target = effective(state);
  fr6311::Settings safety = transmitted_;
  bool resetRampTimer = false;
  for (unsigned i = 0; i < 6; ++i) {
    fr6311::Channel &channel = safety.channels[i];
    if (target.channels[i].muted) channel.muted = true;
    if (!channel.muted && channel.attenuation < state.minimumAttenuation) {
      channel.attenuation = state.minimumAttenuation;
      resetRampTimer = true;
    }
    if (!channel.muted && target.channels[i].gain > channel.gain) {
      const unsigned adjusted = channel.attenuation +
          (target.channels[i].gain - channel.gain);
      channel.attenuation = adjusted > 99 ? 99 : static_cast<uint8_t>(adjusted);
      resetRampTimer = true;
    }
  }

  fr6311::Settings next = target;
  for (unsigned i = 0; i < 6; ++i) {
    if (!target.channels[i].muted && safety.channels[i].muted) {
      next.channels[i].attenuation = 99;
      resetRampTimer = true;
    } else {
      next.channels[i].attenuation = safety.channels[i].attenuation;
    }
  }

  uint32_t safetyFrames[4];
  uint32_t nextFrames[4];
  if (!fr6311::encode(safety, safetyFrames) ||
      !fr6311::encode(next, nextFrames)) return false;

  // Apply mutes and tighter limits before changing routing, gain, or unmuting.
  sendFrames(safetyFrames);
  sendFrames(nextFrames);
  transmitted_ = next;
  state_ = state;
  if (resetRampTimer) lastRampStepAt_ = millis();
  return true;
}

bool AudioControl::isRamping() const {
  if (!initialized_) return false;
  const fr6311::Settings target = effective(state_);
  for (unsigned i = 0; i < 6; ++i) {
    if (!target.channels[i].muted &&
        transmitted_.channels[i].attenuation != target.channels[i].attenuation) {
      return true;
    }
  }
  return false;
}

bool AudioControl::tick() {
  if (!initialized_) return false;
  const uint32_t now = millis();
  if (now - lastRampStepAt_ < state_.rampIntervalMs || !isRamping()) return true;

  const fr6311::Settings target = effective(state_);
  fr6311::Settings next = transmitted_;
  for (unsigned i = 0; i < 6; ++i) {
    fr6311::Channel &channel = next.channels[i];
    if (channel.muted) continue;
    if (channel.attenuation < target.channels[i].attenuation) {
      ++channel.attenuation;
    } else if (channel.attenuation > target.channels[i].attenuation) {
      --channel.attenuation;
    }
  }
  uint32_t frames[4];
  if (!fr6311::encode(next, frames)) return false;
  sendFrames(frames);
  transmitted_ = next;
  lastRampStepAt_ = now;
  return true;
}
