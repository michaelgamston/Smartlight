/*
                                                                  *****FILE HEADER*****
File Name - AWS_funcs.cpp

Author/s - Michael Gamston - Joe Button

Description - Function definitions for connecting the module to WIFI/LTE and AWS 

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - Must run SPIFFS.ino first to format memory for SPIFFS and save certs, otherwise you will not be able to 
            acess required certs as they won't exsist. 

Libraries - WiFIClientSecure: https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure
            PubSubClient: https://github.com/knolleary/pubsubclient
            Effortless_SPIFFS: https://github.com/thebigpotatoe/Effortless-SPIFFS


Repo - michaelgamston/MVP
Branch - main

*/
#include "AWS_funcs.h"
#include <ArduinoJson.h>
#include "connect.h"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
eSPIFFS spiffs;

//topics 
const char *AWS_IOT_PUBLISH_IMAGES_TOPIC = "FiPy/images";
const char *AWS_IOT_PUBLISH_PARAMS_TOPIC = "FiPy/params";
const char *AWS_IOT_SUBSCRIBE_TOPIC = "FyPi/sub";

//SPIFFS credentials
const char* THINGNAME;
const char* AWS_CERT_CA;
const char* AWS_CERT_CRT;
const char* AWS_CERT_PRIVATE;
const char* AWS_IOT_ENDPOINT;

void getSPIFFS(){
  spiffs.openFromFile("/ThingName.txt", THINGNAME);
  spiffs.openFromFile("/CAcert.txt", AWS_CERT_CA);
  spiffs.openFromFile("/CRTcert.txt", AWS_CERT_CRT);
  spiffs.openFromFile("/Privkey.txt", AWS_CERT_PRIVATE);
  spiffs.openFromFile("/Endpoint.txt", AWS_IOT_ENDPOINT);
}

void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* incomeingMessage = doc["incomeingMessage"];
  Serial.println(incomeingMessage);
}

void connectAWS()
{

  // wifi connectivity is acheived with the following 14 lines
  //wifi station mode (standard wifi connection mode)
  WiFi.mode(WIFI_STA);
  //pass wifi credentails for connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("CONNECTING TO Wi-Fi");

  // while wifi isnt connected print message and wait for connection before moving on 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  getSPIFFS();
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("CONNECTING TO AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT TIMEOUT!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT TOPIC SUBSCRIBED");
}

void send_image(uint8_t *im, size_t size) 
{
  client.beginPublish(AWS_IOT_PUBLISH_IMAGES_TOPIC, size, false);
  client.write(im, size);
  client.endPublish();
  Serial.println("IMAGE PUBLISHED");
  delay(10000);
} 

void send_params()
{
  StaticJsonDocument<200> doc; // TODO calculate size needed here (in place of 200)
  // doc["height"] = H;
  // doc["width"] = W;
  // doc["blocksize"] = BLOCK_SIZE;
  doc["device_id"] = "id";
  char jsonBuffer[512]; // TODO calculate size needed here (in place of 500)
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_PARAMS_TOPIC, jsonBuffer);
  Serial.println("PARAMS PUBLISHED");
}
