# Overview 

This is Software version 0.1

Features:
- log file handling
- SPI comms
- Image handling
- AWS light level commands 
- DALI sequencing 
- Time keeping 
- SPIFFS 
- AWS communication

Unit doesn't not turn the lamp on, this for data collection and system testing purposes.
It simulates the light turning on but not command is sent to the DALI.

## Serial Comms Wiring

| LILYGO pin | AI thinker pin | Purpose |
| ---------- | -------------- | ------- |
| GPIO15/13  | GPIO15         | CS      |
| GPIO18     | GPIO14         | SCLK    |
| GPIO23     | GPIO13         | COPI    |
| GPIO19     | GPIO12         | CIPO    |
