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
const char apn[] = "three.co.uk";
const char gprsUser[] = "";
const char gprsPass[] = "";

//WiFi credentails
const char WIFI_SSID[] = "LOL";               
const char WIFI_PASSWORD[] = "";   

/* ALL OFFICE BASED UNITS HAVE BEEN CONFIGURED WITH SPIFFS */
#ifndef SPIFFSdef

static const String THINGNAME = "Office_1";                       //change this


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
MIIDWjCCAkKgAwIBAgIVAI85+lTliIfCi4mrp+tkwgebJwgbMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjEyMDUxNDM5
NDNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCg5GXaqWrWsO0CImSi
n0gko9XPb7Z6wmr7i9AiLTstOVUBFr0f9zYlUr7qloQk2pr83GXrJ2NeGrdpJXfT
AXEiHid8Xgojbk3JQhmFhqaMdEPVQn9Jrq/QfmcxnBzGqMGQijYSytNm/FoeVDbJ
EUsXAF8Il3aHd21koLE9yXIvy7PS+V0lOydDNLLM0osWEV40/1hrH0Nlkyn0tSpD
hdwu7bNvN8up+PePJCU4Oquabys13yAoiTVBhvqpH1nAcFdJ7GDIg5bg23r0mqzJ
xzY2y427d4hWrbwljgqA+kGy6G3S8TGcOqUMClqZAIjGGWZ2XiZTKssx8S3Qw8JH
xkf7AgMBAAGjYDBeMB8GA1UdIwQYMBaAFGx8Gtua34X+rKZT7Wc/VerxGQFoMB0G
A1UdDgQWBBRZKNWNg7/6/l+09B8nNoAc6YRwfjAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAbHsFEUpsVhoXjz0ckULMHV5G
3FAOFuhsljc+EQw/Q4oiA6x9AEw58TYcr3GH+Zw6Bji2r9sczeFfgcFPoA4BvRHw
vSHY/VSiTwLfw/aasa1FxLc7xokM691CiqsFXoLMdnXlnDK4Fw4sOctkikKjoCpl
G5CeTwaTs9NLlvb+ekVXwlIYFiY6LhVNs2DCRDHS7+wx2WEPDxYxxyNiEPsE7zIe
6mvGQvWsokyWYUq5mXCxH2L5ZhK7rz//bvIzbY/3P0KW85TPQl3MbgqJYrabxq75
8HNhPA4yTbGc38PW/Tm8SK4er8FLuaOLCTZuqdLA9N8cfl29OeU7XBvlH2x2Aw==
-----END CERTIFICATE-----
)KEY";

// Device Private Key                                               //change this
static const String AWS_CERT_PRIVATE PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAoORl2qlq1rDtAiJkop9IJKPVz2+2esJq+4vQIi07LTlVARa9
H/c2JVK+6paEJNqa/Nxl6ydjXhq3aSV30wFxIh4nfF4KI25NyUIZhYamjHRD1UJ/
Sa6v0H5nMZwcxqjBkIo2EsrTZvxaHlQ2yRFLFwBfCJd2h3dtZKCxPclyL8uz0vld
JTsnQzSyzNKLFhFeNP9Yax9DZZMp9LUqQ4XcLu2zbzfLqfj3jyQlODqrmm8rNd8g
KIk1QYb6qR9ZwHBXSexgyIOW4Nt69Jqsycc2NsuNu3eIVq28JY4KgPpBsuht0vEx
nDqlDApamQCIxhlmdl4mUyrLMfEt0MPCR8ZH+wIDAQABAoIBACSiHHbfIE5l1nEn
95X3gmulSieDHh9xaP6BZRCDD/hKF4q57LH8jb2Ny56KSuhBRgzQ+fvC6iMhHiW4
vEG1EFdeWks89j3+/YMXo8v9TqqoRlGmZGkYdzuXrFNlYj/xaLFKZul8eQn352UE
c7zwTRKdqQXBxuEtViEVXYXd+jvbXziL5rfWtdRDlBk/d8bzC5AMzd4IC7MnpMbF
Dk1meyiKqqRuzhhHEwiVNzYjxchQS72k6it21NTSesCxaCm513H8vPW18DbkNUkM
wrUqke8RTUBUiSuLRh4SIe2QPBrL7ncf+aJHZleUP0cl5yGm6e+vo12V6gsfWYkL
Ob3E5NECgYEA1drlUtihd4iqBKhtrErmz7GA1L5X68jEivVei2Y7jG6dMGsF1cO/
3bWnrxM8BgWQjwXb9WlgNQP5/RUQLxtsCXO23WTmPZQC6lksPE02jKByBtjZekqb
4m3vUk69lUCmZI0Q0RyAlxYN63H2d7GiAwtDcq55GgoaFX+8Ojc+I00CgYEAwJl+
7xguiVn22+ZdpbETQA7lhQh2D/Lw3YpoIq6udTyGq3N6R9n87hLZ5VwmsKDgmWjJ
ffsEWdWZE1ig6IGkLT5XkY1bTMvlJ30ZWbRv4DfD7BOv1kE+zf5vZ1QiXLjwhi8V
/kKQxE+h8HvfPaiW5MNuGHbvQPA7NJUo1bHsZGcCgYBhxC28vy6qH/9iG8Hv11pH
ryVvnQlC0MkRofU0E2u72sHf5b2IKXJooGhlgIybUCXzTMFvI1qJQiUiT2PTtVyh
wZXjBwK/OhBPJK81bjM2/Q7EFJTv9HXpgUq8vSSsvB6Vr2v1vWsIJ1v7vGDp/XpM
m/Sc1Cim/+FG3NunXaP5rQKBgQC8Qk2s7/otD98Yu2i2WQv0BZucGdUtac6+/lcj
75NfzTXlyQ6xxQZSv+J1K0o2RErGKYMqvX5XQU5Ldspx+qa8AsAd6gpgWr03iDSL
VrA0fs3nqKary0LUn+vNGJQuBig1QTlCO1a+TQiBxNNFtdEo805eY4Mh+rAkidhM
aYoj6QKBgQCieaj6sEuV+fdVFL6m6ynwSAW+GwuJDVzR96kcEAJ4JoCxj0njJNHw
ACGh4bJ6HcxUpyrMJ7uFkGDmVFzH8iD4MurCiGHITytGnxBnDaRJcakgHwoiIRyF
bmViw+Y6qUsk13nZ77PK5UR3lbaOLKQKUpCJC1n6v4XepiGxas0oJQ==
-----END RSA PRIVATE KEY-----
)KEY";

#endif