#pragma once

#include <Adafruit_NeoPixel.h>
#include <KnobLightSettings.h>

static_assert(KNOB_LEDS_PER_RING > 0 && KNOB_LEDS_PER_RING <= 60,
              "Use 1-60 LEDs per knob ring.");
static_assert(KNOB_LED_MAX_BRIGHTNESS > 0 && KNOB_LED_MAX_BRIGHTNESS <= 255,
              "LED brightness ceiling must be 1-255.");

class KnobLights {
 public:
  bool begin() {
    const int pin = KNOB_LED_PIN;
    if ((pin != 4 && pin != 5 && pin != 12 && pin != 13 &&
         pin != 14) || pin == IR_LED_PIN) {
      Serial.println("[ERROR] Knob LEDs disabled: invalid or conflicting GPIO");
      return false;
    }
    if (pixels_.numPixels() != 2 * KNOB_LEDS_PER_RING) {
      Serial.println("[ERROR] Knob LED buffer allocation failed");
      return false;
    }
    pixels_.begin();
    ready_ = true;
    off();
    Serial.printf("[INFO] Knob LEDs ready: GPIO %d, %u LEDs, startup OFF\n",
                  pin, static_cast<unsigned>(pixels_.numPixels()));
    return true;
  }

  bool ready() const { return ready_; }
  const KnobLightSettings& settings() const { return settings_; }

  void apply(const KnobLightSettings& settings) {
    settings_ = settings;
    startedAt_ = millis();
    dirty_ = true;
    tick();
  }

  void off() {
    KnobLightSettings next = settings_;
    next.on = false;
    apply(next);
  }

  void tick() {
    if (!ready_) return;
    const uint32_t now = millis();
    if (!dirty_ && (!settings_.on || settings_.pattern == 0 ||
                    uint32_t(now - frameAt_) < 40)) return;
    frameAt_ = now;
    dirty_ = false;
    const uint32_t elapsed = now - startedAt_;
    const unsigned head = (elapsed / 100) % KNOB_LEDS_PER_RING;
    for (unsigned i = 0; i < pixels_.numPixels(); ++i) {
      const unsigned position = i % KNOB_LEDS_PER_RING;
      uint32_t color = settings_.color;
      unsigned strength = settings_.on ? 255 : 0;
      if (settings_.on) {
        switch (settings_.pattern) {
          case 1: {
            const unsigned phase = (elapsed / 8) % 512;
            strength = phase <= 255 ? phase : 511 - phase;
            break;
          }
          case 2:
            color = Adafruit_NeoPixel::ColorHSV(
                static_cast<uint16_t>(elapsed * 16 +
                                     position * 65536UL / KNOB_LEDS_PER_RING));
            break;
          case 3:
            strength = (position + elapsed / 140) % 3 == 0 ? 255 : 0;
            break;
          case 4: {
            const unsigned distance =
                (head + KNOB_LEDS_PER_RING - position) % KNOB_LEDS_PER_RING;
            strength = distance < 4 ? 255 >> distance : 0;
            break;
          }
          case 5:
            color = Adafruit_NeoPixel::ColorHSV(static_cast<uint16_t>(elapsed * 8));
            break;
          case 6:
            color = Adafruit_NeoPixel::ColorHSV(
                26000 + smoothNoise(position, elapsed, 1800) * 70);
            strength = 70 + smoothNoise(position + 31, elapsed, 2200) * 185 / 255;
            break;
          case 7: {
            const unsigned phase =
                (elapsed / 18 + position * 512 / KNOB_LEDS_PER_RING) % 512;
            const unsigned wave = phase <= 255 ? phase : 511 - phase;
            color = Adafruit_NeoPixel::ColorHSV(33000 + wave * 35);
            strength = 50 + wave * 130 / 255;
            break;
          }
          case 8:
            color = Adafruit_NeoPixel::ColorHSV(
                1400 + smoothNoise(position + 17, elapsed, 720) * 3500 / 255);
            strength = 100 + smoothNoise(position, elapsed, 360) * 155 / 255;
            break;
          case 9:
            color = 0xff9b32;
            strength = 170 + smoothNoise(0, elapsed, 650) * 85 / 255;
            break;
          case 10: {
            const unsigned sparkle = smoothNoise(position, elapsed, 950);
            strength = sparkle > 150 ? 30 + (sparkle - 150) * 225 / 105 : 30;
            break;
          }
          case 11: {
            const unsigned span = KNOB_LEDS_PER_RING - 1;
            const unsigned travel = span ? (elapsed / 110) % (2 * span) : 0;
            const unsigned spot = travel <= span ? travel : 2 * span - travel;
            const unsigned distance = position > spot ? position - spot : spot - position;
            strength = distance < 3 ? 255 >> distance : 8;
            break;
          }
        }
      }
      const unsigned level = uint32_t(strength) * settings_.brightness *
                             KNOB_LED_MAX_BRIGHTNESS / 25500UL;
      pixels_.setPixelColor(i, ((color >> 16) & 255) * level / 255,
                            ((color >> 8) & 255) * level / 255,
                            (color & 255) * level / 255);
    }
    pixels_.show();
  }

 private:
  static unsigned smoothNoise(unsigned position, uint32_t elapsed, unsigned period) {
    const auto sample = [position](uint32_t step) -> unsigned {
      uint32_t value = step * 747796405UL + position * 2891336453UL;
      value ^= value >> 16;
      value *= 2246822519UL;
      return (value ^ (value >> 13)) & 255;
    };
    const uint32_t step = elapsed / period;
    const unsigned fraction = elapsed % period;
    return (sample(step) * (period - fraction) + sample(step + 1) * fraction) / period;
  }

  Adafruit_NeoPixel pixels_{2 * KNOB_LEDS_PER_RING, KNOB_LED_PIN, NEO_GRB + NEO_KHZ800};
  KnobLightSettings settings_;
  uint32_t startedAt_ = 0;
  uint32_t frameAt_ = 0;
  bool dirty_ = true;
  bool ready_ = false;
};
