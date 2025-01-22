#include "src/gnuboy/gbc.h"
#include "src/smsplus/mastersystem.h"
#include "src/nofrendo/nes.h"
#include "src/globals.h"

enum console_t
{
	GBC,
	NES,
	SMS,
	UNSELECTED
};

struct game_t
{
	char *filename;
	console_t console;
} game;

int selection_menu(char **options, int num_options)
{
	Serial.println("Select:");
	for (int i = 0; i < num_options; i++)
	{
		tft.setCursor(20, 10 + i * 20);
		tft.println(options[i]);
	}

	tft.setCursor(0, 10);
	tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
	tft.setTextWrap(false);
	tft.print(">");

	int selected = -1;
	int cursor = 0;

	while (selected == -1)
	{
		rc_read_pad();

		if (pad[KEY_DOWN])
		{
			tft.setCursor(0, 10 + cursor * 20);
			tft.print(" ");
			cursor++;
			if (cursor >= num_options)
			{
				cursor = 0;
			}

			tft.setCursor(0, 10 + cursor * 20);
			tft.print(">");

			delay(200);
		}
		else if (pad[KEY_UP])
		{
			tft.setCursor(0, 10 + cursor * 20);
			tft.print(" ");
			cursor--;
			if (cursor < 0)
			{
				cursor = num_options - 1;
			}

			tft.setCursor(0, 10 + cursor * 20);
			tft.print(">");

			delay(200);
		}
		else if (pad[KEY_A])
		{
			selected = cursor;
		}
	}

	return selected;
}

void select_rom()
{
	// --------------------------------
	// ---- Find all files

	rc_open_dir("/");
	uint8_t num_files = rc_get_num_files();

	char *files[num_files];
	rc_get_file_names(files, num_files);

	Serial.println("Files in root:");
	for (int i = 0; i < num_files; i++)
	{
		Serial.println(files[i]);
	}

	// --------------------------------
	// ---- Choose console

	char *consoles[] = {"Gameboy Color", "NES", "Master System"}; // Order is important!!
	uint8_t console = selection_menu(consoles, 3);

	Serial.print("Selected console:");
	Serial.println(console);

	delay(500);

	// --------------------------------
	// ---- Filter ROMs

	uint8_t num_roms = 0;
	char *roms[num_files];
	for (int i = 0; i < num_files; i++)
	{
		// Check if the file is a valid ROM
		if (files[i][0] != '.')
		{
			if (console == GBC)
			{
				if (strstr(files[i], ".gbc") || strstr(files[i], ".gb"))
				{
					roms[num_roms] = files[i];
					num_roms++;
				}
			}
			else if (console == NES)
			{
				if (strstr(files[i], ".nes"))
				{
					roms[num_roms] = files[i];
					num_roms++;
				}
			}
			else if (console == SMS)
			{
				if (strstr(files[i], ".sms"))
				{
					roms[num_roms] = files[i];
					num_roms++;
				}
			}
		}
	}

	Serial.println("ROMs found:");
	for (int i = 0; i < num_roms; i++)
	{
		Serial.println(roms[i]);
	}

	// ---- Choose ROM
	tft.fillScreen(0x0000);
	uint8_t rom = selection_menu(roms, num_roms);

	// --------------------------------
	// ---- game_t setup

	game.filename = roms[rom];
	if (console == GBC)
	{
		game.console = GBC;
	}
	else if (console == NES)
	{
		game.console = NES;
	}
	else if (console == SMS)
	{
		game.console = SMS;
	}

	Serial.println("Selected ROM:");
	Serial.println(game.filename);
	Serial.println("Selected console:");
	Serial.println(game.console);
}

void setup()
{
	Serial.begin(115200);
	// delay(10000);

	Serial.println("Starting...");
	if (!rc_system_init()) {
		PANIC("System init failed");
	}
	Serial.println("Hardware setup complete");

	delay(500);

	select_rom();

	tft.fillScreen(0x0000);

	switch (game.console)
	{
	case GBC:
		setupGBC(game.filename);
		break;
	case NES:
		setupNES(game.filename);
		break;
	case SMS:
		setupSMS(game.filename);
		break;
	default:
		break;
	}
}

void loop()
{
	switch (game.console)
	{
	case GBC:
		loopGBC();
		break;
	case NES:
		loopNES();
		break;
	case SMS:
		loopSMS();
		break;
	default:
		break;
	}
}