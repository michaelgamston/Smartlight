/*
                                                                  *****FILE HEADER*****
File Name - daliSend.cpp

Author/s - Michael Gamston

Description - Functions for sending a command to our dali control unit

Hardware - A0.4

Comments - add where the commad come from when it's changes (motion, time, aws, sequence)
         - motionActivation is still buggy and requres work  

Libraries 
    
Repo - michaelgamston/MVP
Branch - main

*/
#include "daliSend.h"
#include "ESPtime.h"
#include "logControl.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Software serial 
static const byte rxPin = 33;
static const byte txPin = 32;
static SoftwareSerial softSerial (rxPin, txPin);

// Light level 
//currently anything 99 is full brightness, 10 and below turns the lamp off 

static int currentLightLevel = 0;



void daliINIT(void){

    Serial.println("dali init");

    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    softSerial.begin(115200);

    Serial.println("dali init complete");
    
}


void daliSend(int lightLevel){
    
    Serial.println();
    if(lightLevel != currentLightLevel){
        softSerial.write(lightLevel);
        currentLightLevel = lightLevel;
        updateLogFile(lightLevel);
        Serial.print("New light level sent: ");
        Serial.println(currentLightLevel);
    }else {
        Serial.print("New light level: ");
        Serial.print(lightLevel);
        Serial.println(" recieved but not sent");
    }
    Serial.println();
}
