#include <Arduino.h>
#include "snes.h"
extern "C"
{

#include "snes9x.h"
#include "soundux.h"
#include "memmap.h"
#include "apu.h"
#include "display.h"
#include "gfx.h"
#include "cpuexec.h"
#include "srtc.h"
#include "save.h"
}
#include "../globals.h"

rc_framebuffer snes_framebuffer;

bool keys[12] = {0};

struct keymap
{
    int keycode;
    int snes_key;
};

struct keymap keymap[] = {
    {KEY_UP, SNES_UP_MASK},
    {KEY_DOWN, SNES_DOWN_MASK},
    {KEY_LEFT, SNES_LEFT_MASK},
    {KEY_RIGHT, SNES_RIGHT_MASK},

    {KEY_A, SNES_A_MASK},
    {-1, SNES_B_MASK},
    {KEY_B, SNES_X_MASK},
    {-1, SNES_Y_MASK},

    {-1, SNES_TL_MASK},
    {-1, SNES_TR_MASK},

    {KEY_SELECT, SNES_SELECT_MASK},
    {KEY_START, SNES_START_MASK},
};

bool S9xInitDisplay(void)
{
    GFX.Pitch = SNES_WIDTH * 2;
    GFX.ZPitch = SNES_WIDTH;
    GFX.Screen = (uint8_t *)snes_framebuffer.buffer; // (buffer is actually a uint16_t array, but snes9x takes it as uint8_t)
    GFX.SubScreen = (uint8_t *)malloc(GFX.Pitch * SNES_HEIGHT_EXTENDED);
    GFX.ZBuffer = (uint8_t *)malloc(GFX.ZPitch * SNES_HEIGHT_EXTENDED);
    GFX.SubZBuffer = (uint8_t *)malloc(GFX.ZPitch * SNES_HEIGHT_EXTENDED);
    return GFX.Screen && GFX.SubScreen && GFX.ZBuffer && GFX.SubZBuffer;
}

uint32_t S9xReadJoypad(int32_t port)
{
    if (port != 0)
        return 0;

    uint32_t joypad = 0;
    for (int i = 0; i < 12; ++i)
    {
        if (keys[i])
        {
            joypad |= keymap[i].snes_key;
        }
    }

    return joypad;

    return 0;
}

bool S9xReadMousePosition(int32_t which1, int32_t *x, int32_t *y, uint32_t *buttons)
{
    return false;
}

bool S9xReadSuperScopePosition(int32_t *x, int32_t *y, uint32_t *buttons)
{
    return false;
}

bool JustifierOffscreen(void)
{
    return true;
}

void JustifierButtons(uint32_t *justifiers)
{
    (void)justifiers;
}

void setupSNES(char *romfilename)
{
    Serial.println("Setting up SNES");
    Serial.println(romfilename);

    Settings.CyclesPercentage = 100;
    Settings.H_Max = SNES_CYCLES_PER_SCANLINE;
    Settings.FrameTimePAL = 20000;
    Settings.FrameTimeNTSC = 16667;
    Settings.ControllerOption = SNES_JOYPAD;
    Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;
    Settings.SoundPlaybackRate = 48000;
    Settings.DisableSoundEcho = false;
    Settings.InterpolatedSound = true;
    Serial.println("Settings set");

    rc_create_framebuffer(&snes_framebuffer, SNES_WIDTH, SNES_HEIGHT_EXTENDED, false);
    snes_framebuffer.height = SNES_HEIGHT;
    Serial.println("Frame buffer created");

    if (!S9xInitDisplay())
        PANIC("Display init failed!");

    if (!S9xInitMemory())
        PANIC("Memory init failed!");

    if (!S9xInitAPU())
        PANIC("APU init failed!");

    if (!S9xInitSound(0, 0))
        PANIC("Sound init failed!");

    if (!S9xInitGFX())
        PANIC("Graphics init failed!");

    Serial.println("Emulator initialized");

    // FILE *file = fopen(filename, "rb");
    // if (!file)
    //     PANIC("ROM file not found!");

    // fseek(file, 0, SEEK_END);
    // long size = ftell(file);
    // fseek(file, 0, SEEK_SET);

    // uint8_t *rom = (uint8_t *)malloc(size);
    // if (!rom)
    //     PANIC("ROM allocation failed!");

    // fread(rom, 1, size, file);
    // fclose(file);

    // Load ROM
    rc_open_dir("/");

    rc_open_file(romfilename);
    size_t size = rc_get_file_size();
    Serial.println("ROM size read");
    byte *data = (byte *)ps_malloc(size);
    rc_read_file(data, size);

    Serial.println("ROM read");
    
    Memory.ROM = data;
    Memory.ROM_AllocSize = size;

    if (!LoadROM(NULL))
        PANIC("ROM loading failed!");

    S9xSetPlaybackRate(Settings.SoundPlaybackRate);

}

long loopSNES()
{
    Serial.println("Looping SNES");

    rc_read_pad();

    for (int i = 0; i < 12; ++i)
    {
        if (keymap[i].keycode != -1)
        {
            keys[i] = pad[keymap[i].keycode];
        }
    }

    IPPU.RenderThisFrame = 0;
    S9xMainLoop();
    IPPU.RenderThisFrame = 0;
    S9xMainLoop();

    IPPU.RenderThisFrame = 1;
    S9xMainLoop();

    rc_send_frame(&snes_framebuffer);

    delay(16);
    return 0;
}