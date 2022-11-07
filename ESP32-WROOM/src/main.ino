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
#include "dali.h"
#include "MySPIFFS.h"

#define PIN_TX              27
#define PIN_RX              26

void setup()
{
  SPIFFS.begin();
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);

  connectAWS();
  init_spi();
  delay(2000); // Allow time for peripherals to power up.

while(1)
{
  LTE_publish("Hello from SIM7600");
  delay(10000);
}
}

void loop()
{
  //recieve data from both peripheral's and send to aws
  for (int i = 1; i <= 2; i++){
    spi_txn(i, 8192);
    send_image(spi_buf, SPI_BUFFER_SIZE);
    set_buf();
  }

//  client.loop();

  delay(2000);
}
