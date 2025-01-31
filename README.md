# Retro Card
Emulate your favourite retro consoles on your ESP32 using the Arduino framework.

# Emulators
While I'm still improving these, the planned emulators for this project are
 - [x] Nintendo Entertainment System
 - [x] Gameboy (color)
 - [x] SMS
 - [x] SNES (slow)
 - [ ] Doom/PrBoom

**NOTE**: This project wouldn't be possible without the work of [ducalex](https://github.com/ducalex) and the other contributors to [`retro-go`](https://github.com/ducalex/retro-go). The core emulators are tweaked versions of the ones from that repository. This project has some modifications to slim down on memory usage, make it easier to use on your setup, and to make it all work with the popular Arduino framework.

# Porting
To use this with your own ESP32 setup, you mainly just have to modify the drivers in `src/retro-card` and corresponding files like `src/defines.h`. Currently, it uses a ST7789 SPI LCD, but it shouldn't be difficult to use your own display, especially if it works with `Adafruit_GFX`. For input, I'm using an MCP23008 GPIO Expander, but you can modify `rc_input.cpp` to use your own input forms.

# Requirements
 - An ESP32(S3) with at least 2MB of PSRAM (many ROMS are too big to fit into RAM)
 - Some form of input
 - At least a 240x240 screen (although you can technically scale down video with some modifications)

# To Do
Here are some features planned for this project
 - Audio support with internal DAC
 - Better scaling
 - ROM Art
