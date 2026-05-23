#include "telemetry.h"

#include <Arduino.h>
#include <Wire.h>

#include "gpio.h"
#include "max6675.h"

// I2C configuration for RPM sensor
#define RPM_I2C_ADDR 0x30

MAX6675 CHT(GPIO_SPI_CLK, GPIO_CHT, GPIO_SPI_SDA);
MAX6675 EGT(GPIO_SPI_CLK, GPIO_EGT, GPIO_SPI_SDA);

Telemetry telemetry;
uint32_t lastTelemetry = 0;

Telemetry* getTelemetry() { return &telemetry; }
uint32_t getLastTelemetry() { return lastTelemetry; }

void setupTelemetry() {
  pinMode(GPIO_CDI, INPUT_PULLUP);
  lastTelemetry = millis();
}

void updateTelemetry() {
  auto now = millis();

  // Read telemetry from i2c device (16bit big-endian)
  // Wire.beginTransmission(RPM_I2C_ADDR);
  if (Wire.requestFrom(RPM_I2C_ADDR, 2) == 2) {
    uint8_t highByte = Wire.read();
    uint8_t lowByte = Wire.read();
    telemetry.RPM = (highByte << 8) | lowByte;
  }

  auto tempCHT = CHT.readCelsius();
  if (isfinite(tempCHT) && !isnan(tempCHT)) {
    telemetry.CHT = (uint16_t)round(tempCHT * 10);
    lastTelemetry = now;
  }
  auto tempEGT = EGT.readCelsius();
  if (isfinite(tempEGT) && !isnan(tempEGT)) {
    telemetry.EGT = (uint16_t)round(tempEGT * 10);
  }
}
