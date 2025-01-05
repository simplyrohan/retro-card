// Main code to run SMSPlus emulator with the Arduino IDE
#include <Arduino.h>
// Main code to run SMSPlus emulator with the Arduino IDE
extern "C"
{
#include "smsplus.h"
}
#include "../globals.h"

extern unsigned char *videoBuffer;
extern unsigned char *oldVideoBuffer;

extern unsigned char *backBuffer;

extern uint16_t palette[256];

extern QueueHandle_t videoQueue;

extern void drawFrame();

// Video Task
extern void videoTask(void *pvParameters);

// --- MAIN
extern void setupSMS(char *romfilename);

extern void loopSMS();