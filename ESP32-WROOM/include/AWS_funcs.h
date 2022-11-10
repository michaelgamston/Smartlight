/*
                                                                  *****FILE HEADER*****
File Name - AWS_funcs.h

Author/s - Michael Gamston - Joe Button

Description - Function definitions for connecting the module to WIFI/LTE and AWS 

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for comments 

Libraries - WiFIClientSecure: https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure
            PubSubClient: https://github.com/knolleary/pubsubclient
            Effortless_SPIFFS: https://github.com/thebigpotatoe/Effortless-SPIFFS


Repo - michaelgamston/MVP
Branch - main

*/
#pragma once
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "Effortless_SPIFFS.h"


extern WiFiClientSecure net;

// Constants
extern const char *AWS_IOT_PUBLISH_IMAGES_TOPIC;
extern const char *AWS_IOT_PUBLISH_PARAMS_TOPIC;
extern const char *AWS_IOT_SUBSCRIBE_TOPIC;

#ifdef SPIFFSdef
    
    /* 
        
        Input: N/A
        Output: Fills AWS certification variables 
        Return: N/A  
        Action: 
            - uses eSPIFFS to read certs from SPIFFS memory  
        Comments: N/A

    */
    void getSPIFFS();
#endif

/* 
     
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: Checks MQTT broker and calls the call back function

*/
void checkMQTT(void);

/* 
     
    Input: 
        - char *topic
        - byte *payload
        - unsigned int length

    Output: prints in coming payloads from MQTT topic
    Return: N/A  
    Action:
        - Receives incomming MQTT payload, deserializes it into a char*
        and then prints it to the serial monitor.   
    Comments: When impelement in a lamp it will print this to a handler function
            which will decide what happens (won't have a serial mon at this stage)

*/
void messageHandler(char *topic, byte *payload, unsigned int length);

/* 
     
    Input: N/A
    Output: DEBUG message for connection process 
    Return: true on success 
    Action:
        - Connects WIFI 
        - Sets AWS certs
        - Connects to AWS 
        - Subscribes to a topic      
    Comments: When we have LTE functionality we will not use WIFI

*/
bool connectAWS();

/* 
     
    Input: 
        - uint8_t *im 
        - size_t size
    Output: N/A
    Return: N/A  
    Action:
        - Publishes image to AWS s3      
    Comments: N/A

*/
void send_image(uint8_t *im, size_t size);

/* 
     
    Input: N/A
    Output: N/A
    Return: N/A  
    Action:
        - Publishes parameters to AWS MQTT broker
    Comments: Parameters will chance when A0.3 model is complete

*/
void send_params();

/*
    Input: N/A
    Output: true on success
    Return: N/A  
    Action:
        - Test GSM connect with HTTP request to if is connection avaiable 
    Comments: 

*/
bool http_test();

/*
    Input: N/A
    Output: true on success
    Return: N/A  
    Action:
        - Connects GSM modem to network and GPRS
    Comments: 

*/
bool LTE_connect();

/*
    Input: N/A
    Output: true on success
    Return: N/A  
    Action:
        - test function to send simple message
    Comments: 

*/
bool LTE_publish(const char *message);