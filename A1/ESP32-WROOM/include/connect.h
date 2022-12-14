/*
                                                                  *****FILE HEADER*****
File Name - connect.h

Author/s - Michael Gamston - Joe Button

Description - Contains connection credentials for SIM card and WiFI

Hardware - A0.4

Comments - Whilst we build a prototype AWS certs will stay in here, once launch SPIFFS will be used to store them. 

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main

*/
#pragma once 
#include "MySPIFFS.h"

//SIM credentails 
const char apn[] = "everywhere";
const char gprsUser[] = "eesecure";
const char gprsPass[] = "password";