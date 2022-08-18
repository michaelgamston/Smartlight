/*
                                                                  *****FILE HEADER*****
File Name - connect.h

Author/s - Michael Gamston - Joe Button

Description - Contains connection credentials for SIM card and WiFI

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - N/A

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main

*/
#pragma once 
//TinyGSM (LTE)
//#define TINY_GSM_MODEM_SIM7000
//#define TINY_GSM_GPRS true
//#define SerialMon Serial 
//SoftwareSerial SerialAT(22,23);
//#define SerialAT Serial1
// #define TINY_GSM_DEBUG SerialMon

//ESP32 serial2 
//#define MODEM_UART_BAUD 115200
//#define MODEM_DTR 25
//#define MODEM_TX 17
//#define MODEM_RX 16
//#define MODEM_PWRKEY 12

//SIM credentails 
const char apn[] = "eezone";
const char gprsUser[] = "eesecure";
const char gprsPass[] = "password";

//WiFi credentails
const char WIFI_SSID[] = "dlink_DWR-932_4CC7";               
const char WIFI_PASSWORD[] = "NxNqD79725";   
