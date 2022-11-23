/*
                                                                  *****FILE HEADER*****
File Name - logControl.h

Author/s - Robert Hodorogea

Description - Log control file  functions header needed by logControl.cpp

Hardware - A0.3 (ESP32-LIYGO, 2xESP32-CAM)

Comments - 

Libraries:
    - arduino - esp32 - FS.h - https://github.com/espressif/arduino-esp32/tree/master/libraries/FS
    - arduino - esp32 - SPIFFS.h - https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS

Repo - michaelgamston/MVP
Branch - main

*/

#pragma once

#include "MySPIFFS.h"
#include "AWS_funcs.h"
#include "ESPtime.h"

/*
    Input: const char* DateTimeLevel, const char* path
    Output: N/A
    Return: N/A  
    Action: updates log file at specified path with current date, time and light level in percentages
    Comments: N/A
*/
void updateLogFile(int lightLevel);

/*
    Input: const char* path
    Output: N/A
    Return: N/A  
    Action: sends log file's contents in string format to AWS and deletes the file
    Comments: N/A
*/
void logFileToAWS(const char* path);
