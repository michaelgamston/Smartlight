# Overview 

This is Software version 0.1

Features:
- log file handling
- SPI comms
- Image handling
- AWS light level commands 
- DALI contol using UART 
- Time keeping 
- SPIFFS 
- AWS communication

## Serial Comms Wiring

| LILYGO pin | AI thinker pin | Purpose |
| ---------- | -------------- | ------- |
| GPIO15/13  | GPIO15         | CS      |
| GPIO18     | GPIO14         | SCLK    |
| GPIO23     | GPIO13         | COPI    |
| GPIO19     | GPIO12         | CIPO    |
