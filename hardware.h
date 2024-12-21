#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include "Adafruit_GFX.h"
#include "Adafruit_ST7789.h"
#include "defines.h"
#include <Adafruit_MCP23X08.h>
#include "SdFat.h"

extern SdFat SD;
extern File fp;

extern Adafruit_ST7789 tft;
extern Adafruit_MCP23X08 mcp;

extern bool keys[8];
extern bool oldKeys[8];

extern void setupHardware();

void get_keys();

#endif