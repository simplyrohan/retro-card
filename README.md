# Retro Card
A tiny console to run all your favorite retro consoles

# Emulators
While I'm still implementing these, the planned emulators for this project are
 - [x] NES (it sucks though)
 - [x] GBC/GB
 - [x] SMS
 - [ ] PrBoom

> [!IMPORTANT]  
> These are notes for myself and keeping track of this project
# To Do
 - [ ] Use real nofrendo
    - NES Emulator is based on NESCAT but because of me chopping it up and the one-file source code, the code is horrendous right now
 - [ ] Fix `printCentered` to support multiline

 # Console Info
 Technical specs and other info of consoles for reference
 ### NES
Resolution: 256x240
Memory: 2 KB Work RAM + 2 KB Video RAM + 256 bytes sprite RAM
ROM: 
Controller: 2 ports; D-Pad, A/B, Start/Select
Emulator of choice: nofrendo

### Gameboy (Color)
Resolution: 160x144
Memory:  2 KB ROM, 127 B High RAM, 16 KB Video RAM, 32 KB RAM
ROM: Max 8 MB ROM, plus 128 KB RAM