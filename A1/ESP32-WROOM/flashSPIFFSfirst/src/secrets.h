/*
                                                                  *****FILE HEADER*****
File Name - secrets.h

Author/s - Michael Gamston - Joe Button

Description - Contains connection credentials for SIM card and WiFI

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - fiels to be stored in spiffs

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main

*/
#pragma once
#include <pgmspace.h>


static const char* THINGNAME = "";                        //change this


static const char* AWS_IOT_ENDPOINT = "a2yk3gd80zfqa3-ats.iot.eu-west-2.amazonaws.com";       //change this

// Amazon Root CA 1
static const char* AWS_CERT_CA PROGMEM = R"EOF(
)EOF";

// Device Certificate                                               //change this
static const char* AWS_CERT_CRT PROGMEM = R"KEY(

)KEY";

// Device Private Key                                               //change this
static const char* AWS_CERT_PRIVATE PROGMEM = R"KEY(
)KEY";
