#include "daliSend.h"
#include <SoftwareSerial.h>
#include <time.h>

static const byte rxPin = 33;
static const byte txPin = 32;

static SoftwareSerial softSerial (rxPin, txPin);

static int currentLightLevel = 0;

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
void checkTime(void* parameters){
    //add time comparision, create thread, change currently level, send to dali
    currentLightLevel = 0U;
}

#endif

void daliINIT(void){
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    softSerial.begin(115200);

#ifdef ACTIVATE_Y_MOTION
    mutex = xSemaphoreCreateMutex();

    xTaskCreate(
        checkActivationFlag, 
        "dali activation chech", 
        1024, 
        NULL, 
        1, 
        NULL
        );
#endif
    Serial.println("dali init complete");
    
}


void daliSend(int lightLevel){
    
    Serial.println();
    if(lightLevel != currentLightLevel){
        softSerial.write(lightLevel);
        currentLightLevel = lightLevel;
        Serial.print("New light level sent: ");
        Serial.println(currentLightLevel);
    }else {
        Serial.print("New light level: ");
        Serial.print(lightLevel);
        Serial.println(" recieved but not sent");
    }
    Serial.println();
}
