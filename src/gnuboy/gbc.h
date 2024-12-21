#pragma once
#include <Arduino.h>

extern uint16_t *videoBuffer;
extern uint16_t *oldVideoBuffer;

extern void *audioBuffer;

extern int draw;

// --- MAIN
extern void video_callback(void *buffer);

extern void audio_callback(void *buffer, size_t length);

extern void setupGBC(byte* rom, size_t romSize);

extern long loopGBC();