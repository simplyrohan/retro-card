#include "src/gnuboy/gbc.h"
#include "src/nes/nofrendo.h"
#include "src/globals.h"

File file;
File dir;

game_t game;

char filenames[255][100];
int filenum = 0;

void makeLowercase(char *str)
{
	for (int i = 0; str[i]; i++)
	{
		str[i] = tolower(str[i]);
	}
}

void selectROM()
{
	findFiles();
	for (int i = 0; i < filenum; i++)
	{
		tft.setCursor(20, 20 + (i * 20));
		tft.println(filenames[i]);
	}

	uint8_t selected = -1;
	uint8_t cursor = 0;

	// Draw cursor
	tft.setTextColor(0xFFFF, 0x0000);
	tft.setCursor(0, 20);
	tft.println(">");
	while (1)
	{
		get_keys();

		if (keys[KEY_UP])
		{
			Serial.println("Up");

			tft.setCursor(0, 20 + (cursor * 20));
			tft.println(" ");
			cursor--;
			if (cursor < 0)
				cursor = filenum - 1;

			tft.setCursor(0, 20 + (cursor * 20));
			tft.println(">");
			delay(200);
		}
		if (keys[KEY_DOWN])
		{
			Serial.println("Down");

			tft.setCursor(0, 20 + (cursor * 20));
			tft.println(" ");
			cursor++;
			if (cursor >= filenum)
				cursor = 0;

			tft.setCursor(0, 20 + (cursor * 20));
			tft.println(">");
			delay(200);
		}

		if (keys[KEY_A])
		{
			game.filename = filenames[cursor];
			if (strstr(game.filename, ".gbc") || strstr(game.filename, ".gb"))
			{
				game.console = GBC;
			}
			else if (strstr(game.filename, ".nes"))
			{
				game.console = NES;
			}
			else
			{
				game.console = UNSUPPORTED;
			}
			return;
		}

		delay(5);
	}
}

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

			// check if file is a ROM
			// if (strstr(fileName, ".gbc") || strstr(fileName, ".gb"))
			// {
				strcpy(filenames[filenum], fileName);
				filenum++;
			// }
		}
		file.close();
	}
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

	// tft.setTextColor(0xFFFF, 0x0000);

	// menu:
	tft.fillScreen(0x0000);

	// Select ROM
	selectROM();

	// // Initialize emulator
	tft.fillScreen(0x0000);

	switch (game.console)
	{
	case (GBC):
		printCentered("Loading Gameboy Color");
		tft.println(game.filename);
		setupGBC(game.filename);
		// tft.fillScreen(0x0000);
		// printCentered("Loaded");
		// delay(1000);

		break;
	case (NES):
		printCentered("Loading NES");
		setupNES(game.filename);
		tft.fillScreen(0x0000);
		// printCentered("Loaded ", game.filename);
		delay(1000);

		break;
	default:
		printCentered("Unsupported file type");
		delay(2000);
		// goto menu;
		PANIC("Unsupported file type");
		break;
	}
	// setupNES("Super Mario Bros.nes");
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
	default:
		break;
	}
	// loopNES();
}