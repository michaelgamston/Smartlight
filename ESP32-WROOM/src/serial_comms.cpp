#include "serial_comms.h"
#include <ESP32DMASPIMaster.h>
#include <Arduino.h>

ESP32DMASPI::Master master;

// SPI pin defs 
const uint8_t CS = 12; // Chip select 
const uint8_t SCLK = 13; // Serial clock
const uint8_t COPI = 22; // Controller out peripheral in 
const uint8_t CIPO = 37; // Controller in peripheral out 

// Buffer size - need to change this to suit image size (currently only need H * W * 1BPP)
const uint32_t BUFFER_SIZE = 8192; // must be multiple of 4

// tx and rx buffers
uint8_t *spi_master_tx_buf;
uint8_t *spi_master_rx_buf;

void set_buffer() 
{
  memset(spi_master_tx_buf, 0, BUFFER_SIZE);
  memset(spi_master_rx_buf, 0, BUFFER_SIZE);
}

void setup_spi() 
{
  spi_master_tx_buf = master.allocDMABuffer(BUFFER_SIZE);
  spi_master_rx_buf = master.allocDMABuffer(BUFFER_SIZE);
  set_buffer();
  master.setDataMode(SPI_MODE0);           // default: SPI_MODE0
  master.setFrequency(5000000);            // default: 8MHz (too fast for bread board...)
  master.setMaxTransferSize(BUFFER_SIZE);  // default: 4092 bytes
  // pins 
  pinMode(CS, OUTPUT);
  // begin() after setting
  master.begin(HSPI, SCLK, CIPO, COPI, CS);  // default: HSPI (CS: 15, CLK: 14, MOSI: 13, MISO: 12)
  Serial.println("setup spi");
}

void receive_image()
{
  // CS low means in transfer mode
  digitalWrite(CS, LOW);
  // start and wait to complete transaction
  master.transfer(spi_master_tx_buf, spi_master_rx_buf, BUFFER_SIZE);
  //reset CS to HIGH to complete transaction
  digitalWrite(CS, HIGH);
}
