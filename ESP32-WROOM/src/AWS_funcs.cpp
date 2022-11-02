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
#include "OTA.h"
#include "MySPIFFS.h"
//#define USE_WIFI
//topics 
const char *AWS_IOT_PUBLISH_IMAGES_TOPIC = "FiPy/images";
const char *AWS_IOT_PUBLISH_PARAMS_TOPIC = "FiPy/params";
const char *AWS_IOT_SUBSCRIBE_TOPIC = "OTA/updates";

#ifdef SPIFFSdef
  //SPIFFS credentials
  String THINGNAME;
  String AWS_CERT_CA;
  String AWS_CERT_CRT;
  String AWS_CERT_PRIVATE;
  String AWS_IOT_ENDPOINT;

  void getSPIFFS(){
    THINGNAME = fileToString(SPIFFS, "/ThingName.txt");
    AWS_IOT_ENDPOINT = fileToString(SPIFFS, "/Endpoint.txt");
    AWS_CERT_CA= fileToString(SPIFFS, "/CAcert.txt");
    AWS_CERT_CRT= fileToString(SPIFFS, "/CRTcert.txt");
    AWS_CERT_PRIVATE= fileToString(SPIFFS, "/Privkey.txt");  
  }
#endif

#ifdef USE_WIFI
  WiFiClientSecure net = WiFiClientSecure();
  PubSubClient *client = new PubSubClient(AWS_IOT_ENDPOINT, 8883, messageHandler, net);
#else
  #include <TinyGsmClient.h>
  #include <SSLClient.h>
  #include <ArduinoHttpClient.h>

  #define MODEM_UART_BAUD 19200
  #define AWS_IOT_PUBLISH_TOPIC   "test"

  const char server[]   = "vsh.pp.ua";
  const char resource[] = "/TinyGSM/logo.txt";
  const int  port       = 80;

  TinyGsm modem(Serial2);
  TinyGsmClient LTE_client(modem);
  SSLClient LTE_secureClient(&LTE_client);
  PubSubClient *client = new PubSubClient(AWS_IOT_ENDPOINT, 8883, messageHandler, LTE_secureClient);
  HttpClient http(LTE_client, server, port);

  

#endif


void messageHandler(char* topic, byte* payload, unsigned int length)
{
  const char* url;
  Serial.print("Incoming from topic: ");
  Serial.println(topic);
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  int instruction = doc["instruction"];


  switch(instruction){
      // incoming message 
    case 1: 
      Serial.printf(doc["message"]);
      Serial.println();
      break;
      // OTA update instrucion
    case 2:
      Serial.println("OTA update incoming");    
      runOTA(doc["url"]);
      break;
  }
}

void send_image(uint8_t *im, size_t size) 
{
  client->beginPublish(AWS_IOT_PUBLISH_IMAGES_TOPIC, size, false);
  client->write(im, size);
  client->endPublish();
  Serial.println("IMAGE PUBLISHED");
  delay(1000);
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
  client->publish(AWS_IOT_PUBLISH_PARAMS_TOPIC, jsonBuffer);
  Serial.println("PARAMS PUBLISHED");
}

#ifdef USE_WIFI

bool connectAWS()
{
  delay(5000);
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

#ifdef SPIFFSdef
  getSPIFFS();
#endif

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  Serial.println("CA set");
  net.setCertificate(AWS_CERT_CRT);
  Serial.println("CRT set");
  net.setPrivateKey(AWS_CERT_PRIVATE);
  Serial.println("Priv key set");

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  // Create a message handler

  Serial.println("CONNECTING TO AWS IOT");

  while (!client->connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client->connected())
  {
    Serial.println("AWS IoT TIMEOUT!");
    return false;
  }

  // Subscribe to a topic
  client->subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT TOPIC SUBSCRIBED");
  return true;
}

#else

bool http_test(){

  Serial.print(F("Performing HTTP GET request... "));
  int err = http.get(resource);
  if (err != 0) {
    Serial.println(F("failed to connect"));
    delay(10000);
    return false;
  }

  int status = http.responseStatusCode();
  Serial.print(F("Response status code: "));
  Serial.println(status);
  if (!status) {
    delay(10000);
    return false;
  }

  Serial.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName  = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    Serial.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    Serial.print(F("Content length is: "));
    Serial.println(length);
  }
  if (http.isResponseChunked()) {
    Serial.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  Serial.println(F("Response:"));
  Serial.println(body);

  Serial.print(F("Body length is: "));
  Serial.println(body.length());

  // Shutdown

  http.stop();

  return true;
}


bool LTE_connect()
{
    Serial.println("Setting modem Baud rate");
    modem.setBaud(MODEM_UART_BAUD);
    Serial.println("Initializing modem ...");
    modem.restart();

    String modemInfo = modem.getModemInfo();
    Serial.print("Modem info: ");
    Serial.println(modemInfo);

    Serial.print("SIM status: ");
    Serial.println(modem.getSimStatus());

    Serial.println("Connecting to network");
    if (!modem.waitForNetwork()) {
        Serial.println("network failed to connect");
        delay(1000);
        return false;
    }
    else if (modem.isNetworkConnected()) {
        Serial.println("network connected");
    }

    Serial.println("Connecting GPRS..");
    modem.gprsConnect("everywhere", "eesecure", "secure");
    if (modem.isGprsConnected()) { 
        Serial.println("GPRS connected");
    }
    else
        Serial.println("GPRS not connected");

    Serial.println("Getting signal Strength");
    int signalQuality = modem.getSignalQuality();
    Serial.print("Signal quality: ");
    Serial.println(signalQuality);

    //if (http_test()) Serial.println("http successful");

    return true;
}

bool connectAWS()
{
    static bool LTE_ready = false;

    if((LTE_ready == false) || (client->connected() == false) )
    {
        while(!LTE_ready)
          LTE_ready = LTE_connect();

        LTE_secureClient.setCACert(AWS_CERT_CA);
        LTE_secureClient.setCertificate(AWS_CERT_CRT);
        LTE_secureClient.setPrivateKey(AWS_CERT_PRIVATE);
        Serial.print("Certs set");
        for(int i=0; i<10; i++)
        {
            if(client->connect(THINGNAME))
            {
                client->subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
                Serial.println("CONNECTED AWS IOT");
                return true;
            }
            else
            {
                Serial.print(".");
                delay(100);
            }
        }
    }
    else
    {
        Serial.println("Already connected to AWS IOT");
        return true;
    }

    return false;
}

// for simple test
bool LTE_publish(const char *message)
{
    for(int i=0; i<3; i++)
    {
        if(client->publish(AWS_IOT_PUBLISH_TOPIC, message))
        {
            Serial.println("Publish OK");
            return true;
        }

        Serial.println("Publish failed");
        delay(100);
    }

    return false;
}

#endif
