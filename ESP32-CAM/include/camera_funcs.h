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
void print_frame(uint8_t frame[H][W]);
size_t count_pixels();
uint8_t *image_to_array();

extern uint8_t previous_frame[H*W];
extern uint16_t current_frame[H*W];
