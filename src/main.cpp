#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "fan_ctrl.h"
#include "fuel_analog.h"
#include "telemetry.h"
#include "types.h"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID "e1e7af55-0c37-4f29-80bf-f447757738b0"
#define UUID_CHAR_TELEMETRY "e1e7af55-9460-40f3-8b57-af0e56b471c3"
#define UUID_CHAR_FAN_CTRL "e1e7af55-6eb0-40a7-bd9e-496d6b874940"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    BLEDevice::startAdvertising();
    Serial.println("Client connected");
  };

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Client disconnected");
    deviceConnected = false;
  }
};

void setup() {
  // Serial.begin(460800);

  setupTelemetry();
  setupFanCtrl();
  setupFuelAnalog();

  // Create the BLE Device
  BLEDevice::init("xc170");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristics
  pCharacteristic = pService->createCharacteristic(
      UUID_CHAR_TELEMETRY, BLECharacteristic::PROPERTY_READ);
  pCharacteristic->setCallbacks(new CallbackTelemetry());

  pCharacteristic = pService->createCharacteristic(
      UUID_CHAR_FAN_CTRL,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new CallbackFanControl());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(
      0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();

  delay(500);
}

void loop() {
  // Regular operation stuff
  updateTelemetry();
  readFuelAnalog(getTelemetry());
  getTelemetry()->fanAmps = readFanAmps();

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }

  // NOTE: startup delay to make sure ESC has a chance to init
  if ((millis() / 1000) > 10) {
    // safety override if no serial and override not set
    if ((getLastTelemetry() < (int32_t(millis()) - 60000)) &&
        getFanControl()->override == 0) {
      // Kick up the override. It can always be dialed up / down and hold state,
      // but at least it won't be zero.
      getFanControl()->override = 20;
    }

    updateFanEsc(getTelemetry());
  }

  delay(500);
}
