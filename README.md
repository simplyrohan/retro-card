# Retro Card
Emulate your favourite retro consoles on your ESP32 using the Arduino framework.

This project uses an ESP32 Pico, a 240x240 LCD, and some buttons to run emulators for the NES, Gameboy Color, Sega Master System, and Super Nintendo. This project is super easy to use, being built on the popular Arduino framework. Just load your ROMs on the SD card, pop it in and voila, retro games on the go. This project is still in its early stages, I'm planning on building a fully featured and affordable mini console for tinkering and gaming. But for now you can have a lot of fun emulating games on your ESP32.

## Gameboy + Gameboy Color
https://github.com/user-attachments/assets/9c74dcf2-9fcb-494b-95bb-5f6e5f629e5e

## Nintendo Entertainment System
https://github.com/user-attachments/assets/02f1b89a-b6f0-4aea-8e59-171c23978e29

## Sega Master System
https://github.com/user-attachments/assets/b06c0b30-a72e-4a53-b9f5-9ecdc6743122

## Super Nintendo Entertainment System
https://github.com/user-attachments/assets/dd5dda0e-e0b9-41b5-8528-285d8d14556c



# Emulators
While I'm still improving these, the planned emulators for this project are
 - [x] Nintendo Entertainment System
 - [x] Gameboy (Color)
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
