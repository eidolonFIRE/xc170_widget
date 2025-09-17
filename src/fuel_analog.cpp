#include "fuel_analog.h"

#include <Arduino.h>

#define FUEL_ADC_LEN 32
int fuelAdcValue = 0;
int fuelAdcValueRolling[FUEL_ADC_LEN] = {0};
int fuelAdcIndex = 0;

void setupFuelAnalog() {
  // Setup the GPIO pin for ADC reading
  pinMode(0, INPUT);         // Set GPIO 0 as input for ADC
  analogReadResolution(12);  // Set ADC resolution to 16bits
  analogSetAttenuation(ADC_11db);
}

void readFuelAnalog(Telemetry* telemetry) {
  // Rolling sum of adc values (later truncated to "average" them)
  int newValue = analogRead(0);  // Read ADC value from pin 0 (GPIO 0)
  fuelAdcValue += newValue - fuelAdcValueRolling[fuelAdcIndex];
  fuelAdcValueRolling[fuelAdcIndex] = newValue;
  fuelAdcIndex = (fuelAdcIndex + 1) % FUEL_ADC_LEN;  // Incr \w wrap
  if (telemetry->RPM > 100) {
    // De-noised value while flying
    telemetry->fuel = fuelAdcValue >> 4;
  } else {
    // Instant value while on the ground
    telemetry->fuel = newValue << 1;
  }
}