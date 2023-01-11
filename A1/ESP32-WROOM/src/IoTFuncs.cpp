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
#include "IoTFuncs.h"
#include "connect.h"
#include "MySPIFFS.h"
#include "daliSend.h"
#include "ESPtime.h"

// remove all USE_WIFI functions

//#define USE_WIFI

//topics 
const char* AWS_IOT_PUBLISH_IMAGES_TOPIC = "Images";
const char* AWS_IOT_PUBLISH_LOGFILES_TOPIC = "logTopic";
const char* AWS_IOT_PUBLISH_SERIAL_TOPIC = "Serial";

const char* AWS_IOT_SUBSCRIBE_TOPIC = "Instruction";
const char* AWS_IOT_SUBSCRIBE_TIME_TOPIC = "Time";

static SemaphoreHandle_t mutex;
static bool callbackFlag = false; 

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


void checkMQTT(void* parameters){
  while(1){
    if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
      if(client.connected()) client.loop();
      else connectAWS();
      xSemaphoreGive(mutex);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
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
        daliSelectMode(CustomLevelActivation);
        daliSend(doc["lightLevel"]);
        break;

      case 3: 
        //not tested any of these 
        if (doc["mode"] == "high") daliSelectMode(High);
        else if (doc["mode"] == "low") daliSelectMode(Low);
        else if (doc["mode"] == "standby") daliSelectMode(Standby);
        else if (doc["mode"] == "timeActivation") daliSelectMode(TimeActivation);
        else if (doc["mode"] == "motionActivation") daliSelectMode(MotionActivation);
        break;

      case 4:
        // this was Set esp time. This is now obsolete 

      case 5: 
        //expect a map containing number of instructions, and then instructions with index. [brightness %, and ms delay]
        // {
        //   "instruction" : 5,
        //   "sequence": {
        //      "size": 2,
        //      "instructionList" : [100, 1000, 50, 1000]
        //   }
        // }
        daliSelectMode(SequenceActivation);
        daliSequenceInit(doc["sequence"]);
        break;
    }
}

void send_image(uint8_t *im, size_t size) 
{
  if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
    if(client.beginPublish(AWS_IOT_PUBLISH_IMAGES_TOPIC, size, false)){
      size_t chunks = size / 1024;
      for(size_t i = 0; i < chunks; i++)
        client.write(im + i*1024, 1024);
      if(size % 1024 !=0) client.write(im + chunks*1024, size % 1024);
      client.endPublish();
      Serial.println("Image published");
    }
    else Serial.println("Image failed");
    xSemaphoreGive(mutex);
  }
} 

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

void timeQuery() 
{
  struct tm timeinfo;
  modem.sendAT(GF("+CCLK?"));
  modem.waitResponse(GF(GSM_NL));
  String cclk = modem.stream.readStringUntil('\n');
  modem.waitResponse();
  String currentDt = cclk.substring(cclk.indexOf("\"")+1, cclk.lastIndexOf("\""));
  currentDt.trim();
  getLocalTime(&timeinfo);
  char s[51];
  strftime(s, 50, "%A, %B %d %Y %H:%M:%S", &timeinfo);

  Serial.print("\r\nCCLK: ");
  Serial.println(cclk);

  Serial.print("\r\ncurrentDt from SIM7600: ");
  Serial.println(currentDt);

  Serial.print("\r\ncurrentDt from ESP32: ");
  Serial.println(String(s));

  timeSetFlag == true; 
}

void timeSet()
{
  modem.sendAT(GF("+CNTPCID=1"));
  modem.waitResponse();

  modem.sendAT(GF("+CNTP=\"pool.ntp.org\",0"));
  modem.waitResponse();

  modem.sendAT(GF("+CNTP"));
  modem.waitResponse(3000, GF("+CNTP: 1" GSM_NL));

  modem.sendAT(GF("+CCLK?"));
  modem.waitResponse(GF(GSM_NL));
  String cclk = modem.stream.readStringUntil('\n');
  modem.waitResponse();
  Serial.print("\r\nCCLK: ");
  Serial.println(cclk);
  String currentDt = cclk.substring(cclk.indexOf("\"")+1, cclk.lastIndexOf("\""));
  currentDt.trim();
  Serial.print("\r\ncurrentDt: ");
  Serial.println(currentDt);

  int hh, mm, ss, yy, mon, day, tz;
  struct tm when = {0};
  time_t epoch = 0;

  sscanf(currentDt.c_str(), "%d/%d/%d,%d:%d:%d-%d", &yy, &mon, &day, &hh, &mm, &ss, &tz);
  when.tm_hour = hh;
  when.tm_min = mm;
  when.tm_sec = ss;
  when.tm_year = 2000 + yy - 1900;

  when.tm_mon = mon - 1;
  when.tm_mday = day;
  when.tm_isdst = -1;
  epoch = mktime(&when);
  struct timeval tv;
  tv.tv_sec = epoch+1;
  tv.tv_usec = 0;
  settimeofday(&tv, NULL);

  epoch = epoch + (60 * (tz/4) * 60);
  Serial.print("CURRENT EPOCH:");
  Serial.println(epoch);

  timeQuery();
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
        vTaskDelay(1000/ portTICK_PERIOD_MS);
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
    modem.gprsConnect(apn, gprsUser, gprsPass);
    if (modem.isGprsConnected()) { 
        Serial.println("GPRS connected");
    }
    else
        Serial.println("GPRS not connected");

    Serial.println("Getting signal Strength");
    int signalQuality = modem.getSignalQuality();
    Serial.print("Signal quality: ");
    Serial.println(signalQuality);

    timeSet();

    //if (http_test()) Serial.println("http successful");

    return true;
}

bool connectAWS()
{
    static bool LTE_ready = false;
    mutex = xSemaphoreCreateMutex();

    if((LTE_ready == false) || (client.connected() == false) )
    {
        while(!LTE_ready)
          LTE_ready = LTE_connect();

        LTE_secureClient.setCACert(AWS_CERT_CA.c_str());
        LTE_secureClient.setCertificate(AWS_CERT_CRT.c_str());
        LTE_secureClient.setPrivateKey(AWS_CERT_PRIVATE.c_str());
        Serial.println("Certs set");
        client.setBufferSize(8192);
        for(int i=0; i<10; i++)
        {
             if(client.connect(THINGNAME.c_str()))
            {
                client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
                client.subscribe(AWS_IOT_SUBSCRIBE_TIME_TOPIC);
                Serial.println("CONNECTED AWS IOT");

                if(!callbackFlag){
                    if (xTaskCreatePinnedToCore(
                      checkMQTT,
                      "MQTT callback",
                      8192,
                      NULL,
                      2,
                      NULL,
                      1
                    ) == pdTRUE) Serial.println("checkMQTT Task Created");
                    callbackFlag = true;
                  }
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

    if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
      if(client.publish(topic, message))
      {
          Serial.println("Publish OK");
          xSemaphoreGive(mutex);
          return true;
      }
      Serial.println("Publish failed");
      xSemaphoreGive(mutex);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  
    return false;
}

