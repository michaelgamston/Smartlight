/*
                                                                  *****FILE HEADER*****
File Name - OTA.h

Author/s - Michael Gamston

Description - Function definitions for OTA updates

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for comments 

Libraries - 


Repo - michaelgamston/MVP
Branch - main

*/
#pragma once 

#include <HTTPClient.h>
#include <ESP32httpUpdate.h>


void runOTA(const char* url);