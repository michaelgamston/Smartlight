#include <time.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "secrets.h"

const char* ssid = "AndroidAPBEF9";               
const char* password = "pqeh8163";   


const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

WiFiClientSecure net = WiFiClientSecure();
PubSubClient *client = new PubSubClient(AWS_IOT_ENDPOINT, 8883, net);

void send_time()
{
  StaticJsonDocument<200> doc; 
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  char timeDate[32];
  strftime(timeDate,32, "%FT%TZ", &timeinfo);
  doc["instruction"] = 3;
  doc["time"] = timeDate;
  char jsonBuffer[50];
  serializeJson(doc, jsonBuffer);
  client->publish(AWS_IOT_PUBLISH_TIME_TOPIC, jsonBuffer);
  Serial.println("PARAMS PUBLISHED");
}

void setup(void){


    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");

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
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    if (!client->connected())
    {
        Serial.println("AWS IoT TIMEOUT!");
    }

    Serial.println("AWS IoT TOPIC SUBSCRIBED");

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    delay(10000); //give time for lilygo to start up before sending time update

}

void loop(void){
    send_time();
    delay(5000);
}