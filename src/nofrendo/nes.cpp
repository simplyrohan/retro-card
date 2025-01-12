#include <Arduino.h>
extern "C"
{
#include "nofrendo.h"
}
#include "../globals.h"

static nes_t *nes;

rc_framebuffer nes_framebuffer;


// --- MAIN
static void build_palette(nespal_t n)
{
    nes_framebuffer.palette = (uint16_t *)nofrendo_buildpalette(n, 16);
}

static void blit_screen(uint8 *bmp)
{
    rc_send_frame(&nes_framebuffer);
}

void setupNES(char *romfilename)
{
    Serial.begin(115200);
    delay(2000);
    Serial.println("Starting...");

    rc_create_framebuffer(&nes_framebuffer, NES_SCREEN_PITCH, NES_SCREEN_HEIGHT, true);

    nes = nes_init(SYS_DETECT, 16000, true, NULL);
    if (!nes)
        PANIC("Init failed.");
    Serial.println("NES init done.");

    int ret = -1;

    // ---- Read ROM
    rc_open_file(romfilename);
    size_t size = rc_get_file_size();
    uint8_t *data = (uint8_t *)malloc(size);
    rc_read_file(data, size);
    rc_close_file();

    // ---- Insert ROM
    ret = nes_insertcart(rom_loadmem(data, size));
    Serial.println("ROM inserted.");

    if (ret == -1)
    {
        PANIC("ROM load failed.");
    }
    else if (ret == -2)
    {
        PANIC("Unsupported mapper.");
    }
    else if (ret == -3)
    {
        PANIC("BIOS file required.");
    }
    else if (ret < 0)
    {
        PANIC("Unsupported ROM.");
    }

    // ---- NES Config
    nes->blit_func = blit_screen;

    build_palette(NES_PALETTE_NOFRENDO); // Default palette
    Serial.println("Palette built.");


    // This is necessary for successful state restoration
    // I have not yet investigated why that is...
    nes_emulate(false);
    nes_emulate(false);

    nes_setvidbuf((uint8_t *)nes_framebuffer.buffer);
    Serial.println("DONE");
}

long loopNES()
{
    int buttons = 0;

    rc_read_pad();

    if (pad[KEY_UP])
        buttons |= NES_PAD_UP;
    if (pad[KEY_RIGHT])
        buttons |= NES_PAD_RIGHT;
    if (pad[KEY_DOWN])
        buttons |= NES_PAD_DOWN;
    if (pad[KEY_LEFT])
        buttons |= NES_PAD_LEFT;
    if (pad[KEY_A])
        buttons |= NES_PAD_A;
    if (pad[KEY_B])
        buttons |= NES_PAD_B;
    if (pad[KEY_START])
        buttons |= NES_PAD_START;
    if (pad[KEY_SELECT])
        buttons |= NES_PAD_SELECT;

    input_update(0, buttons);
    nes_emulate(true); // true = draw frame

    return 0;
}