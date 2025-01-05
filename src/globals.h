#pragma once

#include "../hardware.h"

#define PANIC(x)          \
    {                     \
        Serial.printf(x); \
        while (1)          \
            ;             \
    }

typedef enum console_t
{
    GBC,
    NES,
    SMS,
    UNSUPPORTED
};

typedef struct game_t
{
    char *filename;
    console_t console;
};