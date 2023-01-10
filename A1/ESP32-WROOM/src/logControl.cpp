
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

static char dateTimeLevelLog[35] = "";
static char head[25] = "";
static char tail[25] = "";
static const char* path = "/LogFile.txt";
static int lines = 1;
static int version = 1;
static char buffer[256];

void updateLogFile(int lightLevel) {
    sprintf(dateTimeLevelLog, "UUUUUUUUUUUU %i %i %i %i.00I\n", ESPtime.getHour(true), ESPtime.getMinute(), ESPtime.getSecond(), lightLevel);
    lines++;
    Serial.print("log file updated with ");
    Serial.println(dateTimeLevelLog);
    appendFile(SPIFFS, path, dateTimeLevelLog);
}

void logFileToAWS(void* parameters) {

    while(1){ 
        // every 30 mins is 1800000ms 
        //CHANGE TO SEND ON THE HOUR AND HALF HOUR 
        vTaskDelay(60000/ portTICK_PERIOD_MS);
        if(checkSize(SPIFFS, path) > 0){
            Serial.println("Sending log file to aws");
            sprintf(tail, "T%i", lines+1);
            appendFile(SPIFFS, path, tail);
            String contents = fileToString(SPIFFS, path);
            jsonify(contents);
            LTE_publish(buffer, AWS_IOT_PUBLISH_LOGFILES_TOPIC);
            //opening the file in write mode and not append should clean it contents 
            createFile(SPIFFS, path);
            version++;
            sprintf(head, "HMMMMMMM %i %i %i VVV\n", ESPtime.getYear(), ESPtime.getMonth()+1, ESPtime.getDay());
            appendFile(SPIFFS, path, head);
            lines = 1;
            Serial.println("Log file sent to aws");
        }else{
            appendFile(SPIFFS, path, "No light level changes recorded.");
            String contents = fileToString(SPIFFS, path);
            jsonify(contents);
            LTE_publish(buffer, AWS_IOT_PUBLISH_LOGFILES_TOPIC);
        }
    }
       
}

bool logFileInit(void){

    Serial.println("Log file init start");
    createFile(SPIFFS, path);
    if(!checkFile(SPIFFS, path)){
        return false;
    }
    sprintf(head, "HMMMMMMM %i %i %i VVV\n", ESPtime.getYear(), ESPtime.getMonth()+1, ESPtime.getDay());
    appendFile(SPIFFS, path, head);
    if(xTaskCreatePinnedToCore(
        logFileToAWS,
        "sends log file to aws every 30 mins",
        8192,
        NULL,
        2,
        NULL,
        0
    ) == pdFALSE) {
        Serial.println("Task failed to create");
        return false;
        }
    
    Serial.println("log file init complete");
    return true;

}

void jsonify(String logfile){
    memset(buffer, 0 , 256);
    StaticJsonDocument<200> doc;
    doc["logfile"] = logfile;
    doc["version"] = version;
    serializeJson(doc, buffer);
}