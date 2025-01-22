#include "rc_video.h"

QueueHandle_t video_queue;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void write_frame(rc_framebuffer *fb) {}

void video_task(void *params)
{
    rc_framebuffer fb;

    while (true)
    {
        if (xQueueReceive(video_queue, &fb, portMAX_DELAY) == pdTRUE)
        {
            uint16_t *final_buffer = (uint16_t *)malloc(fb.width * fb.height * sizeof(uint16_t));

            if (fb.use_palette)
            {
                for (int pos = 0; pos < fb.height * fb.width; pos++)
                {
                    final_buffer[pos] = fb.palette[((uint8_t *)fb.buffer)[pos]];
                }
            }
            else
            {
                memcpy(final_buffer, fb.buffer, fb.width * fb.height * sizeof(uint16_t));
            }
            tft.drawRGBBitmap((240 / 2) - (fb.width / 2), (240 / 2) - (fb.height / 2), (uint16_t *)final_buffer, fb.width, fb.height);
            free(final_buffer);
        }
    }
    delay(1000 / 20); // 60 FPS delay to yeild to FreeRTOS (crashes without it)
}

bool rc_video_init()
{
    video_queue = xQueueCreate(1, sizeof(rc_framebuffer));
    if (video_queue == NULL)
    {
        return false;
    }

    // TFT
    tft.setSPISpeed(40000000);
    tft.init(TFT_WIDTH, TFT_HEIGHT); // Init ST7789 display 240x240 pixel
    tft.setSPISpeed(40000000);
    tft.setRotation(TFT_ROTATION);
    tft.fillScreen(0x0000);

    // Create video task on core 0
    xTaskCreatePinnedToCore(video_task, "video_task", 4096, NULL, 1, NULL, 0);

    return true;
}

bool rc_create_framebuffer(rc_framebuffer *fb, uint16_t width, uint16_t height, bool use_palette)
{
    fb->width = width;
    fb->height = height;
    fb->use_palette = use_palette;

    if (use_palette)
    {
        fb->palette = (uint16_t *)malloc(256 * sizeof(uint16_t));
        if (!fb->palette)
        {
            return false;
        }

        fb->buffer = (uint8_t *)malloc(width * height * sizeof(uint8_t));
        if (!fb->buffer)
        {
            free(fb->palette);
            return false;
        }
    }
    else
    {
        fb->buffer = malloc(width * height * sizeof(uint16_t));
        if (!fb->buffer)
        {
            return false;
        }
    }

    return true;
}

bool rc_send_frame(rc_framebuffer *fb)
{
    if (xQueueSend(video_queue, fb, 0) != pdTRUE)
    {
        return false;
    }

    return true;
}

void rc_destroy_framebuffer(rc_framebuffer *fb)
{
    if (fb->use_palette)
    {
        free(fb->palette);
    }
    free(fb->buffer);
}