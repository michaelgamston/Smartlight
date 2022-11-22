#include "logControl.h"
#include "ESPtime.h"

static char dateTimeLevelLog[35];

void updateLogFile(int lightLevel) {

    sprintf(dateTimeLevelLog, "Level changed to %i : ", lightLevel);
    const char* suffix = ESPtime.getTime().c_str();
    strcat(dateTimeLevelLog, suffix);

    Serial.print("log file updated with '");
    Serial.print(dateTimeLevelLog);
    Serial.println("'");

    appendFile(SPIFFS, "/LogFile.txt", dateTimeLevelLog);
}

//create a thread for this function to be called every 30 mins
void logFileToAWS(char* path = "/LogFile.txt") {
    Serial.println("log file sent to aws");
    String contents = fileToString(SPIFFS, path);
    LTE_publish(contents.c_str());
    deleteFile(SPIFFS, path);
}
