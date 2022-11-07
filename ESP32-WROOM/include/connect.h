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

static const String THINGNAME = "Office_2";                        //change this


static const String AWS_IOT_ENDPOINT = "a2yk3gd80zfqa3-ats.iot.eu-west-2.amazonaws.com";       //change this

// Amazon Root CA 1
static const String AWS_CERT_CA PROGMEM = R"EOF(
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
static const String AWS_CERT_CRT PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUMcopFHX15k7uK4xfyczcsoRW4AgwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIyMTAyNDEyMTAw
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAN1NH1gHNtrgt9DfIwUa
4XtQMa1hs/XduR8pf3VBUP2zbztM1XuIB5E3lYMxMsUUmjpjTpRtWtWbblryHgU4
y/yjeTvbSzwBbRwn+j4ilKSmn6tUOKY9IG1MiCqfHHdPcHsd4VQtYr7P/g4ExHoI
GSPr1ivJSDx6vxPRbOpJWXTW3qDg1X9CUm8yKR5I60Z0I4k1xQkRKxQHepRogYlA
TlwZkgTVfxjTmkMwMCAJkZeaIaDy3HF75IlgRdyem7ZXrK1GAqkBU54FII5331zF
0XJeq70wK39J59SBqgzL9FPD1CUIV+UDHY3i3yuwObtTMJt3pkCZZzg2RfowzSiy
pe0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUaJ0VdR5Tfn82YlN7vSKJemmIK64wHQYD
VR0OBBYEFEJLbGaA2fWAVCq2s5korlR4/73BMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAUo0Bnlkv24A5GQOp36vfjf0cQ
oYAqsmZ/rF6tIxg0xC7sJFsXG4PUQIpFgmgQxUVet1DIvyBm7tg0kqeklHB3Kkpu
81GrXWxiAkH/TO6qDeYu+1cbU8ftiIlYqNHcLDtrkvnRFwlKUAYhlQI9Vd8U52AK
ZspAzOKatwL5TPAmHt6cUcN8Yt6naejhHay6xqgULvNLhZdS7zGoXBY/WuNI0Nvn
0Jfld3fpTrGDx552t7d4RG3lPkLZIGs85nJmiSzi27HuUnoFVIC7g5XAQn+DJXUz
FFFgRiIu+ZLrHmMPtnEpKH/XIGNznK5YFnlzbL/Tssmh6r9lINdjLo+mmbZI
-----END CERTIFICATE-----
)KEY";

// Device Private Key                                               //change this
static const String AWS_CERT_PRIVATE PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA3U0fWAc22uC30N8jBRrhe1AxrWGz9d25Hyl/dUFQ/bNvO0zV
e4gHkTeVgzEyxRSaOmNOlG1a1ZtuWvIeBTjL/KN5O9tLPAFtHCf6PiKUpKafq1Q4
pj0gbUyIKp8cd09wex3hVC1ivs/+DgTEeggZI+vWK8lIPHq/E9Fs6klZdNbeoODV
f0JSbzIpHkjrRnQjiTXFCRErFAd6lGiBiUBOXBmSBNV/GNOaQzAwIAmRl5ohoPLc
cXvkiWBF3J6btlesrUYCqQFTngUgjnffXMXRcl6rvTArf0nn1IGqDMv0U8PUJQhX
5QMdjeLfK7A5u1Mwm3emQJlnODZF+jDNKLKl7QIDAQABAoIBABEd3vfDDxYc3XrV
V3MUoi4UhNMhRHcW4DOt1QLvu2D0GP1gMv7sBZHQ18qe6otdzZQIjFJkpurwq7dz
FKlYvEZdKopbqtUIUzUjTnKx4AdgL+dji0IoZjKuAbYyOlRsG4oaRIxrOkEB+2eA
LOY3bCJgIl+eoOy0ncTdpOv2Imor85QlRddqRu1obRQENrhd5Qzy30y9HA3Y8vIT
l/SEd84UzdphR1uXkw/3UG0TbNgkWKZH5JQtWQqKOiRKRdayuEFfIfUvLQ61Pq92
vPbOze233RQX22JdWsLg7v34STYIE7O/XKi8x7oJsuPJJn2fz0ri6gZSgt/5X3rz
K8xB9gECgYEA9TjJDgSbCKyA7OK9CB7PYa0E8LbmyBgFCo0Mz2+AzgjmlSTyJCXH
UZO+LIT09rinpHZqFDedpstpwRmLxYcQ9LpbvGFRmxJiLJRchYWiErXHnzSVlYV1
tpo9fWrQWLYlFNq02p449AwSgcehCvzQmhhsjnEjdouwGcEvCqnXZW0CgYEA5wcv
XWNQqHg7o73UX1zxmhJVe2eb07WbrKGROjNh2jFU1AIfXbvNElEWLgI3GkLvA8aP
tUirUrTb3nrVUUMYgrbMAa5NRzjyoIKYX/lkc+pOSMkzVV8M2bGrwzzclwD+3jUC
tuU/ALW1pv/10Ou+5I3xCbVF5toQuu2Sqm82coECgYEApjBg2dY3N46pJWGY2KsA
wuKeSYC7MPpOWFM1TlP0CC4yDvvl+qd7yIaHSOammZqVSrlRtPfoKQUMW+izOCAM
Vr8Bss6FnsG7tRirB3aEW4fEjNn3UkkLeMBZUekU4Do5MUPzc4HteT/BPyEKvEaQ
HQ7Wa0az0QBkTWAGQITQTJUCgYEA1foVqI1/+0nBE1uTA/B15Zntiqrf7Spb0Xyj
oiBNFWZzhxmgK5Txz3FC9cIffzdXYUvbdrCj3aqdqmZihS172jWspjHmaX0ZXAkZ
UGsQ2zu83jVcUkoDAO5KhD4uob3OUckPho6119Z1NQQFjUeMOuwswC0hXTvEGx5H
pYv0BgECgYEAk4j/pWZ91Phl2pyB2/bF3CbCXRDrLvu403nCrWFaB9nxV7TynQ5p
+bKYLWUwF8i81WSgrRfiFGx7gTpDyILJ2nr/3hK+19onspgn5QGcaP3HoLa7vNui
EYZVKzTzmaaT6P1kEI+FIrcK2xZpTUT7EypfarCkUSVFtefPHzolxgA=
-----END RSA PRIVATE KEY-----
)KEY";

#endif