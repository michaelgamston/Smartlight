#include "mesh.h"
#include "queue.h"

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
static char buffer[50];

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hi from node 10 ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  queueMessage(("startHere: Received from %u msg=%s\n", from, msg.c_str()));
}

void newConnectionCallback(uint32_t nodeId) {
  sprintf(buffer, "--> startHere: New Connection, nodeId = %u\n", nodeId);
  queueMessage(buffer);
}

void recursiveJsonArrayExtractor(JsonArray arr, uint32_t depth = 1) {
  JsonObject obj;
  uint32_t i = 0;
  for(i = 0; !(arr[i].isNull()); i++) {
    obj = arr[i].as<JsonObject>();
    for(JsonPair p : obj) {
      for(uint32_t j = 1; j <= depth; j++)
        queueMessage("| ");
      queueMessage(p.key().c_str());
      queueMessage(" ");
      if(strstr(p.key().c_str(), "subs")) 
        queueMessage("\n");
      if(p.value().is<JsonArray>())
        recursiveJsonArrayExtractor(p.value().as<JsonArray>(), depth + 1);
      else
        queueMessage(p.value().as<String>().c_str());
    }
  }
}

void changedConnectionCallback() {
  queueMessage("Changed connections\n");
  StaticJsonDocument<2500> doc;
  String form = mesh.subConnectionJson();
  deserializeJson(doc, form);
  JsonObject obj = doc.as<JsonObject>();
  //queueMessageln(doc.as<String>());
  for(JsonPair p : obj) {
    queueMessage(p.key().c_str());
    queueMessage(" ");
    if(strstr(p.key().c_str(), "subs")) 
      queueMessage("\n");
    if(p.value().is<JsonArray>()) {
      JsonArray arr = p.value().as<JsonArray>();
      recursiveJsonArrayExtractor(arr);
    }
    else
      queueMessage(p.value().as<String>().c_str());
  }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  sprintf(buffer, "Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
  queueMessage(buffer);
}

void droppedConnectionCallback(uint32_t nodeId) {
  sprintf(buffer, "Connection dropped, nodeId %u \n", nodeId);
  queueMessage(buffer);
}

void mesh_init() {
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onDroppedConnection(&droppedConnectionCallback);

  //mesh.setRoot(true);
  mesh.setContainsRoot(true);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void mesh_update(void) {
  mesh.update();
}