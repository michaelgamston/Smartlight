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

#pragma once
#include <cstdint>

// Buffer size - need to change this to suit image size (currently only need H * W * 1BPP = 4072 bytes)
extern const uint32_t BUFFER_SIZE; // must be multiple of 4

// tx and rx buffers
extern uint8_t *spi_master_tx_buf;
extern uint8_t *spi_master_rx_buf;

/* 
     
    Input: N/A
    Output: Set's SPI buffers to 0
    Return: N/A  
    Action: 
        - Uses memset to set all address in the buffer to 0
    Comments: N/A

*/
void set_buffer();

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
    Outout: Sends and recieves data to/from peripherals 
    Return: N/A  
    Action: 
        - Swaps buffer data of controller and peripheral 
    Comments: image should be a 1d uint8_t array of length H * W
*/
void receive_image();