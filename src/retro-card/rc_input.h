#pragma once

enum keycode
{
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_A,
    KEY_B,
    KEY_START,
    KEY_SELECT
};

extern bool pad[8];
extern bool old_pad[8];

/**
 * @brief Initialize the input system
 * @returns true if the input system was initialized successfully, false otherwise
 */
extern bool rc_input_init();

/**
 * @brief Read the state of the buttons on the controller and store them in the pad array
 */
extern void rc_read_pad();