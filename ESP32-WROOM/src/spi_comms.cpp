/*
                                                                  *****FILE HEADER*****
File Name - serial_comms.cpp   
                                  
Author/s - Michael Gamston - Joe Button

Description - This file contains functionality for the SPI communication protocol using two peripherals and one controller. 

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - Single bus API is throwing up problems. Currently being looked into by Zak Taylor.

Libraries - Uses esp32 dma SPI library: https://github.com/hideakitai/ESP32DMASPI

Repo - michaelgamston/MVP
Branch - main

*/
#include <Arduino.h>
#include <SPI.h>
#include "spi_comms.h"

SPIClass* hspi = NULL;

uint8_t spi_buf[SPI_BUFFER_SIZE];


void set_buf(void){
  memset(spi_buf, 0, SPI_BUFFER_SIZE);
}
//
// Initialise the SPI controller.
//
// High speed SPI used.
// Default pins for SCLK, MOSI and MISO.
// HSPI_SS (chip select) pin allocated to an unused GPIO pin, since
// the controller has two peripherals.
//
// Return Value  None.
//

void init_spi(void)
{
 Serial.println("Initialising SPI Controller.\n");

// Both SPI bus chip select pins set HIGH - neither peripheral is selected.

 pinMode(CS_1_GPIO_PIN, OUTPUT);
 pinMode(CS_2_GPIO_PIN, OUTPUT);
 Serial.println("PinMode, OUTPUT");
 digitalWrite(CS_1_GPIO_PIN, HIGH);
 digitalWrite(CS_2_GPIO_PIN, HIGH); 
 Serial.println("CS, HIGH");
// Initialise comms buffer.

 set_buf();
 Serial.println("memset");

 hspi = new SPIClass(HSPI);
 hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
 Serial.println("SPI intialized");
}

//
// Enable the given SPI peripheral by setting it's chip select to LOW.
//
// Parameter
// ---------
// peripheral_number   Number of peripheral [1 | 2]
//
// Return Value  None.
//

void enable_spi_peripheral(uint8_t peripheral_number)
{
 if (peripheral_number == 1)
 {
  digitalWrite(CS_1_GPIO_PIN, LOW);
 }
 else if (peripheral_number == 2)
 {
  digitalWrite(CS_2_GPIO_PIN, LOW);
 }

 delay(1);  // Alow bus to 'settle'.
}

//
// Disable the given SPI peripheral by setting it's chip select to HIGH.
//
// Parameter
// ---------
// peripheral_number   Number of peripheral [1 | 2]
//
// Return Value  None.
//

void disable_spi_peripheral(uint8_t peripheral_number)
{
 if (peripheral_number == 1)
 {
  digitalWrite(CS_1_GPIO_PIN, HIGH);
 }
 else if (peripheral_number == 2)
 {
  digitalWrite(CS_2_GPIO_PIN, HIGH);
 }
}

//
// Send data over SPI bus to given peripheral.
//
// Data clock: 5 MHz.
// Bit order : MSB first
// SPI mode  : 0
//
// The data stored in spi_buf is replaced by the peripheral's data.
//
// Parameters
// ----------
// peripheral_number   Number of peripheral [1 | 2]
// data_len            Number of bytes to transmit
//
// Return Value  None.
//

void spi_txn(uint8_t peripheral_number, uint16_t data_len)
{
 if (data_len <= SPI_BUFFER_SIZE)
 {
    Serial.print("Transaction between Controller and Peripheral ");
    Serial.println(peripheral_number);
    hspi->beginTransaction(SPISettings(SPI_BUS_SPEED, MSBFIRST, SPI_MODE0));
    enable_spi_peripheral(peripheral_number);
    while (spi_buf[0] != 5){
    hspi->transfer(spi_buf, data_len);
    }
    disable_spi_peripheral(peripheral_number);
    hspi->endTransaction();
    spi_buf[7501] = DEVICE_NAME;
    spi_buf[7502] = peripheral_number;
    delay(1000);
 }
}