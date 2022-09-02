/*
                                                                  *****FILE HEADER*****
File Name - MySPIFFS.ino

Author/s - Michael Gamston

Description - Functions definitions for SPIFFS handling 

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - Make sure to include secrets.h. No header file for declaring functions, just keeps file handling 

Libraries 
    - arduino - esp32 - FS.h - https://github.com/espressif/arduino-esp32/tree/master/libraries/FS
    - arduino - esp32 - SPIFFS.h - https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS



Repo - michaelgamston/MVP
Branch - main

*/

#include "FS.h"
#include "SPIFFS.h"


#define SPIFFSdef

/* 
   You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
*/

#define FORMAT_SPIFFS_IF_FAILED false

/* 
    
    Input: 
        -fs:FS &fs (file system object)
        - const char* dirname 
        - unit*_t levels 
    Output: lists all of the directories SPIFFS
    Return: N/A  
    Action: 
        - uses the files objects methods to search through system and then print out file names 
    Comments: N/A

*/
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

/* 
    
    Input:
        - fs::FS &fs (file system object)
        - const char* path 
    Output: 
        - prints out converted binary writen in the file 
    Return: N/A  
    Action: 
        - using file object methods to find a read
        and serial.write to convert binary to ascii and print to 
        serial monitor 
    Comments: N/A

*/
void readFile(fs::FS &fs, const char * path);

/* 
    
    Input: 
        - fs::FS &fs (file system object)
        - const char* path
    Output: N/A
    Return: 
        - Returns the file in string format
    Action: 
        - uses the SPIFFS function readString to convert file to string
    Comments: N/A

*/
String fileToString(fs::FS &fs, const char * path);

/* 
    
    Input:
        - fs::FS &fs (file system object)
        - const char* path
        - const char* message 
    Output: 
        - writes message to selected file path
    Return: N/A  
    Action: 
        - uses the file objects methods to open / create a file 
        and the write selected string to it 
    Comments: N/A

*/
void writeFile(fs::FS &fs, const char * path, const char * message);

/* 
    
    Input: 
        - fs::FS &fs (file system object)
        - const char* path
        - const char* message
    Output: 
        - adds text to selected file 
    Return: N/A  
    Action: 
        - uses the file object methods to add text 
    Comments: N/A

*/
void appendFile(fs::FS &fs, const char * path, const char * message);

/* 
    
    Input: 
        - fs::FS &fs (file system object)
        - const char* path
        - const char* message
    Output: Fills AWS certification variables 
    Return: N/A  
    Action: 
        - uses the file object methods rename file  
    Comments: N/A

*/
void renameFile(fs::FS &fs, const char * path1, const char * path2);

/* 
    
    Input: 
        - fs::FS &fs (file system object)
        - const char* path
        - const char* message
    Output: 
        - deletes selected file
    Return: N/A  
    Action: 
        - uses the file object methods to delete file 
    Comments: N/A

*/
void deleteFile(fs::FS &fs, const char * path);

