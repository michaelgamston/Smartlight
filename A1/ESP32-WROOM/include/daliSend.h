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

enum daliMode{
    TimeActivation,
    MotionActivation,
    SequenceActivation,
    High,
    Low,
    Standby,
    CustomLevelActivation
};

//#define ACTIVATE_BY_TIME

//complete development on this feature, use ACTIVATE_BY_TIME
//#define ACTIVATE_BY_MOTION 

/* 
    
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: 
        - initialises pins for software serial and initialises the bus
    Comments: N/A

*/
void daliINIT(void);

void daliMotionInit(void);

/* 
    
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: 
        - RTOS task that checks the status of the flag every 10 seconds 
        - calls daliSend to either activate or deactivate the lamp based on the flag 
    Comments: N/A

*/
void checkActivationFlag(void* parameters);

/* 
    
    Input: bool status
    Output: N/A
    Return: N/A  
    Action: 
        - changes the status of the dali activation flag
    Comments: N/A

*/
void daliChangeFlagStatus(bool status);

/* 
    
    Input: N/A
    Output: calls daliSend to either activate or deactivate the light
    Return: N/A  
    Action: 
        - looks at what the time is, and compares it to hardcoded activation or deactivate times
    Comments: N/A

*/
void daliTimeActivation(void* parameters);

void daliSelectMode(daliMode newMode);

void daliClearSequence(void);

/* 
    
    Input: int *lightTime 
    Output: sets the lightTime array for use in the thread
    Return: N/A  
    Action: 
        - creates thread for daliTestSwitch
    Comments: N/A

*/
bool daliSequenceInit(StaticJsonDocument<200> sequence);

/* 
    
    Input: int *lightTime 
    Output: sends selected light levels through to the dali controler 
    Return: N/A  
    Action: 
        - runs through the given array 
        - send a light level from the array and the waits the given time before sending the next 
    Comments: N/A

*/
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