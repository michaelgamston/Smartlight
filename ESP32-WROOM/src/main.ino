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
//#include <FreeRTOS.h>
#include <SoftwareSerial.h>

#define PIN_TX              27
#define PIN_RX              26

const byte rxPin = 33;
const byte txPin = 32;


static const int DALIActivationPin = GPIO_NUM_5;
static int activationBit = 0b0;

SoftwareSerial softSerial (rxPin, txPin);

void sendDALIactivation(void* parameters){
  while(1){

    softSerial.write(activationBit);
    Serial.println(activationBit);

    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}


void setup()
{
  SPIFFS.begin();
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);

  pinMode(DALIActivationPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  softSerial.begin(115200);

  //connectAWS();

  //init_spi();z
  // Allow time for peripherals to power up.
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  xTaskCreatePinnedToCore(
		sendDALIactivation,
		"activate DALI",
		1024,
		NULL,
		1,
		NULL,
		1
  );

  //Delete Setup and loop tasks once created 
}

void loop()
{
  for (int i = 1; i <= 2; i++){
    spi_txn(i, 8192);
    send_image(spi_buf, SPI_BUFFER_SIZE);
    set_buf();
    //Serial.println("looped");
  }
  checkMQTT();
  vTaskDelay(2000 / portTICK_PERIOD_MS);
}
