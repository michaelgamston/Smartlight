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
#include "SMT_Cubik_API_Data_Types.h"

int blocker = 0;

void setup()
{
  Serial.begin(115200);
  connectAWS();
  init_spi();
  delay(2000); // Allow time for peripherals to power up.
  u8CubikControl_LibInit();
  u8CubikControl_DaliLight_Init(CA_REQUEST_IFACE_ENABLE, 0U, 0U);
  u8CubikControl_DaliLight_SetLevel(0);
}

void loop()
{
  //recieve data from both peripheral's and send to aws
  
  for (int i = 1; i <= 2; i++){
    spi_txn(i, 20);
    set_buf();
  }

  if (trigger){
    u8CubikControl_DaliLight_SetLevel(100);
    send_image(spi_buf, SPI_BUFFER_SIZE);
    blocker = 0; 
  }
  else if (blocker > 3) {
    u8CubikControl_DaliLight_SetLevel(0);
    blocker = 4;
  }
  else blocker++;  
  
  
  client.loop();
  delay(2000);
}
