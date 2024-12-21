#include "src/gnuboy/gbc.h"
#include "src/globals.h"

void setup()
{
	Serial.begin(115200);
	delay(2000);
	Serial.println("Starting...");

	setupHardware();

	delay(500);

	// load rom from sd
	fp = SD.open("/gold.gbc", FILE_READ);
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

	setupGBC(data, size);
}

void loop()
{
	get_keys();
	long time = loopGBC();
}