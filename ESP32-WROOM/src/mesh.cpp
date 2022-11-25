/*
                                                                  *****FILE HEADER*****
File Name - mesh.cpp

Author/s - Robert Hodorogea

Description - WiFi mesh functions needed for ESP32-LILYGO.

Hardware - A0.3 (ESP32-LIYGO, 2xESP32-CAM)

Comments - 

Libraries - painlessmesh - https://gitlab.com/painlessMesh/painlessMesh

Repo - michaelgamston/MVP
Branch - main

*/

#include "mesh.h"

painlessMesh  mesh;

static TaskHandle_t* meshUpdater;
static TaskHandle_t* messageBroadcaster;

void sendMessage(void* parameters) {
  while(1) {
    String msg = "Hi from node 10 ";
    msg += mesh.getNodeId();
    mesh.sendBroadcast( msg );
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

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

void printMeshTree() {
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

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  printMeshTree();
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void droppedConnectionCallback(uint32_t nodeId) {
  Serial.printf("Connection dropped, nodeId %u \n", nodeId);
}

void mesh_init() {
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onDroppedConnection(&droppedConnectionCallback);

  //mesh.setRoot(true); // no more than one node should be set as root/parent node
  mesh.setContainsRoot(true); // if a root/parent node was set, the rest of the nodes should be aware of it

  xTaskCreatePinnedToCore(
    mesh_update,
    "Mesh updater",
    10240,
    NULL,
    1,
    meshUpdater,
    0
  );

  xTaskCreatePinnedToCore(
    sendMessage,
    "Message sender",
    2048,
    NULL,
    1,
    messageBroadcaster,
    1
  );

}

void mesh_update(void* parameters) {
  while(1) {
    mesh.update();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void mesh_stop() {
  mesh.stop();
  vTaskDelete(meshUpdater);
  vTaskDelete(messageBroadcaster);
}
