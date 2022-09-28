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
#include "SMT_Cubik_API_Data_Types.h"

// timing variables
unsigned long start;
unsigned long end;
const uint32_t IMAGE_SIZE = H * W;


void setup()
{
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  setup_spi();
  setup_camera(FRAME_SIZE);
  // Neural network object
  Serial.println("setup");

  u8CubikControl_DaliLight_Init(CA_REQUEST_IFACE_ENABLE, 0U, 0U);
  u8CubikControl_DaliLight_SetLevel(0);
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
    u8CubikControl_DaliLight_SetLevel(100);
    digitalWrite(4, HIGH);
    
  }
  send_image();
  Serial.println(digitalRead(15));
  end = millis();
  Serial.printf("Took %d ms\n", end-start);
  delay(2000);
  digitalWrite(4, HIGH);
}