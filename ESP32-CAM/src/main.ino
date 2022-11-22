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

static const int longFlash = 500;
static const int shortFlash = 100;
static const int LED = 4;

void flashLED(int flashLength){
  digitalWrite(LED, HIGH);
  delay(flashLength);
  digitalWrite(LED, LOW);
}

void send(void* parameters){
  if(send_image()) flashLED(longFlash);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
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
    flashLED(shortFlash);
    addActivationByte();
  }

  if(send_image()) flashLED(longFlash);
  
  
  delay(2000);

}