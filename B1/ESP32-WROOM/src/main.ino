/*
                                                                  *****FILE HEADER*****
File Name - main.ino

Author/s - Michael Gamston - Joe Button

Description - Main function's for Controll of ESP32-WROOM

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - Cubik intergration throwing up errors, work on getting it work. or implement own version. 

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main

*/
#include <Arduino.h>

#include "spi_comms.h"
#include "AWS_funcs.h"
#include "MySPIFFS.h"
#include "mesh.h"
#include "daliSend.h"
#include "logControl.h"


#define PIN_TX              27
#define PIN_RX              26




void setup()
{
  if(!SPIFFS.begin()) {
    SPIFFS.format();
    ESP.restart();
  }
  
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);
 
  connectAWS();
  daliINIT();
  init_spi();
  logFileInit();
  //mesh_init();

  xTaskCreatePinnedToCore(
    checkMQTT,
    "checking mqtt",
    4096,
    NULL,
    1,
    NULL,
    1
  );
  
  // Allow time for peripherals to power up.
  vTaskDelay(2000 / portTICK_PERIOD_MS);
}

void loop()
{
  spiLoopPeripheral();
}
