#include "rc_video.h"

QueueHandle_t video_queue;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void video_task(void *params)
{
    rc_framebuffer fb;
    while (true)
    {
        if (xQueueReceive(video_queue, &fb, portMAX_DELAY) == pdTRUE)
        {
            tft.fillScreen(0x0000);
            if (fb.use_palette)
            {
                for (int y = 0; y < fb.height; y++)
                {
                    for (int x = 0; x < fb.width; x++)
                    {
                        uint8_t index = ((uint8_t *)fb.buffer)[y * fb.width + x];
                        tft.drawPixel(x, y, fb.palette[index]);
                    }
                }
            }
            else
            {
                for (int y = 0; y < fb.height; y++)
                {
                    for (int x = 0; x < fb.width; x++)
                    {
                        uint16_t color = ((uint16_t *)fb.buffer)[y * fb.width + x];
                        tft.drawPixel(x, y, color);
                    }
                }
            }
        }
    }
}

bool rc_video_init()
{
    video_queue = xQueueCreate(1, sizeof(rc_framebuffer));
    if (video_queue == NULL)
    {
        return false;
    }

    // TFT
    tft.init(TFT_WIDTH, TFT_HEIGHT); // Init ST7789 display 240x240 pixel
    tft.setSPISpeed(80000000);
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
        if (fb->buffer == NULL)
        {
            return false;
        }
    }

    return true;
}

bool rc_send_frame(rc_framebuffer *fb)
{
    if (xQueueSend(video_queue, fb, portMAX_DELAY) != pdTRUE)
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