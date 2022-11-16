/*
                                                                  *****FILE HEADER*****
File Name - mesh.h

Author/s - Robert Hodorogea

Description - WiFi mesh functions needed for ESP32-LILYGO. Header file for mesh.cpp

Hardware - A0.3 (ESP32-LIYGO, 2xESP32-CAM)

Comments - 

Libraries - painlessmesh - https://gitlab.com/painlessMesh/painlessMesh

Repo - michaelgamston/MVP
Branch - main

*/

#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike" //name of wifi mesh
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555


/*
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: sends a broadcast message to all mesh nodes, which includes its ID
    Comments: N/A
*/
void sendMessage();

/*
    Input: uint32_t form, String &msg
    Output: N/A
    Return: N/A  
    Action: callback function, triggers when current node has received a message from a node in the mesh
    Comments: N/A
*/
void receivedCallback( uint32_t from, String &msg );

/*
    Input: uint32_t nodeId
    Output: N/A
    Return: N/A  
    Action: callback function, triggers when a new node has connected to the current mesh node
    Comments: nodeId variable represents the ID of the newly joined node

*/
void newConnectionCallback(uint32_t nodeId);

/*
    Input: JsonArray arr, uint32_t depth
    Output: N/A
    Return: N/A  
    Action: needed by printMeshTree(), allows extraction of any existing json structured arrays to be extracted and printed to the Serial
    Comments: N/A
*/
void recursiveJsonArrayExtractor(JsonArray arr, uint32_t depth); 

/*
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: prints the mesh tree topology to Serial from its json array format
    Comments: N/A
*/
void printMeshTree();

/*
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: callback function, triggers when a connection change in the mesh is detected
    Comments: N/A
*/
void changedConnectionCallback();

/*
    Input: int32_t offset
    Output: N/A
    Return: N/A  
    Action: callback function, triggers when internal mesh time is synced across all mesh nodes
    Comments: N/A
*/
void nodeTimeAdjustedCallback(int32_t offset);

/*
    Input: uint32_t nodeId
    Output: N/A
    Return: N/A  
    Action: callback funtion, triggers when a node has disconnected from the current mesh node
    Comments: N/A
*/
void droppedConnectionCallback(uint32_t nodeId);

/*
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: performs basic initialization of mesh network
    Comments: N/A
*/
void mesh_init();


/*
    Input: N/A
    Output: N/A
    Return: N/A  
    Action: performs crucial maintenance tasks to maintain mesh functionality up and running
    Comments: should be executed at all times in a separate high-priority task

*/
void mesh_update(void);
