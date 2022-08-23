/*
                                                                  *****FILE HEADER*****
File Name - dali.cpp

Author/s - Michael Gamston 

Description - Contains definitions for controlling DALI light system

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - This library was built by cross examining the Cubik library and some other's online. It's bare bone version that contains only
            functionality that we need. 

            IF USING DEBUG MAKE SURE TO INITIALIZE SERIAL MON

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main

*/
#pragma once
#include <Arduino.h>

/* DALI coomands */
#define BROADCAST_LL 0b11111110	/* Broadcast address, sends 'set light level' to all devices */
#define BROADCAST_C  0b11111111 /* Broadcast address, sends 'command following' to all devices */
#define ON_C 0b00000101         /* Generic command - turn on to max level */
#define OFF_C 0b00000000        /* Generic command - turn off to 0% level */
#define RESET 0b00100000        /* Generic command - tell device(s) to perform reset */
#define HALF_BIT_INTERVAL 1666  /* Used in timing calcualtion */

#define DALITX 4
#define DALIRX 15


#define REST_LL 51U // 20% of 255U
#define ACTIVE_LL 225U

// modded this from cubik library, research more, unsure of why it's this  
static uint16_t interval = (HALF_BIT_INTERVAL >> 2) - 2;

//optional defines for debugs 

//#define POLARITY_INVERSION 
//#define DEBUG
//#define ESP32_CAM

#ifdef ESP32_CAM
    #define LED 4 // make sure to change DALITX to a differnt pin
#endif

void sendCommand(uint8_t cmd1, uint8_t cmd2);

void sendByte(uint8_t b);

void sendBit(int b);

void sendOne(void);

void sendZero(void);

void DALI_init();

void DALI_setLightDirect(uint8_t level);

void DALI_setLightFade(uint8_t level);

#ifdef ESP32_CAM
    void flash_LED(uint8_t fnumber, uint8_t fdelay);
#endif