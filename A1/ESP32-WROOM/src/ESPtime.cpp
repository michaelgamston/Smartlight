#include "ESPtime.h"

ESP32Time ESPtime(0);

bool timeSetFlag = false;

void setTime(StaticJsonDocument<200> doc){

    struct tm timeinfo;
    strptime(doc["time"], "%FT%TZ", &timeinfo);
    ESPtime.setTimeStruct(timeinfo);
    Serial.print("Time has been set to: ");
    Serial.println(ESPtime.getDateTime());
    timeSetFlag = true;
    LTE_publish("Time has been set", AWS_IOT_PUBLISH_SERIAL_TOPIC);
    
}