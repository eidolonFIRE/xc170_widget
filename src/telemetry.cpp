#include "telemetry.h"

#include <driver/pcnt.h>

#include "gpio.h"
#include "max6675.h"

MAX6675 CHT(GPIO_SPI_CLK, GPIO_CHT, GPIO_SPI_SDA);
MAX6675 EGT(GPIO_SPI_CLK, GPIO_EGT, GPIO_SPI_SDA);

Telemetry telemetry;
uint32_t lastTelemetry = 0;
pcnt_config_t config;
volatile uint32_t rpmCounts = 0;

Telemetry* getTelemetry() { return &telemetry; }
uint32_t getLastTelemetry() { return lastTelemetry; }

void IRAM_ATTR Ext_INT1_ISR() { rpmCounts++; }

void setupTelemetry() {
  pinMode(GPIO_CDI, INPUT_PULLUP);
  attachInterrupt(GPIO_CDI, Ext_INT1_ISR, RISING);
  lastTelemetry = millis();
}

void updateTelemetry() {
  auto now = millis();

  telemetry.RPM = rpmCounts / ((now - lastTelemetry) / 1000.0f) * 60.0f;
  rpmCounts = 0;

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
