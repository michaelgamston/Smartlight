#include <Arduino.h>
#include "queue.h"

static QueueHandle_t printQueue; 
static int buf_len = 50;
//SemaphoreHandle_t printMutex;


void initQueue(void){
    printQueue = xQueueCreate(50, buf_len); //length of 100 elements, each element us 50 bytes big (so a print statment can have only 50 charaters)
    //printMutex = xSemaphoreCreateBinary();
}

void queueCallback(void *parameters){
    char print_buf[buf_len];
    while(1){
        if (xQueueReceive(printQueue, (void*)&print_buf, 0) == pdTRUE){    
            //xSemaphoreTake(printMutex, 10);
            Serial.print(print_buf);
            Serial.println();
            //xSemaphoreGive(printMutex);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void queueMessage(String message){
    char msg_buf[buf_len];
    strcpy(msg_buf, message.c_str());
    xQueueSend(printQueue, (void*)&msg_buf, 10);
}

