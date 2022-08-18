/*
                                                                  *****FILE HEADER*****
File Name - SPIFFS.ino

Author/s - Michael Gamston

Description - To format Memory space for SPIFFS and then save credentials 

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - File should not be stored on module once SPIFFS is setup and working. The point of SPIFFS is to get rid 
        of file like MySpiffs.h so credentails don't have to be hardcoded and can avoid being overwritten during updates

Libraries 
    - Effortless_SPIFFS: https://github.com/thebigpotatoe/Effortless-SPIFFS
    - SPIFFS: https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS


Repo - michaelgamston/MVP
Branch - main

*/
#include "MySpiffs.h"


void setup(){

    //delay to allow serial monitor to catch up 
    delay(5000);
    Serial.begin(115200);
    eSPIFFS spiffs(&Serial);
    bool format = false; 

    if(!spiffs.checkFlashConfig()){
        format = true;
    }
    else Serial.println("SPIFFS already configured");

    if(format) {
        Serial.println("Configuring SPIFFS");
        if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
            Serial.println("SPIFFS failed to formate, ESP resarting.");
            delay(5000);
            ESP.restart();
        }
        else Serial.println("SPIFFS formated");  
    }

    Serial.println("Saving credentials");
    spiffs.saveToFile("/ThingName.txt", THINGNAME);
    spiffs.saveToFile("/CAcert.txt", AWS_CERT_CA);
    spiffs.saveToFile("/CRTcert.txt", AWS_CERT_CRT);
    spiffs.saveToFile("/Privkey.txt", AWS_CERT_PRIVATE);
    spiffs.saveToFile("/Endpoint.txt", AWS_IOT_ENDPOINT);

}

void loop(){}