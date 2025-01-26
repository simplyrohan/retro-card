#include <Arduino.h>
#include "nes.h"
extern "C"
{
#include "nofrendo.h"
}
#include "../globals.h"

nespal_t palette = NES_PALETTE_NOFRENDO;
static nes_t *nes;

rc_framebuffer nes_framebuffer;

static void build_palette(nespal_t n)
{
    uint16_t *pal = (uint16_t *)nofrendo_buildpalette(n, 16);
    for (int i = 0; i < 256; i++)
    { 
        nes_framebuffer.palette[i] = pal[i];
    }
    free(pal);
}

static void blit_screen(uint8 *bmp)
{
    rc_send_frame(&nes_framebuffer);
}

void setupNES(char *romfilename)
{
    Serial.println("Setting up NES");
    Serial.println(romfilename);

    rc_create_framebuffer(&nes_framebuffer, NES_SCREEN_PITCH, NES_SCREEN_HEIGHT, true);

    nes = nes_init(SYS_DETECT, 16000, true, NULL);
    if (!nes)
        PANIC("Init failed.");

    rc_open_dir("/");
    rc_open_file(romfilename);
    size_t size = rc_get_file_size();

    byte *data = (byte *)ps_malloc(size);
    rc_read_file(data, size);

    int ret = nes_insertcart(rom_loadmem(data, size));

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
    // TODO: i should implement this into the loop as well as aother emulators
    // app->tickRate = nes->refresh_rate;
    nes->blit_func = blit_screen;

    // ppu_setopt(PPU_LIMIT_SPRITES, rg_settings_get_number(NS_APP, SETTING_SPRITELIMIT, 1));

    build_palette(palette);

    // This is necessary for successful state restoration
    // I have not yet investigated why that is...
    nes_emulate(false);
    nes_emulate(false);
}

long loopNES()
{
    int buttons = 0;

    rc_read_pad();
    if (pad[KEY_UP])
        buttons |= NES_PAD_UP;
    if (pad[KEY_DOWN])
        buttons |= NES_PAD_DOWN;
    if (pad[KEY_LEFT])
        buttons |= NES_PAD_LEFT;
    if (pad[KEY_RIGHT])
        buttons |= NES_PAD_RIGHT;
    if (pad[KEY_A])
        buttons |= NES_PAD_A;
    if (pad[KEY_B])
        buttons |= NES_PAD_B;
    if (pad[KEY_START])
        buttons |= NES_PAD_START;
    if (pad[KEY_SELECT])
        buttons |= NES_PAD_SELECT;

    nes_setvidbuf((uint8_t *)nes_framebuffer.buffer);

    input_update(0, buttons);

    // There is a delay with all the other stuff in this loop so we can speed up the emulator with this
    nes_emulate(0); // 1 = draw
    nes_emulate(0); // 1 = draw
    nes_emulate(1); // 1 = draw

    return 0;
}