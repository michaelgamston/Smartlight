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
#include "spi_comms.h"
#include "AWS_funcs.h"
#include <Arduino.h>
#include "MySPIFFS.h"
#include "mesh.h"
#include "daliSend.h"

#define PIN_TX              27
#define PIN_RX              26



void SPItransfer(void* parameters){
  while(1){
    for (int i = 1; i <= 2; i++){
    spi_txn(i, 8192);
    send_image(spi_buf, SPI_BUFFER_SIZE);
    set_buf();
    //Serial.println("looped");
    }
     vTaskDelay(2000/ portTICK_PERIOD_MS);
  }
}

void setup()
{
  SPIFFS.begin();
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);

  daliINIT();
  connectAWS();
  init_spi();
  mesh_init();
  // Allow time for peripherals to power up.
  vTaskDelay(2000 / portTICK_PERIOD_MS);


  xTaskCreatePinnedToCore(
    SPItransfer,
    "SPI",
    10240,
    NULL,
    1,
    NULL,
    0
  );
  
  //Delete Setup and loop tasks once created 
}

void loop()
{

    checkMQTT();
//   mesh_update();
//   for (int i = 1; i <= 2; i++){
//   spi_txn(i, 8192);
//   send_image(spi_buf, SPI_BUFFER_SIZE);
//   set_buf();
//   //Serial.println("looped");
//   }
    vTaskDelay(2000/ portTICK_PERIOD_MS);
}
