#pragma once

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "types.h"

FanControl* getFanControl();

void setupFanCtrl();
u_int16_t readFanAmps();
void updateFanEsc(Telemetry* telemetry);
void saveFanConfig();
void loadFanConfig();

class CallbackFanControl : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic* pCharacteristic) {
    pCharacteristic->setValue((uint8_t*)(getFanControl()), sizeof(FanControl));
  }
  void onWrite(BLECharacteristic* pCharacteristic,
               esp_ble_gatts_cb_param_t* param) {
    uint8_t* data = pCharacteristic->getData();
    memcpy(getFanControl(), data, sizeof(FanControl));
    saveFanConfig();
  }
};
