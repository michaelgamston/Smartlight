/*
                                                                  *****FILE HEADER*****
File Name - camera_funcs.cpp

Author/s - Joe Button

Description - Functions for capturing and handling images from camera

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for updated comments

Libraries - esp_camera: https://github.com/espressif/esp32-camera/blob/master/driver/include/esp_camera.h

Repo - michaelgamston/MVP
Branch - main

*/
#include "camera_funcs.h"
#include "camera_pins.h"
#include <Arduino.h>

uint8_t previous_frame[H*W];
uint16_t current_frame[H*W];

bool setup_camera(framesize_t frameSize) {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = frameSize;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  bool ok = esp_camera_init(&config) == ESP_OK;

  sensor_t *sensor = esp_camera_sensor_get();
  sensor->set_framesize(sensor, frameSize);

  return ok;
}

/**
 * Capture image and do down-sampling
 */

bool capture_still() {
  camera_fb_t *frame_buffer = esp_camera_fb_get(); // struct 

  if (!frame_buffer)
      return false;

  // set all 0s in current frame
  for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
          current_frame[y*W + x] = 0;


  // down-sample image in blocks 
  for (uint32_t i = 0; i < WIDTH * HEIGHT; i++) {
      // grab x and y coordinates 
      const uint16_t x = i % WIDTH;
      const uint16_t y = floor(i / WIDTH);
      const uint8_t block_x = floor(x / BLOCK_SIZE);
      const uint8_t block_y = floor(y / BLOCK_SIZE);
      const uint8_t pixel = frame_buffer->buf[i]; // this accesses the pixels in the raw image (an array of 320x240 uint8_t )
      const uint16_t current = current_frame[block_y*W + block_x];
      // average pixels in block (accumulate)
      current_frame[block_y*W +block_x] += pixel;
  }
  // average pixels in block (rescale)
  for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
          current_frame[y*W +x] /= BLOCK_SIZE * BLOCK_SIZE;


#if DEBUG
  Serial.println("Current frame:");
  print_frame(current_frame);
  Serial.println("---------------");
#endif

  esp_camera_fb_return(frame_buffer);

  return true;
}


/**
 * Compute the number of different blocks
 * If there are enough, then motion happened
 */
bool motion_detect() {
  uint16_t changes = 0; // number of changes
  const uint16_t blocks = (WIDTH * HEIGHT) / (BLOCK_SIZE * BLOCK_SIZE); // number of blocks
  // loop over pixels
  for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
          // compare pixel at x,y to previous frame pixel at x,y
          float current = current_frame[y*W +x];
          float prev = previous_frame[y*W +x];
          float delta = abs(current - prev) / prev; // magnitude of change wrt to previous value
          // print coords of pixel change if threshold met
          if (delta >= BLOCK_DIFF_THRESHOLD) {
#if DEBUG
              Serial.print("diff\t");
              Serial.print(y);
              Serial.print('\t');
              Serial.println(x);
#endif
              // keep track of number of pixels changed for each frame
              changes += 1;
          }
      }
  }
#if 0
  Serial.print("Changed ");
  Serial.print(changes);
  Serial.print(" out of ");
  Serial.println(blocks);
#endif 
  // compare average change to threshold 
  return (1.0 * changes / blocks) > IMAGE_DIFF_THRESHOLD;
}


/**
 * Copy current frame to previous
 */
void update_frame() {
  for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
          previous_frame[y*W +x] = current_frame[y*W +x];
      }
  }
}

/**
 * For serial debugging
 * @param frame
 */
void print_frame(uint16_t frame[H*W]) {
  for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
          Serial.print(frame[y*W +x]);
          Serial.print('\t');
      }

      Serial.println();
  }
}

size_t count_pixels(){
  camera_fb_t *frame_buffer = esp_camera_fb_get();
  size_t output = frame_buffer->height * frame_buffer->width;
  esp_camera_fb_return(frame_buffer);
  return output;
}

uint8_t *image_to_array(){
  camera_fb_t *frame_buffer = esp_camera_fb_get();
  uint8_t *output = frame_buffer->buf;
  esp_camera_fb_return(frame_buffer);
  return output;
}