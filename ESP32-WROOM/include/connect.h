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
// #ifndef SPIFFS

//     const char* THINGNAME = "Joe_FiPy";                        //change this
    

//     const char* AWS_IOT_ENDPOINT = "a2yk3gd80zfqa3-ats.iot.eu-west-2.amazonaws.com";       //change this
    
//     // Amazon Root CA 1
//     static const char* AWS_CERT_CA PROGMEM = R"EOF(
//     -----BEGIN CERTIFICATE-----
//     MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
//     ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
//     b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
//     MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
//     b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
//     ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
//     9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
//     IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
//     VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
//     93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
//     jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
//     AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
//     A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
//     U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
//     N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
//     o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
//     5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
//     rqXRfboQnoZsG4q5WTP468SQvvG5
//     -----END CERTIFICATE-----
//     )EOF";
    
//     // Device Certificate                                               //change this
//     static const char* AWS_CERT_CRT PROGMEM = R"KEY(
//     -----BEGIN CERTIFICATE-----
//     MIIDWjCCAkKgAwIBAgIVALwDlrl3j/+p58Wb4e5PVFUY3tpgMA0GCSqGSIb3DQEB
//     CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
//     IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjA4MDExMTQ5
//     MDBaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
//     dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDWAq4RN/yCjFHLRHOB
//     81xdY5OOSXL9qf1l/BwyLD1Ie/4L7P+wbhtpCLOp/+Wi1tKPv21Sqy99t7aMrY9w
//     KJvatUUSJECHgIUdecTFhw+tkL0PgMXhB66sWmhGQ+3OwFCPFv51IiFlVy2lUeKF
//     TKBKH5skOzTfZavZwFyc+vJGBY9Iu1nUblDpqjfy1j1sCNE88jCCx/18NdUFGB+c
//     IU1zN6gmF7F0J++S18RiO1PftblXQs1Fw2mtcHcQpoN0lk6Yg7LXDLtTGkbmMo4v
//     ul4EuueMUoffC8vgI4jhsw2xe3YCOoFU/wNHM6m+UfCmyvGZoALeSbt63Dg0vl0r
//     kN61AgMBAAGjYDBeMB8GA1UdIwQYMBaAFEKDrwgj5ew/ezjBeZUpFANM5DBeMB0G
//     A1UdDgQWBBT0bb3dVoz6lh3wtzvs7E9Ob9WwDjAMBgNVHRMBAf8EAjAAMA4GA1Ud
//     DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAtDO/iEvLYCLZlXECAmpe5v/q
//     MhHU0uV+2HjMYmVtcUB9IciL6v5TQk2kBoKfFmieUfXaI2M0IV9WdmiWksl/ntZO
//     6SgtrBCHJT4B6bdGXXkxxckw+wUiN2UyID8cPzHjzbDiHQP1pftmYQhjYFy0J/06
//     msyBR2ONOSDRQErqzDJa63afP7JV84trTBUwj7E3T7QyqIss7Fs04abp3o3mjiTU
//     Qnyg2haMaH9RLSeZiBAFN4Zu/k1sc50hinSTJUQua909qc8u7kWUQEi0aq91NavH
//     +cb+CnH8e6g4GIeWFxuanP2ai4B8KyMY5UpbiQv0cUPTNW32Su1M7KAa46z/2A==
//     -----END CERTIFICATE-----
//     )KEY";
    
//     // Device Private Key                                               //change this
//     static const char* AWS_CERT_PRIVATE PROGMEM = R"KEY(
//     -----BEGIN RSA PRIVATE KEY-----
//     MIIEpQIBAAKCAQEA1gKuETf8goxRy0RzgfNcXWOTjkly/an9ZfwcMiw9SHv+C+z/
//     sG4baQizqf/lotbSj79tUqsvfbe2jK2PcCib2rVFEiRAh4CFHXnExYcPrZC9D4DF
//     4QeurFpoRkPtzsBQjxb+dSIhZVctpVHihUygSh+bJDs032Wr2cBcnPryRgWPSLtZ
//     1G5Q6ao38tY9bAjRPPIwgsf9fDXVBRgfnCFNczeoJhexdCfvktfEYjtT37W5V0LN
//     RcNprXB3EKaDdJZOmIOy1wy7UxpG5jKOL7peBLrnjFKH3wvL4COI4bMNsXt2AjqB
//     VP8DRzOpvlHwpsrxmaAC3km7etw4NL5dK5DetQIDAQABAoIBAQDRuDiaORIjYJy7
//     JC5JV5HZFr6/veyI9m+S5HdBF6dKg++WAj8ixxPnXxapblK7u+cF5SKd05Grkj+7
//     Jk4dLD5nGiJAv/PYlbJdCzowE8XiYadULO49MkkTL1lmGiVN6ZpITtE3OiYd6Eub
//     6V2hIVEmskhXPgn3Lew1bFOVpjmLMRauP2iAdhlSQAsmQ/DpUKvimqwgmlO1/ULM
//     gmq3lkELqKRIu6ggx6JSpEVxVg1nJQIKyCEB4VFzWAPzOASIsPZlKpfhQUTdQQn7
//     U2Xy1eyoWOmro0rlY4c2uu5qlC2L+cExTR2b8tRw54kxXaGBwQWugNd5Ph4PfIum
//     RRwEW791AoGBAPACxLDIgvsh9GGWHIPTIipzHCHtQb/W62YIGn0V+NlvWUcRX7X6
//     bGxILnB3lRJyRG/2MkvSnvopUjY1luQi8iM0rlINLgpyiKFC4anGOoz3iH8MToHJ
//     0LR1GIgKBI73lf+2YI1pffS8ihul2SQvNGOOlf/qpviEljIODXjGCeH3AoGBAORE
//     fgaSvTZwnsvJK9e4I0eLDoJkNuFMVTRx9VW6RcXCNfA4fofzycNGjanyR+bBMWym
//     K8aGafO6kAcZmm18V68Dy9K8crPliDLxsoXgZsgoTuOOnO3I7U/kb3f5LrlGBuCC
//     jjeRZnvcnu+IUJt01ONO1Czadg1ZMMDrzRe/b1mzAoGBAIA16l5Jbna657ItnyPo
//     aGrmvNN+y02wgGUr1QKW9y0n5zgolsk+cibR1ETuDGsjMV3h1XJdm+Ekq68bp3RN
//     Gq4l2S0przuTfVsvwB6SLvrYzEiaCWIZreyIVMdvGopIxg8onKFOBiPzh49HZByJ
//     PuIj7AsejX1aTmWEzjuDBwp3AoGBAM/xrTTjvEa5tY9DhhMQBeOIxAD4th3r4brT
//     DYG5HRKYZ3acoHfNGEIyJZ9pvSJzYEjv3/UMQbzseYReSJXwrhgbRzSEw6EqOjbe
//     g5j94eP5BFS8eC0N21HqcblDYNMa/ezMQi5Q4KF5S7g93+CeP8mK6DKsGNr0Izu8
//     E6vMku2NAoGAUY9MZBkuwzcqn0JARZXXiCoicqQUkfM54rKkPutBgn2C6NfqmtaR
//     wg6SLcnyQvXuAAgpXY/spNpgh2O8GKvr/0oGjE5vKOlnxNrivmgsdTicUjguzsrD
//     Y9DPUQ+elee3TgXPwZyc5kzdZmKKEJViwtfoSAdjGni4l6F5icndtdE=
//     -----END RSA PRIVATE KEY-----
//     )KEY";

// #endif