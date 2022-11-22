# Comments 
Remember before launch to #define out any debuging. I.e serial stuff as this won't have function in final MVP product. 

# Instructions for Robert;
(This can be done on or off the bread board, it might even be better to do it off for now. So long as you can power both units.)

Please flash the lilygo with the new ESP32-WROOM folder contents

Please flash TimeControl onto the the unit marked with TC. You will need to change the wifi crednetials to your phones hotspot as the wifi at 
the engine shed doesn't support encryted MQTT.

Once this is done please power both units up and let me know what the output of the lilygo is.

After the start up of the lilygo and timer of 10 seconds is over on the TC, we should see the time being updated and printed out every 5 seconds or so
through the lilgo's serial monitor.

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
