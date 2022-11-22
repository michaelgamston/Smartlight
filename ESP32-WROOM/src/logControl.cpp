
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

void updateLogFile(const char* DateTimeLevel = "YYYYMMDD_HHMMSS_%", const char* path = "/LogFile.txt") {
    appendFile(SPIFFS, path, DateTimeLevel);
    appendFile(SPIFFS, path, "\n");
}

void sendToAWS(const char* path = "/LogFile.txt") {
    String contents = fileToString(SPIFFS, path);
    LTE_publish(contents.c_str());
    deleteFile(SPIFFS, path);
}
