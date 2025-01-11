#include "rc_system.h"

bool rc_system_init() {
    if (!rc_audio_init()) {
        return false;
    }

    if (!rc_input_init()) {
        return false;
    }

    if (!rc_filesys_init()) {
        return false;
    }

    if (!rc_video_init()) {
        return false;
    }

    return true;
}