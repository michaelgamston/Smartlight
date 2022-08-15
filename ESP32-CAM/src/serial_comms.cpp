#include "serial_comms.h"
#include <ESP32DMASPISlave.h>
#include <Arduino.h>
#include "camera_funcs.h"

ESP32DMASPI::Slave slave;

const uint32_t BUFFER_SIZE = 8192;
const uint32_t IMAGE_SIZE = H * W;
uint8_t* spi_slave_tx_buf;
uint8_t* spi_slave_rx_buf;
// Pins
const uint8_t ERROR_GPIO_PIN = 2;
const uint8_t CS_GPIO_PIN = 15;
bool is_setup = false;

void setup_spi()
{
  // to use DMA buffer, use these methods to allocate buffer
  spi_slave_tx_buf = slave.allocDMABuffer(BUFFER_SIZE);
  spi_slave_rx_buf = slave.allocDMABuffer(BUFFER_SIZE);
  // pins 
  pinMode(CS_GPIO_PIN, INPUT);
  // set buffer data...
  memset(spi_slave_tx_buf, 0, BUFFER_SIZE);
  memset(spi_slave_rx_buf, 0, BUFFER_SIZE);
  // slave device configuration
  slave.setDataMode(SPI_MODE0);
  slave.setMaxTransferSize(BUFFER_SIZE);
  // begin() after setting
  slave.begin();  // HSPI = CS: 15, CLK: 14, MOSI: 13, MISO: 12 -> default
                  // VSPI (CS:  5, CLK: 18, MOSI: 23, MISO: 19)
  is_setup = true;
}

void copy_to_buffer()
{
  if (!is_setup)
  {
    setup_spi();
  }
  for (int i = 0; i < IMAGE_SIZE; i++)
  {
    spi_slave_tx_buf[i] = (uint8_t) current_frame[i];
  }
}

void send_image()
{
  copy_to_buffer();
  // if there is no transaction in queue, add transaction
  if (slave.remained() == 0) 
  {
    // copy image into tx buffer
    for (int i = 0; i < IMAGE_SIZE; i++) 
    {
      spi_slave_tx_buf[i] = (uint8_t) current_frame[i];
      // spi_slave_tx_buf[i] = 12;
      // Serial.println(spi_slave_tx_buf[i]);
    }

    // this adds buffers to queue 
    slave.queue(spi_slave_rx_buf, spi_slave_tx_buf, BUFFER_SIZE);
  }

  // if transaction has completed from master,
  // available() returns size of results of transaction,
  // and buffer is automatically updated
  // truthiness of slave.available is false if there is a transaction in progress, otherwise true
  while (slave.available()) 
  {
    // do something with received data: spi_slave_rx_buf
    slave.pop();
    Serial.println("transferring");
  }
}
