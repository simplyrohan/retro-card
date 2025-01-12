#pragma once

#include "retro-card/rc_system.h"


#define PANIC(x)          \
	{                     \
		Serial.printf(x); \
		while (1)         \
			;             \
	}