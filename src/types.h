#pragma once

#include <Arduino.h>

struct FanControl {
  uint16_t override = 0;
  uint16_t chtMin = 2100;  // *10 celcius
  uint16_t chtMax = 2400;  // *10 celcius
};

struct Telemetry {
  uint16_t fuel = 0;  // raw value
  uint16_t CHT = 0;   // * 10 celcius
  uint16_t EGT = 0;   // * 10 celcius
  uint16_t RPM = 0;
  uint16_t fanAmps = 0;  // Milliamps
  uint16_t fanCtrl = 0;
};
