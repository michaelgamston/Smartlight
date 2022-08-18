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
#include "serial_comms.h"
#include "AWS_funcs.h"
#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  connectAWS();
  setup_spi();
}

void loop()
{
  receive_image(); // Receive image via SPI from esp32-cam
  send_image(spi_master_rx_buf, BUFFER_SIZE); // send image via WiFi to AWS S3 bucket along with some parameters
  client.loop();
  delay(2000);
}
