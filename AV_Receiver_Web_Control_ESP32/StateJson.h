#pragma once

#include <ArduinoJson.h>
#include "AudioControl.h"

void writeState(JsonObject object, const AudioState &state);
bool readState(JsonObjectConst object, AudioState &state);
