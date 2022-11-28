# Comments 

Remember before launch to #define out any debuging. I.e serial stuff as this won't have function in final MVP product. 

# MQTT instructions 

Instructions are send it json format. Instruction 1 is used for sending a message, instruction 2 is used for sending a url for changing the lightLevel.
If you do not follow the syntax below the message handler will throw an error. 
{
    "instruction" : 1, 
    "message" : "your message"
}

{
    "instruction" : 2,
    "lightLevel" : 100
}

# Structure

Two platformio projects:

-ESP32-CAM 

-ESP32-WROOM

Both projects are using the Ardunio Framework. These files will contain our functionality for the MVP. 

## Serial Comms Wiring


| WROOM pin| AI thinker pin | Purpose |
| -------- | -------------- | ------- |
| GPIO2/5  | GPIO15         | CS      |
| GPIO14   | GPIO14         | SCLK    |
| GPIO13   | GPIO13         | COPI    |
| GPIO12   | GPIO12         | CIPO    |

# ESP32-CAM

Functional MVP code. Sends image in byte array with check byte at position one using SPI. Ready for deloyment. 

# ESP32-WROOM

Recieves image from CAM and then adds ID info to byte array before sending to AWS. DALI functionality still in progress.
