/*
                                                                  *****FILE HEADER*****
File Name - main.ino

Author/s - Joe Button

Description - Main function's for Controll of ESP32-CAM

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for comments 

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main

*/
#include <Arduino.h>
#include "camera_funcs.h"
#include "serial_comms.h"

// timing variables
unsigned long start;
unsigned long end;
const uint32_t IMAGE_SIZE = H * W;


void setup()
{
  Serial.begin(115200);
  setup_spi();
  setup_camera(FRAME_SIZE);
  // Neural network object
  Serial.println("setup");
}

void loop()
{
  // print_quant_conversions();
  start = millis();
  // copy previous frame for background subtraction
  update_frame();
  // take pic
  capture_still();
  if (motion_detect())
  {
    Serial.println("Motion detected");
  }
  send_image();
  end = millis();
  Serial.printf("Took %d ms\n", end-start);
  delay(2000);
}