#pragma once

#include <ESP32Time.h>
#include <ArduinoJson.h>

extern ESP32Time ESPtime;

extern bool timeSetFlag; 

void setTime(StaticJsonDocument<200> doc);