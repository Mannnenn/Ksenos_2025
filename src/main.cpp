#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>

#define DEVICENAME "UART test"
#define INTERVAL 1000 // millisec

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "54cef7fa-2d9c-472e-81ee-4d5a6d84ac5c" // UART service UUID
#define CHARACTERISTIC_UUID_RX "54cef7fa-2d9c-472e-81ee-4d5a6d84ac5d"
#define CHARACTERISTIC_UUID_TX "54cef7fa-2d9c-472e-81ee-4d5a6d84ac5e"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
boolean isrequested = false;
int interval = INTERVAL; // millisec
int currenttime = 0;     // millisec
int previoustime = 0;    // millisec

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    Serial.println("** device connected");
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    Serial.println("** device disconnected");
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0)
    {
      // Display on serial monitor for debug
      Serial.println("*********");
      Serial.print("Received Value: ");
      Serial.println(rxValue.c_str());
      Serial.println("*********");
      // Reply as is
      pTxCharacteristic->setValue(rxValue);
      pTxCharacteristic->notify();
      delay(10);

      if (rxValue.find("start") == 0)
      { //"start" has found
        isrequested = true;
        previoustime = millis(); // elapsed time(ms)
        previoustime -= interval;
      }
      else if (rxValue.find("stop") == 0)
      { //"stop" has found
        isrequested = false;
      }
    }
  }
};

void setup()
{
  Serial.begin(115200);
  // Create the BLE Device
  BLEDevice::init(DEVICENAME); // BLE Device Name scaned and found by clients

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX,
      BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("start advertising");
  Serial.println("Waiting a client connection to notify...");

  // The ESP32S3 bluetoth 5.0 requires security settings.
  // Without it, an error will occur when trying to pair with other devices.
  // Using a 6-digit PIN as the authentication method seems to work.
  // This PIN allows the device to be paired with an Client device.
  // Client device users will be prompted to key in a 6-digit PIN, '123456'.
  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setStaticPIN(123456);
  // Setting ESP_LE_AUTH_REQ_SC_ONLY instead of the PIN setting eliminates the need for PIN input during pairing.
  // pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_ONLY);
}

void loop()
{
  if (deviceConnected)
  {
    if (isrequested)
    {
      currenttime = millis(); // elapsed time(ms)
      if (interval <= currenttime - previoustime)
      {
        previoustime += interval;
        char string0[256];
        sprintf(string0, "Hello %.3f\r\n", currenttime / 1000.f);
        pTxCharacteristic->setValue(string0);
        pTxCharacteristic->notify();
        delay(10);
      }
    }
  }
  else
  {
    isrequested = false;
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }

  // connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
  delay(2); // allow the cpu to switch to other tasks
}