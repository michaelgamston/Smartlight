#include "daliSend.h"
#include <SoftwareSerial.h>

static const byte rxPin = 33;
static const byte txPin = 32;

static SoftwareSerial softSerial (rxPin, txPin);

static int currentLightLevel = 0;


void daliINIT(void){
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    softSerial.begin(115200);
    Serial.println("dali init complete");
}


void daliSend(int lightLevel){
    if(lightLevel != currentLightLevel){
        softSerial.write(lightLevel);
        currentLightLevel = lightLevel;
        Serial.print("New light level sent: ");
        Serial.println(currentLightLevel);
    }
}
