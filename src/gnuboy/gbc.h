#pragma once
#include <Arduino.h>

// --- MAIN
extern void video_callback(void *buffer);

extern void audio_callback(void *buffer, size_t length);

// extern void setupGBC(byte *rom, size_t romSize);
extern void setupGBC(char *romfilename);

extern long loopGBC();