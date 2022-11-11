#include "mesh.h"

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hi from node 10 ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void recursiveJsonArrayExtractor(JsonArray arr, uint32_t depth = 1) {
  JsonObject obj;
  uint32_t i = 0;
  for(i = 0; !(arr[i].isNull()); i++) {
    obj = arr[i].as<JsonObject>();
    for(JsonPair p : obj) {
      for(uint32_t j = 1; j <= depth; j++)
        Serial.print("| ");
      Serial.print(p.key().c_str());
      Serial.print(" ");
      if(strstr(p.key().c_str(), "subs")) 
        Serial.print("\n");
      if(p.value().is<JsonArray>())
        recursiveJsonArrayExtractor(p.value().as<JsonArray>(), depth + 1);
      else
        Serial.println(p.value().as<String>());
    }
  }
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  StaticJsonDocument<2500> doc;
  String form = mesh.subConnectionJson();
  deserializeJson(doc, form);
  JsonObject obj = doc.as<JsonObject>();
  //Serial.println(doc.as<String>());
  for(JsonPair p : obj) {
    Serial.print(p.key().c_str());
    Serial.print(" ");
    if(strstr(p.key().c_str(), "subs")) 
      Serial.print("\n");
    if(p.value().is<JsonArray>()) {
      JsonArray arr = p.value().as<JsonArray>();
      recursiveJsonArrayExtractor(arr);
    }
    else
      Serial.println(p.value().as<String>());
  }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void droppedConnectionCallback(uint32_t nodeId) {
  Serial.printf("Connection dropped, nodeId %u \n", nodeId);
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