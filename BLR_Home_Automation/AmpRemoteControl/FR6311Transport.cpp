#include "FR6311Transport.h"

namespace {
constexpr uint8_t kDataPin = 16;
constexpr uint8_t kClockPin = 17;
constexpr unsigned int kDelayUs = 5;
}

void FR6311Transport::begin() {
  digitalWrite(kDataPin, LOW);
  digitalWrite(kClockPin, LOW);
  pinMode(kDataPin, OUTPUT);
  pinMode(kClockPin, OUTPUT);
}

void FR6311Transport::write(uint32_t frame) {
  for (uint8_t bit = 0; bit < 24; ++bit) {
    digitalWrite(kDataPin, (frame >> bit) & 1U);
    delayMicroseconds(kDelayUs);
    digitalWrite(kClockPin, HIGH);
    delayMicroseconds(kDelayUs);
    digitalWrite(kDataPin, LOW);
    delayMicroseconds(kDelayUs);
    if (bit != 23) {
      digitalWrite(kClockPin, LOW);
      delayMicroseconds(kDelayUs);
    }
  }
  digitalWrite(kDataPin, HIGH);
  delayMicroseconds(kDelayUs);
  digitalWrite(kClockPin, LOW);
  delayMicroseconds(kDelayUs);
  digitalWrite(kDataPin, LOW);
  delayMicroseconds(kDelayUs);
}
