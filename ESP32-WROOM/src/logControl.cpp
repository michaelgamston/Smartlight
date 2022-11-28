
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
#include "MySPIFFS.h"
#include "AWS_funcs.h"
#include "ESPtime.h"

static char dateTimeLevelLog[35];

void updateLogFile(int lightLevel) {

    sprintf(dateTimeLevelLog, "Level changed to %i : ", lightLevel);
    const char* suffix = ESPtime.getTime().c_str();
    strcat(dateTimeLevelLog, suffix);

    Serial.print("log file updated with '");
    Serial.print(dateTimeLevelLog);
    Serial.println("'");

    appendFile(SPIFFS, "/LogFile.txt", dateTimeLevelLog);
}

//create a thread for this function to be called every 30 mins
void logFileToAWS(const char* path = "/LogFile.txt") {
    Serial.println("log file sent to aws");
    String contents = fileToString(SPIFFS, path);
    LTE_publish(contents.c_str());
    deleteFile(SPIFFS, path);
}
