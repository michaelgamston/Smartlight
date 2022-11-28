
/*
                                                                  *****FILE HEADER*****
File Name - logControl.cpp

Author/s - Robert Hodorogea

Description - Functions needed to manage and send log files to AWS.

Hardware - A0.3 (ESP32-LIYGO, 2xESP32-CAM)

Comments - 

Libraries:
    - arduino - esp32 - FS.h - https://github.com/espressif/arduino-esp32/tree/master/libraries/FS
    - arduino - esp32 - SPIFFS.h - https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS

Repo - michaelgamston/MVP
Branch - main

*/
#include "logControl.h"

static char dateTimeLevelLog[35];
static const char* path = "/LogFile.txt";

void updateLogFile(int lightLevel) {

    sprintf(dateTimeLevelLog, "Level changed to %i : ", lightLevel);
    const char* suffix = ESPtime.getTime().c_str();
    strcat(dateTimeLevelLog, suffix);

    Serial.print("log file updated with '");
    Serial.print(dateTimeLevelLog);
    Serial.println("'");

    appendFile(SPIFFS, path, dateTimeLevelLog);
}

void logFileToAWS(void* parameters) {

    while(1){ 
        vTaskDelay(60000/ portTICK_PERIOD_MS);
        Serial.println("log file sent to aws");
        String contents = fileToString(SPIFFS, path);
        LTE_publish(AWS_IOT_PUBLISH_LOGFILES_TOPIC, contents.c_str());
        //opening the file in write mode and not append should clean it contents 
        createFile(SPIFFS, path);
    }
       
}

bool logFileInit(void){

    Serial.println("log file init start");
    createFile(SPIFFS, path);
    if(!checkFile(SPIFFS, path)){
        return false;
    }
    if(xTaskCreate(
        logFileToAWS,
        "sends log file to aws every 30 mins",
        8192,
        NULL,
        2,
        NULL
    ) == pdFALSE) {
        Serial.println("Task failed to create");
        return false;
        }
    
    Serial.println("log file init complete");
    return true;

}
