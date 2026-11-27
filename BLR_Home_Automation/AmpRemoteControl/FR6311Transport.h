#pragma once

#include <Arduino.h>

class FR6311Transport {
 public:
  void begin();
  void write(uint32_t frame);
};
