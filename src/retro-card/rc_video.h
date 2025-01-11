#pragma once

#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include "../defines.h"

extern Adafruit_ST7789 tft;

typedef struct rc_framebuffer
{
    uint16_t width;  // Width of the framebuffer
    uint16_t height; // Height of the framebuffer

    uint16_t *palette; // 5:6:5 RGB palette (256 colors)
    bool use_palette;  // Whether to use a palette or not (if not, buffer is 16-bit RGB565)

    void *buffer; // 8 or 16-bit framebuffer (depending on use_palette)
} rc_framebuffer;

/**
 * Initialize the video system
 * @returns true if the video system was initialized successfully, false otherwise
 */
extern bool rc_video_init();

/**
 * Initialize/allocate an `rc_framebuffer` object
 * @param fb Pointer to the framebuffer object
 * @param width Width of the framebuffer
 * @param height Height of the framebuffer
 * @param use_palette Whether to use a palette or not (will determine the type of the buffer)
 * @returns true if the framebuffer was successfully created, false otherwise
 */
extern bool rc_create_framebuffer(rc_framebuffer *fb, uint16_t width, uint16_t height, bool use_palette);

/**
 * Send framebuffer to be drawn
 * @param fb Pointer to the framebuffer object
 * @returns true if the framebuffer was successfully sent, false if queue is full
 */
extern bool rc_send_frame(rc_framebuffer *fb);

/**
 * Destroy/free a `rc_framebuffer` object
 * @param fb Pointer to the framebuffer object
 */
extern void rc_destroy_framebuffer(rc_framebuffer *fb);