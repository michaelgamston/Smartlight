#pragma once
#include <cstdint>

// Buffer size - need to change this to suit image size (currently only need H * W * 1BPP bytes)
extern const uint32_t BUFFER_SIZE; // must be multiple of 4

// tx and rx buffers
extern uint8_t *spi_slave_tx_buf;
extern uint8_t *spi_slave_rx_buf;
void setup_spi();
void copy_to_buffer();
void send_image();