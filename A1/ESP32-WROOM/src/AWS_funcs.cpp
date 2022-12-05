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
#include "connect.h"
#include "MySPIFFS.h"
#include "daliSend.h"
#include "ESPtime.h"

// remove all USE_WIFI functions

//#define USE_WIFI

//topics 
const char* AWS_IOT_PUBLISH_IMAGES_TOPIC = "TestTX";
const char* AWS_IOT_PUBLISH_PARAMS_TOPIC = "TestRX";
const char* AWS_IOT_PUBLISH_LOGFILES_TOPIC = "logTopic";

const char* AWS_IOT_SUBSCRIBE_TOPIC = "TestTX";
const char* AWS_IOT_SUBSCRIBE_TIME_TOPIC = "Time";


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
  PubSubClient client(AWS_IOT_ENDPOINT.c_str(), 8883, messageHandler, net);
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
    PubSubClient client(AWS_IOT_ENDPOINT.c_str(), 8883, messageHandler, LTE_secureClient);
#endif

void checkMQTT(void){
  client.loop();
}

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
        break;
        // OTA update instrucion
      case 2:
        daliSend(doc["LightLevel"]);
        break;
      case 3:
        // With this line here, it wont let me assign a struct from json to variable 'time' 
        struct tm timeinfo;
        strptime(doc["time"], "%FT%TZ", &timeinfo);
        ESPtime.setTimeStruct(timeinfo);
        Serial.print("Time has been set to: ");
        //should  be8:34:20 1/4/2021
        Serial.println(ESPtime.getDateTime());
        break;
      // case 4: 
      //   daliTestSwitch(doc["lightTime"]);
      break;
    }
}

void send_image(uint8_t *im, size_t size) 
{
  client.beginPublish(AWS_IOT_PUBLISH_IMAGES_TOPIC, size, false);
  client.write(im, size);
  client.endPublish();
  Serial.println("IMAGE PUBLISHED");
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
  client.publish(AWS_IOT_PUBLISH_PARAMS_TOPIC, jsonBuffer);
  Serial.println("PARAMS PUBLISHED");
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

  Serial.println("CONNECTING TO Wi-Fi");

  // while wifi isnt connected print message and wait for connection before moving on 
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
  }

#ifdef SPIFFSdef
  getSPIFFS();
#endif

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA.c_str());
  Serial.println("CA set");
  net.setCertificate(AWS_CERT_CRT.c_str());
  Serial.println("CRT set");
  net.setPrivateKey(AWS_CERT_PRIVATE.c_str());
  Serial.println("Priv key set");

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  // Create a message handler

  Serial.println("CONNECTING TO AWS IOT");

  while (!client.connect(THINGNAME.c_str()))
  {
    Serial.print(".");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT TIMEOUT!");
    return false;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT TOPIC SUBSCRIBED");
  return true;
}

#else

// PWR_PIN : This Pin is the PWR-KEY of the SIM7600
// The time of active low level impulse of PWRKEY pin to power on module 500 ms
void modemPowerOn()
{
  Serial.println("powering ON modem");
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  digitalWrite(PWR_PIN, HIGH);
}

bool LTE_connect()
{
    Serial.println("Initializing modem ...");

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
            Serial.println("waiting modem to start up");
        }

        if(i < 100)
        {
            Serial.print("modem is up in ");
            Serial.print(i*500);
            Serial.println("ms");
            break;
        }
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    Serial.println("Setting modem Baud rate");
    modem.setBaud(MODEM_UART_BAUD);
    String modemInfo = modem.getModemInfo();
    Serial.print("Modem info: ");
    Serial.println(modemInfo);

    Serial.print("SIM status: ");
    Serial.println(modem.getSimStatus());

    Serial.println("Connecting to network");
    if (!modem.waitForNetwork()) {
        Serial.println("network failed to connect");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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

    if((LTE_ready == false) || (client.connected() == false) )
    {
        while(!LTE_ready)
          LTE_ready = LTE_connect();

        LTE_secureClient.setCACert(AWS_CERT_CA.c_str());
        LTE_secureClient.setCertificate(AWS_CERT_CRT.c_str());
        LTE_secureClient.setPrivateKey(AWS_CERT_PRIVATE.c_str());
        Serial.print("Certs set");
        for(int i=0; i<10; i++)
        {
             if(client.connect(THINGNAME.c_str()))
            {
                client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
                client.subscribe(AWS_IOT_SUBSCRIBE_TIME_TOPIC);
                Serial.println("CONNECTED AWS IOT");
                return true;
            }
            else
            {
                Serial.print(".");
                vTaskDelay(100 / portTICK_PERIOD_MS);
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
bool LTE_publish(const char *message, const char* topic)
{
    for(int i=0; i<3; i++)
    {
        if(client.publish(topic, message))
        {
            Serial.println("Publish OK");
            return true;
        }

        Serial.println("Publish failed");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    return false;
}

#endif