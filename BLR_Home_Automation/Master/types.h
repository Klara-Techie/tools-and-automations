#pragma once
#include <WiFi.h>

struct Node {
  const char* id;
  IPAddress address;
  uint32_t lastSeen;
  uint32_t state;
  uint32_t configured;
  bool online;
};

struct Appliance {
  const char* name;
  uint8_t node;
  uint8_t channel;
};
