#pragma once

#include <Arduino.h>

struct FanControl {
  uint16_t override = 0;
  uint16_t chtMin = 210;
  uint16_t chtMax = 230;
};

struct Telemetry {
  uint16_t fuel = 0;
  uint16_t CHT = 0;
  uint16_t EGT = 0;
  uint16_t RPM = 0;
  uint16_t fanAmps = 0;  // Milliamps
  uint16_t fanCtrl = 0;
};
