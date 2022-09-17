/*
                                                                  *****FILE HEADER*****
File Name - connect.h

Author/s - Michael Gamston - Joe Button

Description - Contains connection credentials for SIM card and WiFI

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - Whilst we build a prototype AWS certs will stay in here, once launch SPIFFS will be used to store them. 

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main

*/
#pragma once 
#include "MySPIFFS.h"

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
const char WIFI_SSID[] = "AndroidAPBEF9";               
const char WIFI_PASSWORD[] = "pqeh8163";   

/* ALL OFFICE BASED UNITS HAVE BEEN CONFIGURED WITH SPIFFS */
#ifndef SPIFFSdef

    static const char* THINGNAME = "Office_1 ";                        //change this


    static const string AWS_IOT_ENDPOINT = "a2yk3gd80zfqa3-ats.iot.eu-west-2.amazonaws.com";       //change this

    // Amazon Root CA 1
    static const char* AWS_CERT_CA PROGMEM = R"EOF(
    -----BEGIN CERTIFICATE-----
    MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
    ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
    b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
    MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
    b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
    ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
    9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
    IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
    VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
    93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
    jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
    AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
    A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
    U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
    N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
    o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
    5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
    rqXRfboQnoZsG4q5WTP468SQvvG5
    -----END CERTIFICATE-----
    )EOF";

    // Device Certificate                                               //change this
    static const char* AWS_CERT_CRT PROGMEM = R"KEY(
    -----BEGIN CERTIFICATE-----
    MIIDWjCCAkKgAwIBAgIVALjsxIZS8OheeHItF/ltdCkxcTOiMA0GCSqGSIb3DQEB
    CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
    IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjA5MDEwOTQ5
    MzVaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
    dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQChVK4KCPpY+vb97Ejy
    06k7az7KSkBtVudbJKEDOckXRUBzM8RSEYP8W7V7MOwi03vlkQW51jh/8zuFAI3N
    dClbTAXUXg83ULYCV43EUpandYTqLcB0D1kNUBgxqMbNBy62PLZDlQL5DtKwcQcr
    B6ZFQOBCPM/wA9PC7H4tfhkpY54iW+GSHDztWcGVFh8i3ljzS8VQ4S4AQGvUhDiU
    XR8MlgHYzSmTp0LBmOYaUk7Q+TSLnQFfneA4tjpRYnkurldy6YdAxHaZgHhLq2iN
    th37nV60f2p4n12oy2ajfbuXAwSlv++8qhZIaO7KnDR0wUkXFBypQmr2QGJxtG6V
    z5rHAgMBAAGjYDBeMB8GA1UdIwQYMBaAFC2hCxEsRVAmGG1ib8ZkYFmVH7qpMB0G
    A1UdDgQWBBSmsc5gPQMP6PJ+3sF+q2etF8FAxTAMBgNVHRMBAf8EAjAAMA4GA1Ud
    DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAb4d12rWLtAUtldTGcblejeuO
    aGt12cMqn2fgNeYDgQW1Sll3vheaMZZoB9/s/1jhSjJR5Okn+TU0IGBkTOQGf2C1
    KD0UtdhtZlegBLSwvrY0PNichKDrJKzT/wzLK+WyvL2XB599SGnam0Hd0jhHpWVz
    GyEuGFcdAtWvNOS7rKvGD3VU8U7d7f2g+b9x5zMkWGOXB35fOHKvGSI7HeoVfSeO
    Jhg9wo50abkx3zOlcaDoePeuJBRcrUdSDDkWLhUnFP5R5SIVZuHRKfgofGYbQL1m
    guZdY9OTv8AWZ3+fZ4k/w4i43Fwpo39eIE33rQgb7gL7f1u48CdezpIyLgC+HQ==
    -----END CERTIFICATE-----
    )KEY";

    // Device Private Key                                               //change this
    static const char* AWS_CERT_PRIVATE PROGMEM = R"KEY(
    -----BEGIN RSA PRIVATE KEY-----
    MIIEpAIBAAKCAQEAoVSuCgj6WPr2/exI8tOpO2s+ykpAbVbnWyShAznJF0VAczPE
    UhGD/Fu1ezDsItN75ZEFudY4f/M7hQCNzXQpW0wF1F4PN1C2AleNxFKWp3WE6i3A
    dA9ZDVAYMajGzQcutjy2Q5UC+Q7SsHEHKwemRUDgQjzP8APTwux+LX4ZKWOeIlvh
    khw87VnBlRYfIt5Y80vFUOEuAEBr1IQ4lF0fDJYB2M0pk6dCwZjmGlJO0Pk0i50B
    X53gOLY6UWJ5Lq5XcumHQMR2mYB4S6tojbYd+51etH9qeJ9dqMtmo327lwMEpb/v
    vKoWSGjuypw0dMFJFxQcqUJq9kBicbRulc+axwIDAQABAoIBAHhLbvZWmW1tvGu8
    GuzNfHuszQYDdaTRzvL8Df8cwF/UqGj+KZ3UJ6ktLNYttXWGYfTcPPvDyx9E25Wh
    wxp2SjwkJJikhg4F5aE7bZ78wg8dpr0C3mTr6MfveArXQV0xxNfnzzP3UV8qeVlM
    /j5MluM+q7lYJOhfwi+vLap4QwTDbscqEyHO2KlAHi0wVhpLqGOM0gODzFpArW3k
    T+IMp45wKa/PuZaD42eQRlaFN6fdYbeKdTagQEvycYJXBRoWCk9c5iQ+bmI4Mn7V
    5gUaS8+caYZQ4cs4EIfB3M0CCC1S6OltWPbgAeiBjtgOsLuBYHvoascL6k1zB+0y
    t37KR/kCgYEA0B5E3KXPpsV01aRkOiGgDDTLgMyq1RiYYc78bGEqg7l1MkAVFcKQ
    AVZqEE7P9APw1XzXs94UwPbkSPwsvZaN35BK7RnYUIHdDL1QMv0R1iv2Dg8AARrR
    UyKre6KcNjuvKVye7seKsm7RLmaZ7GFQVvHEvnQh42sdczHuV4mO9eUCgYEAxnK6
    G/KiBFdSSF7SAvTcPPAYRWURtAWBpB2BD9XHOHLnBE7/bojDQi0JrsczN8OU9aJi
    2l0tMulltA6Ky/A/FMxVPesoAMSziTD4D+ttSPV7Ku829aybM2Rb8LAolpnIcrQb
    dDKNR0Eg0LYUumeNp3C0v8gC+IkKCyIxUqFaQzsCgYEAqlQrnb5i7aXrUjLiCKWe
    fH71Euvu4MrpZBtogjcf+YDehwrldtKu0zlrQv+NuLNE9yTe0QnwSGGIQqeI4n2G
    jbNlbxnY7ULrrFKAgcqXP4bpMWZbZYwSR403dc+HM+ezGxH/zYMoY5LIxsYYMdsP
    SqlCkXEwjxUKnE4AOEqP43UCgYBnjkDOCrw/Fa4rc8eVoV84MXYI6GfnkCQJlxfd
    QopWS6d/2RV0AcwawVPXL73rcJVP5PA+0PagJalb32bQa0sPuF0XFZpnZD3n7nTU
    Id+Cfg9K5U/BvdOzasCLhfF4PUGbkZ7t8N9Gepb8Mrf4WcdSw/6eQWpDdYeXLb2d
    R/PJNwKBgQCSwJ02YyCK4nwAf0mKVW47aXQrgeDhAfP41HLyWYJLd8UMQK88VEjU
    RN4XzpmzsiNX1o302O4WJf8vvKcdINRZf27FiFD55tfW4wNkNdHwoo0UKb+sAM3S
    BpibWNVIgCQ01bRMmPlNVoJAK6AoE2WM6vFu76OWkoSc8FZaY0fHtg==
    -----END RSA PRIVATE KEY-----
    )KEY";

#endif