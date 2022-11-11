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
#include "queue.h"
//#define USE_WIFI
//topics 
const char *AWS_IOT_PUBLISH_IMAGES_TOPIC = "FiPy/images";
const char *AWS_IOT_PUBLISH_PARAMS_TOPIC = "FiPy/params";
const char *AWS_IOT_SUBSCRIBE_TOPIC = "OTA/updates";

static char buffer[50];

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

    #define MODEM_UART_BAUD     115200
    #define PWR_PIN             4
    #define POWER_PIN           25
    #define IND_PIN             36

    #define AWS_IOT_PUBLISH_TOPIC   "test"

    TinyGsm modem(Serial1);
    TinyGsmClient LTE_client(modem);
    SSLClient LTE_secureClient(&LTE_client);
    PubSubClient *client = new PubSubClient(AWS_IOT_ENDPOINT.c_str(), 8883, messageHandler, LTE_secureClient);
#endif

void checkMQTT(void){
  client->loop();
}

void messageHandler(char* topic, byte* payload, unsigned int length)
{
  const char* url;
  queueMessage("Incoming from topic: ");
  queueMessage(topic);
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  int instruction = doc["instruction"];


  switch(instruction){
      // incoming message 
    case 1: 
      queueMessage(doc["message"]);
      break;
      // OTA update instrucion
    case 2:
      queueMessage("OTA update incoming");    
      //runOTA(doc["url"]);
      break;
  }
}

void send_image(uint8_t *im, size_t size) 
{
  client->beginPublish(AWS_IOT_PUBLISH_IMAGES_TOPIC, size, false);
  client->write(im, size);
  client->endPublish();
  queueMessage("IMAGE PUBLISHED");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
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
  queueMessage("PARAMS PUBLISHED");
}

#ifdef USE_WIFI

bool connectAWS()
{
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  // wifi connectivity is acheived with the following 14 lines
  //wifi station mode (standard wifi connection mode)
  WiFi.mode(WIFI_STA);
  //pass wifi credentails for connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  queueMessage("CONNECTING TO Wi-Fi");

  // while wifi isnt connected print message and wait for connection before moving on 
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    queueMessage(".");
  }

#ifdef SPIFFSdef
  getSPIFFS();
#endif

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  queueMessage("CA set");
  net.setCertificate(AWS_CERT_CRT);
  queueMessage("CRT set");
  net.setPrivateKey(AWS_CERT_PRIVATE);
  queueMessage("Priv key set");

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  // Create a message handler

  queueMessage("CONNECTING TO AWS IOT");

  while (!client->connect(THINGNAME))
  {
    queueMessage(".");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  if (!client->connected())
  {
    queueMessage("AWS IoT TIMEOUT!");
    return false;
  }

  // Subscribe to a topic
  client->subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  queueMessage("AWS IoT TOPIC SUBSCRIBED");
  return true;
}

#else

// PWR_PIN : This Pin is the PWR-KEY of the SIM7600
// The time of active low level impulse of PWRKEY pin to power on module 500 ms
void modemPowerOn()
{
  queueMessage("powering ON modem");
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  digitalWrite(PWR_PIN, HIGH);
}

bool LTE_connect()
{
    queueMessage("Initializing modem ...");

    // POWER_PIN : This pin controls the power supply of the SIM7600
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // IND_PIN: It is connected to the SIM7600 status Pin,
    // through which you know whether the module starts normally.
    pinMode(IND_PIN, INPUT);

    for(int retry=0; retry<2; retry++)
    {
        modemPowerOn();
        int i = 0;
        for(i=0; i<100; i++)
        {
            if(digitalRead(IND_PIN) == HIGH)
            {
              break;
            }

            vTaskDelay(500 / portTICK_PERIOD_MS);
            queueMessage("waiting modem to start up");
        }

        if(i < 100)
        {
            queueMessage("modem is up");
            break;
        }
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    queueMessage("Setting modem Baud rate");
    modem.setBaud(MODEM_UART_BAUD);
    sprintf(buffer, "Modem info: %s", modem.getModemInfo());
    queueMessage(buffer);

    /*COULDNT CONVERT RETURN TYPE OF .getSimStatus FOR QUEUE FUNCTION */
    // queueMessage("SIM status: ");
    // String modemSimStatus = modem.getSimStatus()
    // queueMessage((modem.getSimStatus()).c_str());

    queueMessage("Connecting to network");
    if (!modem.waitForNetwork()) {
        queueMessage("network failed to connect");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return false;
    }
    else if (modem.isNetworkConnected()) {
        queueMessage("network connected");
    }

    queueMessage("Connecting GPRS..");
    modem.gprsConnect("everywhere", "eesecure", "secure");
    if (modem.isGprsConnected()) { 
        queueMessage("GPRS connected");
    }
    else
        queueMessage("GPRS not connected");

    queueMessage("Getting signal Strength");
    char signalQuality = modem.getSignalQuality();
    /*couldnt use queueMessage here as I can't transfer int */

    sprintf(buffer, "Signal quality: %c", signalQuality);
    queueMessage(buffer);
    //if (http_test()) queueMessage("http successful");

    return true;
}

bool connectAWS()
{
    static bool LTE_ready = false;

    if((LTE_ready == false) || (client->connected() == false) )
    {
        while(!LTE_ready)
          LTE_ready = LTE_connect();

        LTE_secureClient.setCACert(AWS_CERT_CA.c_str());
        LTE_secureClient.setCertificate(AWS_CERT_CRT.c_str());
        LTE_secureClient.setPrivateKey(AWS_CERT_PRIVATE.c_str());
        queueMessage("Certs set");
        for(int i=0; i<10; i++)
        {
             if(client->connect(THINGNAME.c_str()))
            {
                client->subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
                queueMessage("CONNECTED AWS IOT");
                return true;
            }
            else
            {
                queueMessage(".");
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        }
    }
    else
    {
        queueMessage("Already connected to AWS IOT");
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
            queueMessage("Publish OK");
            return true;
        }

        queueMessage("Publish failed");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    return false;
}

#endif