/*
                                                                  *****FILE HEADER*****
File Name - flashSPIFFS.ino

Author/s - Michael Gamston

Description - Functions for SPIFFS handling 

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - Make sure to include secrets.h. No header file for declaring functions, just keeps file handling 

Libraries 
    - arduino - esp32 - FS.h - https://github.com/espressif/arduino-esp32/tree/master/libraries/FS
    - arduino - esp32 - SPIFFS.h - https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS



Repo - michaelgamston/MVP
Branch - main

*/


#include "MySPIFFS.h"


void setup(){

    Serial.begin(115200);
    delay(1000); //allow serial monitor to chatch up after flashing 
    //if formatting fails try again and restart ESP
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        ESP.restart();
    }

    
    listDir(SPIFFS, "/", 0);

    createFile(SPIFFS, "/logFile.txt");
    checkFile(SPIFFS, "/logFile.txt");
    // writeFile(SPIFFS, "/ThingName.txt", THINGNAME);
    // writeFile(SPIFFS, "/Endpoint.txt", AWS_IOT_ENDPOINT);
    // writeFile(SPIFFS, "/CAcert.txt", AWS_CERT_CA);
    // writeFile(SPIFFS, "/CRTcert.txt", AWS_CERT_CRT);
    // writeFile(SPIFFS, "/Privkey.txt", AWS_CERT_PRIVATE);

    listDir(SPIFFS, "/", 0);
    
    Serial.println( "SPIFFS complete");
}

void loop(){

}