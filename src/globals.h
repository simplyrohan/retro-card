#pragma once

#include "../hardware.h"


#define PANIC(x)          \
    {                     \
        Serial.printf(x); \
        exit(1);          \
    }
