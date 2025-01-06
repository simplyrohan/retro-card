// Main code to run SMSPlus emulator with the Arduino IDE
#include "mastersystem.h"

unsigned char *videoBuffer;
unsigned char *oldVideoBuffer;

unsigned char *backBuffer;

uint16_t palette[256] = {0};

QueueHandle_t videoQueue;

void drawFrame()
{
    // tft.fillScreen(0);
    double start = millis();
    tft.startWrite();
    for (int x = 0; x < (SMS_WIDTH); x++)
    {
        for (int y = 0; y < (SMS_HEIGHT / 2); y++)
        {
            int pos = (y * 2) * SMS_WIDTH + (x);
            if (videoBuffer[pos] != oldVideoBuffer[pos]) // Some times a different number has the same color
            {
                // oldVideoBuffer[pos] = videoBuffer[pos];
                tft.writePixel(x, y, palette[videoBuffer[pos]]);
            }
        }
    }
    tft.endWrite();
    Serial.printf("Frame drawn in %f ms\n", millis() - start);

    // Copy video buffer to old video buffer
    memcpy(oldVideoBuffer, videoBuffer, SMS_WIDTH * SMS_HEIGHT);
}

// Video Task
void videoTask(void *pvParameters)
{
    while (true)
    {
        // Wait for video buffer
        xQueueReceive(videoQueue, videoBuffer, portMAX_DELAY);
        Serial.println("Frame received from queue");

        drawFrame();
    }
}

// --- MAIN
void setupSMS(char *romfilename)
{
    Serial.println("Setting up SMS");
    Serial.println(romfilename);

    system_reset_config();
    option_sms.sndrate = 16000;
    option_sms.overscan = 0;
    option_sms.extra_gg = 0;
    option_sms.tms_pal = 0;

    option_sms.console = 0; // SMS

    Serial.println(render_copy_palette(palette));

    // ------------------------------------------
    // Load ROM
    void *data;
    size_t size;
    // fp = SD.open("sonic.sms", FILE_READ);
    Serial.println(romfilename);
    fp = SD.open(romfilename, FILE_READ);
    if (!fp)
    {
        PANIC("File Loading failed!");
    }
    Serial.println("ROM opened");

    size = fp.size();
    Serial.println(fp.size());
    Serial.println("ROM size read");
    data = (byte *)ps_malloc(size);
    Serial.println(fp.read(data, size));

    if (!load_rom(data, (int)size, size))
        PANIC("ROM file loading failed!");
    // ------------------------------------------

    // ------------------------------------------
    // Allocate video buffers
    videoBuffer = (unsigned char *)malloc(SMS_WIDTH * SMS_HEIGHT);
    if (!videoBuffer)
        PANIC("Failed to allocate video buffer!");
    oldVideoBuffer = (unsigned char *)malloc(SMS_WIDTH * SMS_HEIGHT);
    if (!oldVideoBuffer)
        PANIC("Failed to allocate old video buffer!");

    backBuffer = (unsigned char *)malloc(SMS_WIDTH * SMS_HEIGHT);
    if (!backBuffer)
        PANIC("Failed to allocate back buffer!");
    // ------------------------------------------

    bitmap_sms.width = SMS_WIDTH;
    bitmap_sms.height = SMS_HEIGHT;
    bitmap_sms.pitch = bitmap_sms.width;
    bitmap_sms.data = backBuffer;

    system_poweron();

    // ------------------------------------------
    // Create video queue
    videoQueue = xQueueCreate(1, SMS_WIDTH * SMS_HEIGHT);

    // Create video task
    xTaskCreatePinnedToCore(videoTask, "VideoTask", 3072, NULL, 0, NULL, 0);

    Serial.println("Setup done!");
}

void loopSMS()
{
    input.pad[0] = 0x00;
    input.pad[1] = 0x00;
    input.system = 0x00;

    if (keys[KEY_UP])
        input.pad[0] |= INPUT_UP;
    if (keys[KEY_DOWN])
        input.pad[0] |= INPUT_DOWN;
    if (keys[KEY_LEFT])
        input.pad[0] |= INPUT_LEFT;
    if (keys[KEY_RIGHT])
        input.pad[0] |= INPUT_RIGHT;
    if (keys[KEY_A])
        input.pad[0] |= INPUT_BUTTON2;
    if (keys[KEY_B])
        input.pad[0] |= INPUT_BUTTON1;

    if (keys[KEY_START])
        input.system |= INPUT_START;
    if (keys[KEY_SELECT])
        input.system |= INPUT_PAUSE;

    // system_frame(1); // 0 = draw frame
    system_frame(0); // 0 = draw frame

    // Add video buffer to queue
    xQueueSend(videoQueue, backBuffer, 0);
    Serial.println("Frame sent to queue");

    Serial.println(render_copy_palette(palette)); // Could go to the video task but it kept crashing

    delay(1000 / 60);
}