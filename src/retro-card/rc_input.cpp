#include "rc_input.h"

#include <Adafruit_MCP23X08.h>
#include "../defines.h"

Adafruit_MCP23X08 mcp;

bool pad[8] = {0};
bool old_pad[8] = {0};

bool rc_input_init()
{
    bool success = mcp.begin_I2C();
    mcp.pinMode(GPIO_UP, INPUT_MODE);
    mcp.pinMode(GPIO_DOWN, INPUT_MODE);
    mcp.pinMode(GPIO_LEFT, INPUT_MODE);
    mcp.pinMode(GPIO_RIGHT, INPUT_MODE);
    mcp.pinMode(GPIO_A, INPUT_MODE);
    mcp.pinMode(GPIO_B, INPUT_MODE);
    return success;
}

void rc_read_pad()
{
    for (int i = 0; i < 8; i++)
    {
        old_pad[i] = pad[i];
        pad[i] = 0;
    }

    uint8_t inputs[6] = {0, 0, 0, 0, 0, 0};

    inputs[KEY_UP] = !mcp.digitalRead(GPIO_UP);
    inputs[KEY_DOWN] = !mcp.digitalRead(GPIO_DOWN);
    inputs[KEY_LEFT] = !mcp.digitalRead(GPIO_LEFT);
    inputs[KEY_RIGHT] = !mcp.digitalRead(GPIO_RIGHT);
    inputs[KEY_A] = !mcp.digitalRead(GPIO_A);
    inputs[KEY_B] = !mcp.digitalRead(GPIO_B);

    if (inputs[KEY_A])
    {
        // Alternate mode
        if (inputs[KEY_UP])
        {
            // Select
            pad[KEY_SELECT] = 1;
        }
        else if (inputs[KEY_DOWN])
        {
            // Start
            pad[KEY_START] = 1;
        }
        else
        {
            // A
            pad[KEY_A] = 1;
        }
    }
    else
    {
        // Normal mode
        for (int i = 0; i < 6; i++)
        {

            pad[i] = inputs[i];
        }
    }
}