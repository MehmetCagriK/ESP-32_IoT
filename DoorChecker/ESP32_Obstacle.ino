/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define DOOR_CHECK_SER_UUID     "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define DOOR_STATUS_CHAR_UUID   "4fafc205-1fb5-459e-8fcc-c5c9c331914b"
#define NOT_PERIOD_CHAR_UUID    "4fafc209-1fb5-459e-8fcc-c5c9c331914b"

#define DEV_INF_SER_UUID        "0000180A-0000-1000-8000-00805F9B34FB"
#define MAN_NAME_CHAR_UUID      "00002A29-0000-1000-8000-00805F9B34FB"
#define MODEL_NO_CHAR_UUID      "00002A24-0000-1000-8000-00805F9B34FB"

// Define reference for Door Status characteristic here, since we will need it in the loop function
BLECharacteristic *pDoorStatusChar;
bool doorOpen = false;
bool oldDoorOpen = false;

int isObstaclePin = 18; // This is our input pin
int isObstacle = HIGH; // HIGH MEANS NO OBSTACLE

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  // Set pin connected to obstacle sensor's OUT as input(we will read it)
  pinMode(isObstaclePin, INPUT);
  
  // Create device and server instances
  BLEDevice::init("Door Checker");
  BLEServer *pServer = BLEDevice::createServer();
  
  // Create Door Checker Service instance
  BLEService *pDoorCheckService = pServer->createService(DOOR_CHECK_SER_UUID);
    
  // Through Door Status characteristic, connected devices will be informed about the status of the door.
  // This characteristic can be read by connected devices manually or be notified without request.
  pDoorStatusChar = pDoorCheckService->createCharacteristic(
                      DOOR_STATUS_CHAR_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_READ
                    );
  pDoorStatusChar->setValue("UNKNOWN!");
  
  //**************************************
  // Create Device Information Service instance. Device information service is defined by
  // SIG. For more information, check; 
  //https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.device_information.xml
  BLEService *pDevInfoService = pServer->createService(DEV_INF_SER_UUID);
   
  // Manufacturer Name characteristic is one of the optional characteristics associated with 
  // Device Information Service. This characteristic is understandably read only.
  BLECharacteristic *pManNameChar = pDevInfoService->createCharacteristic(
                      MAN_NAME_CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ
                    );
  pManNameChar->setValue("Kose Industries");
                    
  // Model Number characteristic is one of the optional characteristics associated with 
  // Device Information Service. This characteristic is understandably read only.
  BLECharacteristic *pModelNoChar = pDevInfoService->createCharacteristic(
                      MODEL_NO_CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ
                    );
  pModelNoChar->setValue("1.0");                   

  // After creating services, we must start them.
  pDoorCheckService->start();
  pDevInfoService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  
  Serial.println("Characteristic defined! Now you can read it in your phone!");


  
}

void loop() {
  checkDoorOpen();
  
  // If door status is changed, meaning it is opened or closed, notify the connected users
  if (doorOpen != oldDoorOpen)
  {
    if (doorOpen)
    {
      pDoorStatusChar->setValue("OPENED!");
    }
    else
    {
      pDoorStatusChar->setValue("CLOSED!");
    }
    pDoorStatusChar->notify();
    Serial.println(pDoorStatusChar->getValue().c_str());
  }
  oldDoorOpen = doorOpen;

  // Check the door every 1 seconds
  delay(1000);
}

// This function checks the door status, open or closed.
void checkDoorOpen()
{
  
  isObstacle = digitalRead(isObstaclePin);
    if (isObstacle == LOW) {
        Serial.println("OBSTACLE!!, OBSTACLE!!");
        doorOpen = false;
    } else {
        Serial.println("clear");
        doorOpen = true;
    }
}
