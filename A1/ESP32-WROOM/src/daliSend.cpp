/*
                                                                  *****FILE HEADER*****
File Name - daliSend.ino

Author/s - Michael Gamston

Description - Functions for sending a command to our dali control unit

Hardware - A0.4

Comments - 

Libraries 
    
Repo - michaelgamston/MVP
Branch - main

*/
#include "daliSend.h"
#include "ESPtime.h"
#include "logControl.h"
#include <SoftwareSerial.h>

static const byte rxPin = 33;
static const byte txPin = 34;

static SoftwareSerial softSerial (rxPin, txPin);

static const int high = 99;
static const int low = 20;
static const int off = 5; 
static int currentLightLevel = 0;

static const int instructions = 4;
int lightTime[instructions][2];
bool daliTestSwitchFlag = false; 

#ifdef ACTIVATE_BY_TIME
//change these to change the activation times 
static const int activationTimeHours = 15;
static const int activationTimeMins = 17;
static const int deactivationTimeHours = 15;
static const int deactivationTimeMins = 18;
#endif

#ifdef ACTIVATE_BY_MOTION
static bool activationFlag = false;
static bool commandSentFlag = false; 
static SemaphoreHandle_t mutex;


void checkActivationFlag(void* parameters){
    while(1){
        Serial.println("checking dali flag status");
        if (activationFlag){
            Serial.print("flag activated Dali");
            daliSend(100);
        }
        else {
            Serial.print("flag deactivated Dali");
            daliSend(20);
            }

        xSemaphoreTake(mutex, portMAX_DELAY);
        commandSentFlag = true;
        xSemaphoreGive(mutex);

        vTaskDelay(2000/ portTICK_PERIOD_MS);
    }
}

void daliChangeFlagStatus(bool status){
    if(commandSentFlag){
        activationFlag = status;

        Serial.print("daliChangeFlagStatus called, activationFlag is now: ");
        Serial.println(activationFlag);

        xSemaphoreTake(mutex, portMAX_DELAY);
        commandSentFlag = false;
        xSemaphoreGive(mutex);
    }
}
#endif

#ifdef ACTIVATE_BY_TIME
void daliTimeActivation(void* parameters){
    //add time comparision, create thread, change currently level, send to dali
    int hours;
    int mins;
    while(1){
        hours = ESPtime.getHour(true);
        mins = ESPtime.getMinute();
        if(hours == activationTimeHours && mins == activationTimeMins && currentLightLevel != high){
            daliSend(high);
            currentLightLevel = high;
        }
        else if(hours == deactivationTimeHours && mins == deactivationTimeMins && currentLightLevel != off){
            daliSend(off);
            currentLightLevel = off;
        }
        vTaskDelay(1/ portTICK_PERIOD_MS);
    }
}

#endif

void daliINIT(void){

    Serial.println("dali init");

    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    softSerial.begin(115200);

#ifdef ACTIVATE_BY_MOTION

    mutex = xSemaphoreCreateMutex();
    Serial.println("Creating Tasks");
    xTaskCreate(
        checkActivationFlag, 
        "dali activation chech", 
        1024, 
        NULL, 
        1, 
        NULL
    );

#endif
#ifdef ACTIVATE_BY_TIME

    Serial.println("Creating Tasks");
    xTaskCreate( 
        daliTimeActivation,
        "checking for light event times",
        2048,
        NULL,
        1,
        NULL
    );

#endif
    Serial.println("dali init complete");
    
}

// void daliTestSwitchInit(int lightTimeAWS[4][2]){
//     lightTime = lightTimeAWS;
// }

// void daliTestSwitch(void* paramters){
//     for(int i = 0; i < instructions; i++){
//         daliSend(lightTime[0][0]);
//         vTaskDelay(lightTime[0][1]);
//     }
// }


void daliSend(int lightLevel){
    
    Serial.println();
    if(lightLevel != currentLightLevel){
        softSerial.write(lightLevel);
        currentLightLevel = lightLevel;
        //updateLogFile(lightLevel);
        Serial.print("New light level sent: ");
        Serial.println(currentLightLevel);
    }else {
        Serial.print("New light level: ");
        Serial.print(lightLevel);
        Serial.println(" recieved but not sent");
    }

    Serial.println();
}
