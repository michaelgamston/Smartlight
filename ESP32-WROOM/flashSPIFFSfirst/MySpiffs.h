/*
                                                                  *****FILE HEADER*****
File Name - MySpiffs.h

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
#include "Effortless_SPIFFS.h"
#include "FS.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true

const char* THINGNAME = "";                        //change this
 

const char* AWS_IOT_ENDPOINT = "";       //change this
 
// Amazon Root CA 1
static const char* AWS_CERT_CA PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char* AWS_CERT_CRT PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)KEY";
 
// Device Private Key                                               //change this
static const char* AWS_CERT_PRIVATE PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
)KEY";

/* 
     
    Input: N/A
    Output: prints any debug messages to serial mon
    Return: N/A  
    Action: 
        - Formats memory for SPIFFS
        - creats and then saves files for AWS certs 
    Comments: N/A

*/
void setupSPIFFS();