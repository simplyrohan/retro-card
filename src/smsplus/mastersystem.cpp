// Main code to run SMSPlus emulator with the Arduino IDE
#include "mastersystem.h"
#include "../globals.h"

// unsigned char *videoBuffer;
// unsign/ed char *oldVideoBuffer;

// unsigned char *backBuffer;

// uint16_t palette[256] = {0};

rc_framebuffer sms_framebuffer;

// --- MAIN
void setupSMS(char *romfilename)
{
    Serial.println("Setting up SMS");

    // ---- SMS Consfig
    system_reset_config();
    option_sms.sndrate = 16000;
    option_sms.overscan = 0;
    option_sms.extra_gg = 0;
    option_sms.tms_pal = 0;

    option_sms.console = 0; // SMS

    // ---- Video
    rc_create_framebuffer(&sms_framebuffer, SMS_WIDTH, SMS_HEIGHT, true);
    render_copy_palette(sms_framebuffer.palette);

    // ------------------------------------------
    // Load ROM file
    Serial.println(romfilename);
    rc_open_file(romfilename);

    size_t size;
    size = rc_get_file_size();
    Serial.println(size);

    void *data = ps_malloc(size);
    rc_read_file(data, size);

    rc_close_file();

    Serial.println("ROM Read");

    if (!load_rom(data, (int)size, size))
        PANIC("ROM file loading failed!");
    // ------------------------------------------

    // ------------------------------------------
    // ---- SMS Video Config
    bitmap_sms.width = SMS_WIDTH;
    bitmap_sms.height = SMS_HEIGHT;
    bitmap_sms.pitch = bitmap_sms.width;
    bitmap_sms.data = (unsigned char *)sms_framebuffer.buffer;

    // ---- SMS Start
    system_poweron();

    Serial.println("Setup done!");
}

void loopSMS()
{
    input.pad[0] = 0x00;
    input.pad[1] = 0x00;
    input.system = 0x00;

    rc_read_pad();
    if (pad[KEY_UP])
        input.pad[0] |= INPUT_UP;
    if (pad[KEY_DOWN])
        input.pad[0] |= INPUT_DOWN;
    if (pad[KEY_LEFT])
        input.pad[0] |= INPUT_LEFT;
    if (pad[KEY_RIGHT])
        input.pad[0] |= INPUT_RIGHT;
    if (pad[KEY_A])
        input.pad[0] |= INPUT_BUTTON2;
    if (pad[KEY_B])
        input.pad[0] |= INPUT_BUTTON1;
    if (pad[KEY_START])
        input.system |= INPUT_START;
    if (pad[KEY_SELECT])
        input.system |= INPUT_PAUSE;

    // system_frame(1); // 1 = don't draw frame
    system_frame(0); // 0 = draw frame

    render_copy_palette(sms_framebuffer.palette);

    // Add video buffer to queue
    rc_send_frame(&sms_framebuffer);

    delay(1000 / 80);
}