/*
                                                                  *****FILE HEADER*****
File Name - serial_comms.h

Author/s - Michael Gamston - Joe Button

Description - Functions and constants for SPI serial communication between FiPy and esp32-cam. Header file for serial_comms.cpp

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for updated comments

Libraries - Uses esp32 dma SPI library: https://github.com/hideakitai/ESP32DMASPI

Repo - michaelgamston/MVP
Branch - main

*/
#define DEVICE_NAME 1

#ifndef SPI_COMMS_H
#define SPI_COMMS_H

#define CS_1_GPIO_PIN 15   // SDI chip select GPIO pin for peripheral 1.
#define CS_2_GPIO_PIN 13   // SDI chip select GPIO pin for peripheral 2.

#define SPI_BUS_SPEED 5000000  // 5 MHz.
#define SPI_BUFFER_SIZE 8192   // 8192

#define HSPI_MISO   19
#define HSPI_MOSI   23
#define HSPI_SCLK   18
#define HSPI_SS     5    



// Function Prototypes.

// #ifdef __cplusplus             // Allow for C++ to handle C function prototypes.
// extern "C" {
// #endif

/* 
     
    Input: N/A
    Output:N/A
    Return: N/A  
    Action: 
        -Flashes the LED
    Comments: N/A

*/
void flashLED(int flashLength);

/* 
     
    Input: N/A
    Output:clears buffer to 0
    Return: N/A  
    Action: N/A
    Comments: N/A

*/
void set_buf(void);


/* 
     
    Input: N/A
    Output: Sets CS pins to High, clears buffer to 0, intializes SPI bus 
    Return: N/A  
    Action: 
        - Changings CS to output
        - Calls set_buf
    Comments: N/A

*/
extern void init_spi(void);

/* 
     
    Input: int peripheral number
    Output: Changes selected CS pin to LOW
    Return: N/A  
    Action: 
        - prepares selected peripheral for transaction 
    Comments: N/A

*/
void enable_spi_peripheral(uint8_t peripheral_number);

/* 
     
    Input: int peripheral number
    Output: Changes selected CS pin to HIGH
    Return: N/A  
    Action: 
        - ends connection to selected peripheral  
    Comments: N/A

*/
void disable_spi_peripheral(uint8_t peripheral_number);

/* 
     
    Input: int peripheral number, int data length 
    Output: tansacts with selected peripheral, adds device id to hex string
    Return: N/A  
    Action: 
        - starts transction
        - calls enable_spi_peripheral
        - transfers buffers 
        - calls disable_spi_peripheral 
        - ends transaction 
        - adds Device ID to string 
    Comments: N/A

*/
void spi_txn(uint8_t peripheral_number, uint16_t data_len);

// #ifdef __cplusplus
// }
// #endif

// Global Variables.

extern uint8_t spi_buf[SPI_BUFFER_SIZE];

#endif  // SPI_COMMS_H