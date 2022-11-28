/*
                                                                  *****FILE HEADER*****
File Name - serial_comms.h

Author/s - Joe Button

Description - Functions and constants for SPI serial communication between FiPy and esp32-cam. Header file for serial_comms.cpp

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for updated comments

Libraries - Uses esp32 dma SPI library: https://github.com/hideakitai/ESP32DMASPI

Repo - michaelgamston/MVP
Branch - main

*/
#pragma once
#include <cstdint>

// Buffer size - need to change this to suit image size (currently only need H * W * 1BPP bytes)
extern const uint32_t BUFFER_SIZE; // must be multiple of 4

// tx and rx buffers
extern uint8_t *spi_slave_tx_buf;
extern uint8_t *spi_slave_rx_buf;

/*
    Input: N/A
    Outout: Allocate DMA buffers for rx and tx SPI buffers 
    Return: N/A  
    Action: 
        - Sets SPI data mode, frequency and max transfer.
        - Allocates buffer space.
        - initialize SPI bus 
    Comments: N/A

*/
void setup_spi();

/*
    Input: N/A
    Outout: N/A
    Return: N/A  
    Action: 
        - add activation byte to the second element of the buffer
    Comments: N/A

*/
void addActivationByte();

/*
    Input: N/A
    Outout: N/A
    Return: N/A  
    Action: 
        - resets activation byte to 0 
    Comments: N/A

*/
void deactivateByte();

/*
    Input: N/A
    Outout: Calls setup_spi  
    Return: N/A  
    Action: 
        - copied hex array from image to tx buffer
    Comments: N/A

*/
void copy_to_buffer();

/*
    Input: N/A
    Outout: calls copy_to_buffer
    Return: N/A  
    Action: 
        - queues filled buffers ready for transaction 
        - sends buffer when when controller requests 
    Comments: N/A

*/
bool send_image();


