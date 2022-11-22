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

#define ACTIVATE_BY_TIME

//complete development on this feature, use ACTIVATE_BY_TIME
//#define ACTIVATE_BY_MOTION 

#ifdef ACTIVATE_BY_MOTION

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

#endif

#ifdef ACTIVATE_BY_TIME
/* 
    
    Input: N/A
    Output: calls daliSend to either activate or deactivate the light
    Return: N/A  
    Action: 
        - looks at what the time is, and compares it to hardcoded activation or deactivate times
    Comments: N/A

*/
void daliTimeActivation(void* parameters);
#endif
/* 
    
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: 
        - initialises pins for software serial and initialises the bus
    Comments: N/A

*/
void daliINIT(void);

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