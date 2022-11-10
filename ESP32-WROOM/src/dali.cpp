/*
                                                                  *****FILE HEADER*****
File Name - dali.cpp

Author/s - Michael Gamston 

Description - Contains functions for controlling DALI light system

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - This library was built by cross examining the Cubik library and some other's online. It's bare bone version that contains only
            functionality that we need. 

         - if basic functionailty work implement set level, fade level.

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main
test
*/
#include "dali.h"

//flags

uint8_t currLightLevel = 0U; 

void sendCommand(uint8_t cmd1, uint8_t cmd2)
{
#ifdef DEBUG
    /* Debug output only */
    /* GrJo - 2020_12_10 - seems unreliable on the test bench - send command twice */
    Serial.print("Sending commands ");
    Serial.print(cmd1);
    Serial.print(" and ");
    Serial.println(cmd2);
    #ifdef ESP32_CAM
        flash_LED(2, 100);
    #endif
#endif
    sendBit(1);
    sendByte(cmd1);
    sendByte(cmd2);

#ifdef DEBUG
    Serial.println("");
#endif  

#ifdef POLARITY_INVERSION
    digitalWrite(DALITX, LOW);
#else 
    digitalWrite(DALITX, HIGH);
#endif

    delay(5);

#ifdef DEBUG
    Serial.println("Commands sent");
    #ifdef ESP32_CAM
        flash_LED(2, 100);
    #endif
#endif
}

void sendByte(uint8_t b)
{
    for (int i = 7; i >= 0; i--) 
    {
        sendBit((b >> i) & 1);
    }
}

void sendBit(int b)
{
    if (b) {
        sendOne();
    }
    else {
        sendZero();
    } 
}

void sendOne(void)
{

#ifdef DEBUG
    Serial.print(1);
#endif

#ifdef POLARITY_INVERSION
    digitalWrite(DALITX, HIGH);
    delayMicroseconds(interval);
    digitalWrite(DALITX, LOW);
    delayMicroseconds(interval);
#else
    digitalWrite(DALITX, LOW);
    delayMicroseconds(interval);
    digitalWrite(DALITX, HIGH);
    delayMicroseconds(interval);
#endif
}


void sendZero(void)
{

#ifdef DEBUG
    Serial.print(0);
#endif

#ifdef POLARITY_INVERSION
    digitalWrite(DALITX, LOW);
    delayMicroseconds(interval);
    digitalWrite(DALITX, HIGH);
    delayMicroseconds(interval);
#else
    digitalWrite(DALITX, HIGH);
    delayMicroseconds(interval);
    digitalWrite(DALITX, LOW);
    delayMicroseconds(interval);
#endif
}

void DALI_init()
{
    const int delaytime = 10; //ms

    pinMode(DALIRX, INPUT);
    pinMode(DALITX, OUTPUT);

#ifdef ESP32_CAM
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
#endif

#ifdef POLARITY_INVERSION
    digitalWrite(DALITX, HIGH);
#else
    digitalWrite(DALITX, LOW);
#endif

#ifdef DEBUG
    Serial.println("Initializing dali");
#endif

    delay(delaytime);
    sendCommand(BROADCAST_C, RESET);
    delay(delaytime);
    sendCommand(BROADCAST_C, RESET);
    delay(delaytime);
    sendCommand(BROADCAST_C, OFF_C);
    delay(delaytime);
    sendCommand(0b10100101, 0b00000000); //initialise
    delay(delaytime);
    sendCommand(0b10100101, 0b00000000); //initialise

    sendCommand(0b10100001, 0b00000000);  //terminate

#ifdef DEBUG
    Serial.println("Initialized");
    #ifdef ESP32_CAM
        delay(1000);
        flash_LED(5, 100);
    #endif
#endif
}

#ifdef ESP32_CAM
void flash_LED(uint8_t fnumber, uint8_t fdelay){
    for (int i = 0; i < fnumber; i++){
        digitalWrite(LED, HIGH);
        delay(fdelay);
        digitalWrite(LED, LOW);
        if ((i+2) != fnumber) delay(fdelay);
    }
}
#endif

void DALI_setLightDirect(uint8_t level){
    uint8_t outLevel = 0U;

    if (level > 100 && level < 0) return;

    outLevel = (uint8_t)(((uint16_t)level * 255U) / 100U);
    if (outLevel != currLightLevel){
        if (outLevel < 25U){
            sendCommand(BROADCAST_C, OFF_C);
            currLightLevel = 0U;
        }
        else if (outLevel > 250U){
            sendCommand(BROADCAST_C, ON_C);
            currLightLevel = 255U;
        }
        else{
            sendCommand(BROADCAST_LL, outLevel);
            currLightLevel = outLevel;
        } 
            
    }

}


void DALI_setLightFade(uint8_t level){
    uint8_t outLevel = 0U;

    if (level > 100 && level < 0) return;

    outLevel = (uint8_t)(((uint16_t)level * 255U) / 100U);

    if (outLevel < currLightLevel){
        for (currLightLevel; currLightLevel < outLevel; currLightLevel =+ 5){
            sendCommand(BROADCAST_LL, currLightLevel);
            if(currLightLevel > 250) {
                DALI_setLightDirect(100);
                break;
            }
        }
    }

    if (outLevel > currLightLevel){
        for (currLightLevel; currLightLevel > outLevel; currLightLevel =- 5){
            sendCommand(BROADCAST_LL, currLightLevel);
            if(currLightLevel < 6) {
                DALI_setLightDirect(0);
                break;
            };
        }
    }
}




