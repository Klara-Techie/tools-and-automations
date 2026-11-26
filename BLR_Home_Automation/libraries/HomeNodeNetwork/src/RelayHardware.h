#pragma once

#include <Arduino.h>

inline bool validRelayPin(int pin, bool input) {
  if (pin == -1) return true;
#if defined(ESP8266)
  if (pin < 0 || pin > 16 || (pin >= 6 && pin <= 11)) return false;
  if (input && pin == 16) return false;
  if (pin == 1 || pin == 3) return false;
#else
  if (pin < 0 || pin > 39 || (pin >= 6 && pin <= 11)) return false;
  if (pin == 20 || pin == 24 || (pin >= 28 && pin <= 31)) return false;
  if (!input && pin >= 34) return false;
  if (input && pin >= 34) return false;  // No internal pull-up.
#endif
  return true;
}
