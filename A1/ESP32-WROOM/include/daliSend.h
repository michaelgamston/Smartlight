/*
                                                                  *****FILE HEADER*****
File Name - daliSend.ino

Author/s - Michael Gamston

Description - Functions for sending a command to our dali control unit

Hardware - A0.4

Comments - 

Libraries 
    
Repo - michaelgamston/MVP
Branch - main

*/

#pragma once

#include <ArduinoJson.h>

extern bool daliTestSwitchFlag; 


/* 
    
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: 
        - initialises pins for software serial and initialises the bus
    Comments: N/A

*/
void daliINIT(void);

void daliClearSequence(void);

bool daliSequenceInit(StaticJsonDocument<200> sequence);

void daliSequence(void* parameters);

/* 
    
    Input: 
        - int LightLevel 
    Output: send the new lightLevel percentage to dali controller
    Return: N/A  
    Action: 
        - checks that it isn't sending current lightlevel before sending 
    Comments: N/A

*/
void daliSend(int lightLevel);