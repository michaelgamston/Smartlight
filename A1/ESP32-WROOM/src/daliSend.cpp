/*
                                                                  *****FILE HEADER*****
File Name - daliSend.cpp

Author/s - Michael Gamston

Description - Functions for sending a command to our dali control unit

Hardware - A0.4

Comments - add where the commad come from when it's changes (motion, time, aws, sequence)
         - motionActivation is still buggy and requres work  
         - finish switch mode

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
static const int high = 99;
static const int low = 20;
static const int standby = 5; 
static daliMode currentMode = Standby;
static int currentLightLevel = 0;

// Dali Sequence activation
bool daliSequenceFlag = false; 
bool sequenceQueueFlag = false;
bool sequenceTaskFlag = false;
static TaskHandle_t sequenceTask; 
static QueueHandle_t sequenceQueue;

// Time activation 
static TaskHandle_t timeTask;
bool timeTaskFlag = false;
static int activationTimeHours = 15;
static int activationTimeMins = 17;
static int deactivationTimeHours = 15;
static int deactivationTimeMins = 18;

//Motion activation
static TaskHandle_t motionTask;
bool motionTaskFlag = false;
static bool activationFlag = false;
static bool commandSentFlag = false; 
static SemaphoreHandle_t mutex;


void daliINIT(void){

    Serial.println("dali init");

    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    softSerial.begin(115200);

    Serial.println("dali init complete");
    
}

void daliMotionInit(void){

    mutex = xSemaphoreCreateMutex();
    Serial.println("Creating Tasks");
    if (xTaskCreatePinnedToCore(
        daliCheckActivationFlag, 
        "dali activation chech", 
        1024, 
        NULL, 
        1, 
        &motionTask,
        0
    ) == pdTRUE) motionTaskFlag = true;
}

void daliCheckActivationFlag(void* parameters){
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


void daliTimeInit(void ){

    if(timeSetFlag){
        Serial.println("Creating Tasks");
        if (xTaskCreatePinnedToCore( 
            daliTimeActivation,
            "checking for light event times",
            2048,
            NULL,
            1,
            &timeTask,
            0
        ) == pdTRUE) timeTaskFlag = true;
    }else{
        Serial.println("ESPtime has not been set. Unable to start time task");
    }

}

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
        else if(hours == deactivationTimeHours && mins == deactivationTimeMins && currentLightLevel != standby){
            daliSend(standby);
            currentLightLevel = standby;
        }
        vTaskDelay(1/ portTICK_PERIOD_MS);
    }
}

// not tested 
void daliSelectMode(daliMode newMode){

    // clear tasks required by mode change
    if (currentMode == SequenceActivation) daliClearSequence();
    else if (currentMode == TimeActivation && timeTaskFlag) vTaskDelete(timeTask); // not tested
    else if (currentMode == MotionActivation && motionTaskFlag) vTaskDelete(motionTask); // not tested

    // find new mode and do any set up 
    switch (newMode) {
        case High:
            daliSend(high);
            currentMode = High;
            break;
        case Low:
            daliSend(low);
            currentMode = Low;
            break;
        
        case Standby:
            daliSend(standby);
            currentMode = Standby;
            break;
        
        case CustomLevelActivation:
            currentMode = CustomLevelActivation;
            break;
        
        case TimeActivation:
            //need to add deactication mode to timeActivation, also look into 
            // delete task that controlls time activation -- keep variables so it can be reactivated
            daliTimeInit();
            currentMode = TimeActivation;
            break;

        case MotionActivation:
            //THIS DOES NOT WORK. NEEDS FINISHING BEFORE THIS MODE CAN BE USED
            daliMotionInit();
            currentMode = MotionActivation;
            break;
        
        case SequenceActivation:
        currentMode = SequenceActivation; 
        break;       
    }
}

void daliClearSequence(void){

    Serial.println("deleting any previous dali sequence rtos elements");
    if(sequenceQueueFlag) { 
        Serial.println("deleting queue"); 
        vQueueDelete(sequenceQueue); 
        sequenceQueueFlag = false;
    }
    if(sequenceTaskFlag) { 
        Serial.println("deleting task"); 
        vTaskDelete(sequenceTask); 
        sequenceTaskFlag = false;
    }
    daliSequenceFlag = false;        
    
}

bool daliSequenceInit(StaticJsonDocument<200> sequence){

    BaseType_t queueCheckOne;
    BaseType_t queueCheckTwo;

    int size = sequence["size"];
    sequenceQueue = xQueueCreate(size*2, sizeof(int));

    if(uxQueueSpacesAvailable(sequenceQueue) == size*2){
        sequenceQueueFlag = true;

        for (int i = 0; i < (size*2); i += 2){
            int lightInstruction = sequence["instructionList"][i];
            int delayInstruction = sequence["instructionList"][i+1];
            queueCheckOne = xQueueSend(sequenceQueue, &lightInstruction, portMAX_DELAY);
            queueCheckTwo = xQueueSend(sequenceQueue, &delayInstruction, portMAX_DELAY);

            if(queueCheckOne == pdFALSE || queueCheckTwo == pdFALSE){
                Serial.println("queue send failed");
                daliClearSequence();
                return false;
            }
        }

        daliSequenceFlag = true;

        if(xTaskCreatePinnedToCore(daliSequence, "dali test sequencing", 4096, (void*)size, 1, &sequenceTask, 0) == pdTRUE){
            sequenceTaskFlag = true;
            return true;
            }
    }else{
        daliClearSequence();
        Serial.println("dali sequence init failed");
        return false;
    }

    return false;
    
}

void daliSequence(void* parameters){
    int size = (int) parameters;
    int instructions[size][2];
    //Task must wait for daliSequenceInit to finish
    if(daliSequenceFlag){;
        for(int i = 0; i < size; i++){
            if (xQueueReceive(sequenceQueue, &instructions[i][0], 10) == pdFALSE) {
                Serial.print("queue receive lightlevel fail on "); 
                Serial.println(i);
                daliSequenceFlag = false;
            }

            if (xQueueReceive(sequenceQueue, &instructions[i][1], 10) == pdFALSE) {
                Serial.print("queue receive delay fail on "); 
                Serial.println(i); 
                daliSequenceFlag = false;
            }
        }
        if (uxQueueMessagesWaiting(sequenceQueue) == 0){
            vQueueDelete(sequenceQueue);
            sequenceQueueFlag = false;  
        }else {
            daliSequenceFlag = false;
            Serial.println("Queue read for dali sequence failed");
        }    
        while(daliSequenceFlag){ 
            for(int i = 0; i < size; i++){
                daliSend(instructions[i][0]);
                vTaskDelay(instructions[i][1]);
            }
        }
    }
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
