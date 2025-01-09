#include "src/gnuboy/gbc.h"
#include "src/nes/nofrendo.h"
#include "src/smsplus/mastersystem.h"
#include "src/globals.h"

File file;
File dir;

game_t game;

char filenames[255][100];
int filenum = 0;

#define FILES_PER_PAGE 5 // 6 files (0-indexed)

void findFiles()
{
	// Open root directory
	dir = SD.open("/");
	if (!dir)
	{
		PANIC("Failed to open root directory");
	}
	dir.rewind();

	filenum = 0;

	char fileName[100];

	while (file.openNext(&dir, O_READ))
	{
		if (!file.isHidden())
		{
			file.getName(fileName, sizeof(fileName));
			Serial.println(fileName);

			strcpy(filenames[filenum], fileName);
			filenum++;
		}
		file.close();
	}
	dir.close();
}

void selectROM()
{
	findFiles();

	tft.fillScreen(0x0000);
	tft.setTextSize(1);
	tft.setTextColor(0xFFFF, 0x0000);

	// ------------------------------------------
	// Console select
	tft.setCursor(0, 0);
	tft.println("Select Console:");

	tft.setCursor(20, 20);
	tft.println("Gameboy Color");

	tft.setCursor(20, 40);
	tft.println("NES");

	tft.setCursor(20, 60);
	tft.println("Master System");

	// Cursor
	int cursor = 0;

	while (1)
	{
		get_keys();

		if (keys[KEY_UP])
		{
			tft.setCursor(0, 20 + (cursor * 20));
			tft.print(" ");

			cursor--;
			if (cursor < 0)
				cursor = 2;

			delay(200);
		}
		if (keys[KEY_DOWN])
		{
			tft.setCursor(0, 20 + (cursor * 20));
			tft.print(" ");

			cursor++;
			if (cursor > 2)
				cursor = 0;

			delay(200);
		}

		tft.setCursor(0, 20 + (cursor * 20));
		tft.print(">");

		if (keys[KEY_A])
		{
			break;
		}
	}

	if (cursor == 0)
	{
		game.console = GBC;
	}
	else if (cursor == 1)
	{
		game.console = NES;
	}
	else if (cursor == 2)
	{
		game.console = SMS;
	}

	delay(200); // Let go of the button
	// ------------------------------------------
	// ROM select
	// Get ROMS for selected console
	tft.fillScreen(0x0000);
	tft.setCursor(0, 0);
	tft.println("Select ROM:");

	uint8_t roms = 0;
	int romInd[255];
	for (int i = 0; i < filenum; i++)
	{
		if ((strstr(filenames[i], ".gbc") || strstr(filenames[i], ".gb")) && game.console == GBC)
		{
			tft.setCursor(20, 20 + (roms * 20));
			tft.println(filenames[i]);
			romInd[roms] = i;
			roms++;
		}
		else if (strstr(filenames[i], ".nes") && game.console == NES)
		{
			tft.setCursor(20, 20 + (roms * 20));
			tft.println(filenames[i]);
			romInd[roms] = i;
			roms++;
		}
		else if (strstr(filenames[i], ".sms") && game.console == SMS)
		{
			tft.setCursor(20, 20 + (roms * 20));
			tft.println(filenames[i]);
			romInd[roms] = i;
			roms++;
		}
	}
	Serial.println("ROMs found: ");
	Serial.println(roms);

	// Cursor
	cursor = 0;

	while (1)
	{
		get_keys();

		if (keys[KEY_UP])
		{
			tft.setCursor(0, 20 + (cursor * 20));
			tft.print(" ");

			cursor--;
			if (cursor < 0)
				cursor = roms - 1;

			delay(200);
		}
		if (keys[KEY_DOWN])
		{
			tft.setCursor(0, 20 + (cursor * 20));
			tft.print(" ");

			cursor++;
			if (cursor > roms - 1)
				cursor = 0;

			delay(200);
		}

		tft.setCursor(0, 20 + (cursor * 20));
		tft.print(">");

		if (keys[KEY_A])
		{
			Serial.print("Selected ROM: ");
			Serial.println(romInd[cursor]);
			Serial.println(filenames[romInd[cursor]]);
			break;
		}
	}

	// Set selected ROM
	if (game.console == GBC)
	{
		game.filename = filenames[romInd[cursor]];
	}
	else if (game.console == NES)
	{
		game.filename = filenames[romInd[cursor]];
	}
	else if (game.console == SMS)
	{
		game.filename = filenames[romInd[cursor]];
	}

	Serial.print("Selected ROM: ");
	Serial.println(game.filename);

	// ------------------------------------------
}

void printCentered(char *str)
{
	tft.setTextSize(1);

	int16_t x1, y1;
	uint16_t w, h;
	tft.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
	tft.setCursor(240 / 2 - w / 2, 135 / 2 - h / 2);
	tft.println(str);
}

void setup()
{
	Serial.begin(115200);
	delay(2000);

	// Serial.println("Starting...");

	setupHardware();
	Serial.println("Hardware setup complete");

	delay(500);

	// Select ROM Menu
	selectROM();

	Serial.print("Selected ROM: ");
	Serial.println(game.filename);

	// Initialize emulator
	tft.fillScreen(0x0000);

	switch (game.console)
	{
	case (GBC):
		printCentered("Loading Gameboy Color");
		setupGBC(game.filename);
		tft.fillScreen(0x0000);
		delay(1000);

		break;
	case (NES):
		printCentered("Loading NES");
		setupNES(game.filename);
		tft.fillScreen(0x0000);
		delay(1000);

		break;
	case (SMS):
		printCentered("Loading Master System");
		setupSMS(game.filename);
		tft.fillScreen(0x0000);
		delay(1000);

		break;
	default:
		printCentered("Unsupported file type");
		delay(2000);
		PANIC("Unsupported file type");
		break;
	}
}

void loop()
{
	get_keys();

	switch (game.console)
	{
	case (GBC):
		loopGBC();
		break;
	case (NES):
		loopNES();
		break;
	case (SMS):
		loopSMS();
		break;
	default:
		break;
	}
}