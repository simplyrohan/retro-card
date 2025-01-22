#include "rc_system.h"

bool rc_system_init() {
    if (!rc_filesys_init()) {
        Serial.printf("Failed to initialize filesys\n");
        return false;
    }

    if (!rc_audio_init()) {
        Serial.printf("Failed to initialize audio\n");
        return false;
    }

    if (!rc_input_init()) {
        Serial.printf("Failed to initialize input\n");
        return false;
    }

    if (!rc_video_init()) {
        Serial.printf("Failed to initialize video\n");
        return false;
    }

    return true;
}