#include <Arduino.h>
#include "gbc.h"
extern "C"
{
#include "gnuboy.h"
}
#include "../globals.h"

uint16_t *videoBufferGB = NULL;
uint16_t *oldvideoBufferGB = NULL;

int draw = 0;

void video_callback(void *buffer)
{
	tft.startWrite();
	for (int x = 0; x < 160; x++)
	{
		for (int y = 0; y < 144; y++)
		{
			if (videoBufferGB[x + y * 160] != oldvideoBufferGB[x + y * 160])
			{
				tft.writePixel(x + 40, y, videoBufferGB[x + y * 160]);
			}
		}
	}
	tft.endWrite();
}

void audio_callback(void *buffer, size_t length) {}

// void setupGBC(byte *rom, size_t romSize)
void setupGBC(char *romfilename)
{
	Serial.println("Setting up GBC");
	Serial.println(romfilename);
	
	// Initialize the emulator
	if (gnuboy_init(16000, GB_AUDIO_STEREO_S16, GB_PIXEL_565_LE, &video_callback, &audio_callback) < 0)
		PANIC("Emulator init failed!");

	Serial.println("Emulator initialized");

	// Allocate video and audio buffers
	videoBufferGB = (uint16_t *)malloc(160 * 144 * sizeof(uint16_t));
	oldvideoBufferGB = (uint16_t *)malloc(160 * 144 * sizeof(uint16_t));
	if (!videoBufferGB)
		PANIC("Video buffer allocation failed!");
	Serial.println("Video buffer allocated");
	if (!oldvideoBufferGB)
		PANIC("Old video buffer allocation failed!");
	Serial.println("Old video buffer allocated");

	// Allocate audio buffer
	gnuboy_set_framebuffer((void *)videoBufferGB);
	Serial.println("Frame buffer set");

	// Load ROM
	fp = SD.open(romfilename, FILE_READ);
	if (!fp)
	{
		PANIC("ROM Loading failed!");
	}
	Serial.println("ROM opened");

	size_t size = fp.size();
	Serial.println(fp.size());
	byte *data;
	Serial.println("ROM size read");
	data = (byte *)ps_malloc(size);
	Serial.println(fp.read(data, size));

	gnuboy_load_rom((const byte *)data, size);
	// gnuboy_load_rom((const byte *)rom, romSize);

	Serial.println("ROM loaded");

	gnuboy_set_palette(GB_PALETTE_DMG);
	Serial.println("Palette set");

	// Hard reset to have a clean slate
	gnuboy_reset(true);
	Serial.println("Emulator reset");

	// Pin audio task to core 0
}

long loopGBC()
{
	long now = millis();

	bool changed = false;
	for (int i = 0; i < 8; i++)
	{
		if (keys[i] != oldKeys[i])
		{
			Serial.println("Key changed");
			changed = true;
			break;
		}
	}
	if (changed)
	{
		int pad = 0;
		if (keys[KEY_UP])
			pad |= GB_PAD_UP;
		if (keys[KEY_RIGHT])
			pad |= GB_PAD_RIGHT;
		if (keys[KEY_DOWN])
			pad |= GB_PAD_DOWN;
		if (keys[KEY_LEFT])
			pad |= GB_PAD_LEFT;
		if (keys[KEY_A])
			pad |= GB_PAD_A;
		if (keys[KEY_B])
			pad |= GB_PAD_B;
		if (keys[KEY_SELECT])
			pad |= GB_PAD_SELECT;
		if (keys[KEY_START])
			pad |= GB_PAD_START;

		gnuboy_set_pad(pad);

		Serial.println("Updating pad");
	}

	if (draw <= 0)
	{
		gnuboy_run(1); // 1 = draw
		draw = 3;
		// copy video buffer to old video buffer
		memcpy(oldvideoBufferGB, videoBufferGB, 160 * 144 * sizeof(uint16_t));
	}
	else
	{
		draw--;
		gnuboy_run(0); // 0 = no draw
	}

	// Serial.print("Frame time: ");
	// Serial.println((millis() - now));
	return (millis() - now);
}