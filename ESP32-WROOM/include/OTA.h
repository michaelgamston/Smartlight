/*
                                                                  *****FILE HEADER*****
File Name - OTA.h

Author/s - Michael Gamston

Description - Function definitions for OTA updates

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for comments 

Libraries - ESP32httpUpdate.h - https://github.com/suculent/esp32-http-update


Repo - michaelgamston/MVP
Branch - main

*/
#pragma once 

#include <HTTPClient.h>
#include <ESP32httpUpdate.h>

/* 
        
        Input: const char* url 
        Output: 
            - possible error codes is update fails  
        Return: N/A  
        Action: 
            - contacts google cloud to retreive .bin file, then updates firmware with the .bin  
        Comments: N/A

    */
void runOTA(const char* url);