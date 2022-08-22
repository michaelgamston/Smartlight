/*
                                                                  *****FILE HEADER*****
File Name - main.ino

Author/s - Michael Gamston - Joe Button

Description - Main function's for Controll of ESP32-WROOM

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for comments 

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main

*/
#include "spi_comms.h"
#include "AWS_funcs.h"
#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  connectAWS();
  init_spi();
  delay(2000); // Allow time for peripherals to power up.
}

void loop()
{
  //recieve data from both peripheral's and send to aws
  
  for (int i = 1; i <= 2; i++){
    Serial.println(i);
    spi_txn(i, 20);
    send_image(spi_buf, SPI_BUFFER_SIZE);
  }
  
  client.loop();
  //delay(2000);
}
