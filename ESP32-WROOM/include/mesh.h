#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

void receivedCallback( uint32_t from, String &msg );

void newConnectionCallback(uint32_t nodeId);

void recursiveJsonArrayExtractor(JsonArray arr, uint32_t depth); 

void changedConnectionCallback();

void nodeTimeAdjustedCallback(int32_t offset);

void droppedConnectionCallback(uint32_t nodeId);

void mesh_init();

void mesh_update(void);