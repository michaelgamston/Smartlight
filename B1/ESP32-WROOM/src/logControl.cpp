
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

    sprintf(dateTimeLevelLog, "Level changed to %i%.\n Change occured at ", lightLevel);
    const String prefix = ESPtime.getTime();
    const char* pfix = prefix.c_str();
    strcat(dateTimeLevelLog, pfix);
    Serial.print("log file updated with '");
    Serial.println(dateTimeLevelLog);

    appendFile(SPIFFS, path, dateTimeLevelLog);
}

void logFileToAWS(void* parameters) {

    while(1){ 
        // 60000ms or triggers every minuet
        vTaskDelay(60000/ portTICK_PERIOD_MS);
        if(checkSize(SPIFFS, path) > 0){
            Serial.println("Log file sent to aws");
            String contents = fileToString(SPIFFS, path);
            LTE_publish(contents.c_str(), AWS_IOT_PUBLISH_LOGFILES_TOPIC);
            //opening the file in write mode and not append should clean it contents 
            createFile(SPIFFS, path);
            Serial.println("Log file sent to aws");
        }    
    }
       
}

bool logFileInit(void){

    Serial.println("Log file init start");
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
