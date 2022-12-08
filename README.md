# Comments 
Remember before launch to #define out any debuging. I.e serial stuff as this won't have function in final MVP product. 

# TODO 

- [ ] Save all certs in SPIFFS, get rid of connect.h 
- [ ] Get rid of USE_WIFI sections 
- [ ] Complete testing 


# Michaels last changes

Changed location of ESPtime defination to its own location 
Changed LTE_Publish to accept a topic parameter 

# Roberts last changes

Added some new mesh related functions 
Fixed mqtt related problems to sending large files (because of mqtt's packet size limits)

# Structure

Two platformio projects:

-ESP32-CAM 

-ESP32-WROVER LILYGO

Both projects are using the Ardunio Framework. These files will contain our functionality for the MVP. 

## Serial Comms Wiring

| LILYGO pin | AI thinker pin | Purpose |
| ---------- | -------------- | ------- |
| GPIO15/13  | GPIO15         | CS      |
| GPIO18     | GPIO14         | SCLK    |
| GPIO23     | GPIO13         | COPI    |
| GPIO19     | GPIO12         | CIPO    |

# ESP32-CAM

Functional MVP code. Sends image in byte array with check byte at position one using SPI. Ready for deloyment. 

# ESP32-WROVER LILYGO

Recieves image from CAM and then adds ID info to byte array before sending to AWS.
Has local wifi mesh capabilities, can send and receive messages to and from the mesh.
DALI functionality still in progress.
