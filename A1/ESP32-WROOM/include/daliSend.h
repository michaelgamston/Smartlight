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

/* 
    
    Input: N/A 
    Output: N/A
    Return: N/A  
    Action: 
        - Deletes daliSquenceQueue
        - Deletes dalueSquenceTask
        - changes daliSequenceFlag to false 
    Comments: N/A

*/
void daliClearSequence(void);

/* 
    
    Input: 
        - StaticJsonDocument<200 bytes> sequence  
    Output: 
        - Queues instructions and delays
    Return: N/A  
    Action: 
        - Decode json document 
        - Creates Queue
        - Fills Queue 
        - Creates task
    Comments: N/A

*/
bool daliSequenceInit(StaticJsonDocument<200> sequence);

/* 
    
    Input: 
        - void* parameters 
    Output: calls daliSend with instructed light level and the delays an instructed amount of time 
    Return: N/A  
    Action: 
        - creats array of instructions 
        - takes intructions from the queue and into the array 
        - loops over instructions and delays  
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