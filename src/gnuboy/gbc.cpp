#include <Arduino.h>
#include "gbc.h"
extern "C"
{
#include "gnuboy.h"
}
#include "../globals.h"

#define PANIC(x)          \
	{                     \
		Serial.printf(x); \
		while (1)         \
			;             \
	}

int draw = 0;

rc_framebuffer *framebuffer;

void video_callback(void *buffer)
{
	rc_send_frame(framebuffer);
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

	rc_create_framebuffer(framebuffer, 160, 144, false);

	gnuboy_set_framebuffer(framebuffer->buffer);
	Serial.println("Frame buffer set");

	// Load ROM
	rc_open_dir("/");

	rc_open_file(romfilename);
	size_t size = rc_get_file_size();
	Serial.println("ROM size read");
	byte *data = (byte *)ps_malloc(size);
	rc_read_file(data, size);

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
		if (pad[i] != old_pad[i])
		{
			Serial.println("Key changed");
			changed = true;
			break;
		}
	}
	if (changed)
	{
		int gb_pad = 0;
		if (pad[KEY_UP])
			gb_pad |= GB_PAD_UP;
		if (pad[KEY_RIGHT])
			gb_pad |= GB_PAD_RIGHT;
		if (pad[KEY_DOWN])
			gb_pad |= GB_PAD_DOWN;
		if (pad[KEY_LEFT])
			gb_pad |= GB_PAD_LEFT;
		if (pad[KEY_A])
			gb_pad |= GB_PAD_A;
		if (pad[KEY_B])
			gb_pad |= GB_PAD_B;
		if (pad[KEY_SELECT])
			gb_pad |= GB_PAD_SELECT;
		if (pad[KEY_START])
			gb_pad |= GB_PAD_START;

		gnuboy_set_pad(gb_pad);

		Serial.println("Updating pad");
	}

	if (draw <= 0)
	{
		gnuboy_run(1); // 1 = draw
		draw = 3;
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