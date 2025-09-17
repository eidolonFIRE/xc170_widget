#include "telemetry.h"

Telemetry telemetry;

Telemetry* getTelemetry() { return &telemetry; }

char serial1Buff[20];
void ParseSerialRX() {
  if (Serial1.available() > 0) {
    size_t count = Serial1.readBytes(serial1Buff, 16);
    if (serial1Buff[0] == 136 && serial1Buff[1] == 14) {
      // Telemetry packet
      telemetry.CHT = serial1Buff[8] + (serial1Buff[9] << 8);
      telemetry.EGT = serial1Buff[10] + (serial1Buff[11] << 8);
      telemetry.RPM = serial1Buff[12] + (serial1Buff[13] << 8);
    }
    Serial1.flush();
  }
}

void setupTelemetry() {
  // Setup UART
  Serial1.setPins(20, 21);
  Serial1.onReceive(ParseSerialRX);
  Serial1.begin(9600);
}
