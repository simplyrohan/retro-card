#ifndef HARDWARE_H
#define HARDWARE_H

#include "Adafruit_GFX.h"
#include "Adafruit_ST7789.h"
#include "defines.h"
#include <Adafruit_MCP23X08.h>
// Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_MCP23X08 mcp;

bool keys[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setupHardware()
{

    pinMode(TFT_CS, OUTPUT);

    if (!mcp.begin_I2C())
    {
        Serial.println("Error initilizing MCP23008.");
        while (1)
            ;
    }
    mcp.pinMode(GPIO_UP, INPUT_MODE);
    mcp.pinMode(GPIO_DOWN, INPUT_MODE);
    mcp.pinMode(GPIO_LEFT, INPUT_MODE);
    mcp.pinMode(GPIO_RIGHT, INPUT_MODE);
    mcp.pinMode(GPIO_A, INPUT_MODE);
    mcp.pinMode(GPIO_B, INPUT_MODE);
}

void get_keys()
{
    // Clear keys
    for (int i = 0; i < 8; i++)
    {
        keys[i] = 0;
    }

    uint8_t inputs[6] = {0, 0, 0, 0, 0, 0};

    inputs[KEY_UP] = !mcp.digitalRead(GPIO_UP);
    inputs[KEY_DOWN] = !mcp.digitalRead(GPIO_DOWN);
    inputs[KEY_LEFT] = !mcp.digitalRead(GPIO_LEFT);
    inputs[KEY_RIGHT] = !mcp.digitalRead(GPIO_RIGHT);
    inputs[KEY_A] = !mcp.digitalRead(GPIO_A);
    inputs[KEY_B] = !mcp.digitalRead(GPIO_B);

    Serial.print("UP: ");
    Serial.println(inputs[KEY_UP]);
    Serial.print("DOWN: ");
    Serial.println(inputs[KEY_DOWN]);
    Serial.print("LEFT: ");
    Serial.println(inputs[KEY_LEFT]);
    Serial.print("RIGHT: ");
    Serial.println(inputs[KEY_RIGHT]);
    Serial.print("A: ");
    Serial.println(inputs[KEY_A]);
    Serial.print("B: ");
    Serial.println(inputs[KEY_B]);

    if (inputs[KEY_A])
    {
        // Alternate mode
        if (inputs[KEY_UP])
        {
            // Select
            keys[KEY_SELECT] = 1;
        }
        else if (inputs[KEY_DOWN])
        {
            // Start
            keys[KEY_START] = 1;
        }
        else
        {
            // A
            keys[KEY_A] = 1;
        }
    }
    else
    {
        // Normal mode
        for (int i = 0; i < 6; i++)
        {

            keys[i] = inputs[i];
        }
    }
    // keys[KEY_UP] = !mcp.digitalRead(GPIO_UP);
    // keys[KEY_DOWN] = !mcp.digitalRead(GPIO_DOWN);
    // keys[KEY_LEFT] = !mcp.digitalRead(GPIO_LEFT);
    // keys[KEY_RIGHT] = !mcp.digitalRead(GPIO_RIGHT);
    // keys[KEY_A] = !mcp.digitalRead(GPIO_A);
    // keys[KEY_B] = !mcp.digitalRead(GPIO_B);
}

#endif