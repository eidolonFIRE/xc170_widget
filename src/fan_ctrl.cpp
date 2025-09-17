#include "fan_ctrl.h"

#include <Arduino.h>
#include <ESP32Servo.h>

#include "INA226.h"

Servo fanESC;

FanControl fanControl;

// I2C device to read current
INA226 INA0(0x40);

FanControl* getFanControl() { return &fanControl; };

void setupFanCtrl() {
  // Fan Ctrl (Servo)
  fanESC.setPeriodHertz(50);
  fanESC.attach(3, 1000, 2000);

  // Setup I2C for current sensor
  Wire.setPins(8, 9);
  Wire.begin();
  if (!INA0.begin()) {
    Serial.println("INA0 could not connect. Fix and Reboot");
  }
  INA0.setMaxCurrentShunt();

  loadFanConfig();
}

u_int16_t readFanAmps() { return abs(INA0.getCurrent_mA()); }

void updateFanEsc(Telemetry* telemetry) {
  telemetry->fanCtrl = constrain(
      map(telemetry->CHT, fanControl.chtMin, fanControl.chtMax, 0, 100), 0,
      100);
  fanESC.write(max(fanControl.override, telemetry->fanCtrl));
}

void saveFanConfig() {
  ESP.flashWrite(0, (uint32_t*)(&fanControl), sizeof(FanControl));
}

void loadFanConfig() {
  ESP.flashRead(0, (uint32_t*)(&fanControl), sizeof(FanControl));
  fanControl.override = 0;
}
