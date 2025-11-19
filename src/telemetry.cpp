#include "telemetry.h"

#include <driver/pcnt.h>

#include "gpio.h"
#include "max6675.h"

MAX6675 CHT(GPIO_SPI_CLK, GPIO_CHT, GPIO_SPI_SDA);
MAX6675 EGT(GPIO_SPI_CLK, GPIO_EGT, GPIO_SPI_SDA);

Telemetry telemetry;
uint16_t lastTelemetry = 0;
pcnt_config_t config;
uint16_t rpmCounts = 0;

Telemetry* getTelemetry() { return &telemetry; }
uint16_t getLastTelemetry() { return lastTelemetry; }

// char serial1Buff[20];
// void ParseSerialRX() {
//   if (Serial1.available() > 0) {
//     size_t count = Serial1.readBytes(serial1Buff, 16);
//     if (serial1Buff[0] == 136 && serial1Buff[1] == 14) {
//       // Telemetry packet
//       telemetry.CHT = serial1Buff[8] + (serial1Buff[9] << 8);
//       telemetry.EGT = serial1Buff[10] + (serial1Buff[11] << 8);
//       telemetry.RPM = serial1Buff[12] + (serial1Buff[13] << 8);
//     }
//     lastTelemetry = millis() / 1000;
//   }
//   Serial1.flush();
// }

void IRAM_ATTR Ext_INT1_ISR() { rpmCounts++; }

void setupTelemetry() {
  // Setup UART
  // Serial1.setPins(20, 21);
  // Serial1.onReceive(ParseSerialRX);
  // Serial1.begin(9600);
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

  lastTelemetry = now;
}
