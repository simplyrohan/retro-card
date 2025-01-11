#pragma once

#include "rc_audio.h"
#include "rc_input.h"
#include "rc_filesys.h"
#include "rc_video.h"

/**
 * Initializes the Retro Card system (tft, audio, input, sd).
 * @returns true if all systems were initialized successfully, false otherwise.
 */
extern bool rc_system_init();