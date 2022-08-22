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

#define CS_1_GPIO_PIN 2   // SDI chip select GPIO pin for peripheral 1.
#define CS_2_GPIO_PIN 4   // SDI chip select GPIO pin for peripheral 2.

#define SPI_BUS_SPEED 5000000  // 5 MHz.
#define SPI_BUFFER_SIZE 8192   // 8192

#define HSPI_MISO   12
#define HSPI_MOSI   13
#define HSPI_SCLK   14
#define HSPI_SS     18         // Pin Not Connected.

// Function Prototypes.

#ifdef __cplusplus             // Allow for C++ to handle C function prototypes.
extern "C" {
#endif

extern void init_spi(void);
extern void enable_spi_peripheral(uint8_t peripheral_number);
extern void disable_spi_peripheral(uint8_t peripheral_number);
extern void spi_txn(uint8_t peripheral_number, uint16_t data_len);

#ifdef __cplusplus
}
#endif

// Global Variables.

extern uint8_t spi_buf[SPI_BUFFER_SIZE];

#endif  // SPI_COMMS_H