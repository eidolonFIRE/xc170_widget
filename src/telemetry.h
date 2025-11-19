#pragma once

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "types.h"

Telemetry* getTelemetry();

void setupTelemetry();

// Last update in millis
uint16_t getLastTelemetry();

void updateTelemetry();

class CallbackTelemetry : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic* pCharacteristic) {
    pCharacteristic->setValue((uint8_t*)(getTelemetry()), sizeof(Telemetry));
  }
};
