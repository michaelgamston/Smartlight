/*
Functions and constants for SPI serial communication between FiPy and esp32-cam
Uses esp32 dma SPI library: https://github.com/hideakitai/ESP32DMASPI
*/
#pragma once
#include <cstdint>

// Buffer size - need to change this to suit image size (currently only need H * W * 1BPP = 4072 bytes)
extern const uint32_t BUFFER_SIZE; // must be multiple of 4

// tx and rx buffers
extern uint8_t *spi_master_tx_buf;
extern uint8_t *spi_master_rx_buf;

// initalise tx and rx buffers (dma alloc and set to 0s)
void set_buffer();

// set SPI parameters (transfer rate, mode number, buffer size)
void setup_spi();

// receive image from esp32-cam via SPI protocol into spi_master_rx_buf
// image should be a 1d uint8_t array of length H * W
void receive_image();