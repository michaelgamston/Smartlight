#include "logControl.h"

void updateLogFile(const char* DateTimeLevel = "YYYYMMDD_HHMMSS_%\n") {
    appendFile(SPIFFS, "/LogFile.txt", DateTimeLevel);
}

void sendToAWS(const char* path = "/LogFile.txt") {
    String contents = fileToString(SPIFFS, path);
    LTE_publish(contents.c_str());
}
