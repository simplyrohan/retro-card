# Retro Card
Emulate your favourite retro consoles on your ESP32 using the Arduino framework.

This project uses an ESP32 Pico, a 240x240 LCD, and some buttons to run emulators for the NES, Gameboy Color, Sega Master System, and Super Nintendo. This project is super easy to use, built on the popular Arduino framework. Just load your ROMs on the SD card, pop it in and voila, retro games on the go. This project is still in its early stages, I'm planning on building a fully featured and affordable mini console for tinkering and gaming. But for now you can have a lot of fun emulating games on your ESP32.

# Emulators
 - [x] Nintendo Entertainment System
 - [x] Gameboy (Color)
 - [x] SMS
 - [x] SNES (slow)

# Porting
To use this with your own ESP32 setup, you mainly just have to modify the drivers in `src/retro-card` and corresponding files like `src/defines.h`. Currently, it uses a ST7789 SPI LCD, but it shouldn't be difficult to use your own display, especially if it works with `Adafruit_GFX`. For input, I'm using an MCP23008 GPIO Expander, but you can modify `rc_input.cpp` to use your own input forms.

# Requirements
 - An ESP32(S3) with at least 2MB of PSRAM (many ROMS are too big to fit into RAM)
 - Some form of input
 - At least a 240x240 screen (although you can technically scale down video with some modifications)
