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
#include <ArduinoJson.h>

static const byte rxPin = 32;
static const byte txPin = 33;

static SoftwareSerial softSerial (rxPin, txPin);

static const int high = 99;
static const int low = 20;
static const int off = 5; 
static int currentLightLevel = 0;

bool daliSequenceFlag = false; 
static TaskHandle_t sequenceTask; 
static QueueHandle_t sequenceQueue;
static SemaphoreHandle_t daliMutex;

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

void daliSequenceInit(StaticJsonDocument<200> sequence){
    
    if (daliSequenceFlag) { 
        Serial.println("deleting previous dali sequence rtos elements");
        vQueueDelete(sequenceQueue);
        vTaskDelete(sequenceTask);
        vSemaphoreDelete(daliMutex);
        daliSequenceFlag = false;        
    }
    
    daliMutex = xSemaphoreCreateMutex();
    BaseType_t queueCheckOne;
    BaseType_t queueCheckTwo;
    BaseType_t taskCheck;
    int size = sequence["Size"];
    Serial.println("variables created");

    xSemaphoreTake(daliMutex, portMAX_DELAY);
    sequenceQueue = xQueueCreate(size*2, sizeof(int));
    taskCheck = xTaskCreatePinnedToCore(daliSequence, "dali test sequencing", 2048, (void *) size, 1, &sequenceTask, 0);
    Serial.println("rtos objects created");

    //SEQUENCE IS GIVING OUT STRANGE RESULTS.. LOOK INTO THIS
    if(taskCheck == pdTRUE && uxQueueSpacesAvailable(sequenceQueue) == size*2){
        // change this so it's only set if every thing works 
        for (int i = 0; i < size; i ++){
            int lightInstruction = sequence[i][0];
            int delayInstruction = sequence[i][1];
            Serial.print(lightInstruction);
            Serial.print(" ");
            Serial.println(delayInstruction);
            queueCheckOne = xQueueSend(sequenceQueue, &lightInstruction, portMAX_DELAY);
            queueCheckTwo = xQueueSend(sequenceQueue, &delayInstruction, portMAX_DELAY);
            // need a better check here, if anything failes everything needs to deleted
            if(queueCheckOne == pdFALSE || queueCheckTwo == pdFALSE){
                Serial.println("queue send failed");
                vTaskDelete(sequenceTask);
                break;
            }
        }
        //check should account for this too
        xSemaphoreGive(daliMutex);
        daliSequenceFlag = true;
        Serial.println("dali init complete");
        Serial.print((uxQueueMessagesWaiting(sequenceQueue) / 2));
        Serial.println(" instructions queued");
        
    }else{
        if(taskCheck == pdTRUE) vTaskDelete(sequenceTask);
        Serial.println("dali sequence init failed");
    }

    

}

void daliSequence(void* parameters){
    
    int size = (int) parameters;
    Serial.println(size);
    int instructions[size][2];
    //Task must wait for daliSequenceInit to finish
    xSemaphoreTake(daliMutex, portMAX_DELAY);
    Serial.println("mutex taken");
    if(daliSequenceFlag){
        Serial.println("Task entered setup");
        int stop = uxQueueMessagesWaiting(sequenceQueue);
        Serial.print(stop);
        Serial.println(" instructions waiting");
        for(int i = 0; i < size; i++){
            if (xQueueReceive(sequenceQueue, &instructions[i][0], 10) == pdFALSE) {
                Serial.print("queue receive fail on "); 
                Serial.println(i);
                while(1);
            }
            if (xQueueReceive(sequenceQueue, &instructions[i][1], 10) == pdFALSE) {
                Serial.print("queue receive fail on "); 
                Serial.println(i); 
                while(1);
            }
        }

        if (uxQueueMessagesWaiting(sequenceQueue) == 0){
            vQueueDelete(sequenceQueue);
            Serial.println("Queue has been read and deleted");
            
        }else {
            daliSequenceFlag = false;
            Serial.println("Queue read for dali sequence failed");
        }    

        if(daliSequenceFlag) Serial.println("setup finished, dali seqeuence ready");
        for (int i = 0; i < size; i++){
            Serial.print(instructions[i][0]);
            Serial.println(instructions[i][1]);

        }
        while(daliSequenceFlag){ 
            while(1);
            for(int i = 0; i < size; i++){
                daliSend(instructions[i][0]);
                vTaskDelay(instructions[i][1]);
                Serial.println("instruction sent");
            }
        }
    }
}


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
