#include "AWS_funcs.h"
#include <ArduinoJson.h>
#include "WiFi.h"
#include "secrets.h"
#include "camera_funcs.h"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

const char *AWS_IOT_PUBLISH_IMAGES_TOPIC = "FiPy/images";
const char *AWS_IOT_PUBLISH_PARAMS_TOPIC = "FiPy/params";
const char *AWS_IOT_SUBSCRIBE_TOPIC = "FyPi/sub";


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
  doc["height"] = H;
  doc["width"] = W;
  doc["blocksize"] = BLOCK_SIZE;
  doc["device_id"] = "id";
  char jsonBuffer[512]; // TODO calculate size needed here (in place of 500)
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_PARAMS_TOPIC, jsonBuffer);
  Serial.println("PARAMS PUBLISHED");
}
