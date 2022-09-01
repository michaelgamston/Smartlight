/*
                                                                  *****FILE HEADER*****
File Name - OTA.h

Author/s - Michael Gamston

Description - Functions for OTA updates

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for comments 

Libraries - 

Repo - michaelgamston/MVP
Branch - main

*/
#include "OTA.h" 

void runOTA(const char* url){
    t_httpUpdate_return ret = ESPhttpUpdate.update(url);
    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            break;
        default: Serial.println("Update Failed");
    }

}