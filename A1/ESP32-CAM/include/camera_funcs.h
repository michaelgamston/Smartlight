/*
                                                                  *****FILE HEADER*****
File Name - camera_funcs.h

Author/s - Joe Button

Description - Function definitions for capturing and handling images from camera

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for updated comments

Libraries - esp_camera: https://github.com/espressif/esp32-camera/blob/master/driver/include/esp_camera.h

Repo - michaelgamston/MVP
Branch - main

*/
#pragma once
#include "esp_camera.h"

// uint16_t prev_frame[H][W] = { 0 };
// uint16_t current_frame[H][W] = { 0 };

#define FRAME_SIZE FRAMESIZE_SVGA
#define WIDTH 800
#define HEIGHT 600
#define BLOCK_SIZE 8
#define W (WIDTH / BLOCK_SIZE)
#define H (HEIGHT / BLOCK_SIZE)
#define BLOCK_DIFF_THRESHOLD 0.1
#define IMAGE_DIFF_THRESHOLD 0.1
#define DEBUG 0

bool setup_camera(framesize_t);
bool capture_still();
bool motion_detect();
void update_frame();
void print_frame(uint16_t frame[H][W]);
size_t count_pixels();
uint8_t *image_to_array();

extern uint8_t previous_frame[H*W];
extern uint16_t current_frame[H*W];
