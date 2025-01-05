#include "../globals.h"

#define LCD_ENABLED true
#define SOUND_ENABLED false

#define DEBUG true

#include <driver/i2s.h>

//--------------------------------------------------------------------------------
// SPI FLASH MEMORY ACCESS:
#include "esp_spi_flash.h"

#define SPI_FLASH_ADDRESS 0x00300000 // 0x00300000 working (empty flash area)
#define SPI_FLASH_SECTOR_SIZE 0x1000 // 4kB = better not change

uint32_t FILE_ROM_SIZE = 0;
uint32_t FLASH_ROM_SIZE = 0;

// constant data pointer for direct access
const void *ROM;

spi_flash_mmap_handle_t handle1;

uint8_t flashdata[4096] = {0}; // 4kB buffer

//--------------------------------------------------------------------------------
// DRAM MEMORY ACCESS:
//--------------------------------------------------------------------------------

char *ROMFILENAME;      // NES load File name
unsigned char *rom = 0; // Actual ROM pointer

//********************************************************************************
// Allocated MEMORY variables:

uint8_t *OLDSCREENMEMORY[256]; // 256*256 bytes
uint8_t *SCREENMEMORY[256];    // 256*256 bytes

int started = 0;

const uint16_t nes_16bit[64] = {
    0x7BEF,
    0x001F,
    0x0017,
    0x4157,
    0x9010,
    0xA804,
    0xA880,
    0x88A0,
    0x5180,
    0x03C0,
    0x0340,
    0x02C0,
    0x020B,
    0x0000,
    0x0000,
    0x0000,
    0xBDF7,
    0x03DF,
    0x02DF,
    0x6A3F,
    0xD819,
    0xE00B,
    0xF9C0,
    0xE2E2,
    0xABE0,
    0x05C0,
    0x0540,
    0x0548,
    0x0451,
    0x0000,
    0x0000,
    0x0000,
    0xFFDF,
    0x3DFF,
    0x6C5F,
    0x9BDF,
    0xFBDF,
    0xFAD3,
    0xFBCB,
    0xFD08,
    0xFDC0,
    0xBFC3,
    0x5ECA,
    0x5FD3,
    0x075B,
    0x7BCF,
    0x0000,
    0x0000,
    0xFFFF,
    0xA73F,
    0xBDDF,
    0xDDDF,
    0xFDDF,
    0xFD38,
    0xF696,
    0xFF15,
    0xFECF,
    0xDFCF,
    0xBFD7,
    0xBFDB,
    0x07FF,
    0xFEDF,
    0x0000,
    0x0000,
};
//********************************************************************************

//--------------------------------------------------------------------------------
// Define this if running on little-endian (x86) systems
#define HOST_LITTLE_ENDIAN //!!! important for Arduino
#define ZERO_LENGTH 0

// quell stupid compiler warnings
#define UNUSED(x) ((x) = (x))

// 16Bit Color set
//================================================================================
// AUDIO_SETU
#define DEFAULT_SAMPLERATE 24000
#define DEFAULT_FRAGSIZE 60 // max.256, default 240

//********************************************************************************
uint8_t NES_POWER = 1;
//********************************************************************************
QueueHandle_t vidQueue;
//********************************************************************************

/* read counters */
int pad0_readcount, pad1_readcount, ppad_readcount, ark_readcount;

void input_strobe(void)
{
    pad0_readcount = 0;
    pad1_readcount = 0;
    ppad_readcount = 0;
    ark_readcount = 0;
}

uint8_t get_pad0(void)
{
    get_keys();
    uint8_t value = 0;

    if (keys[KEY_START])
        value |= 8; // START
    if (keys[KEY_SELECT])
        value |= 4; // SELECT
    if (keys[KEY_A])
        value |= 1; // A
    if (keys[KEY_B])
        value |= 2; // B}
    if (keys[KEY_UP])
        value |= 16; // UP
    if (keys[KEY_DOWN])
        value |= 32; // DOWN
    if (keys[KEY_LEFT])
        value |= 64; // LEFT
    if (keys[KEY_RIGHT])
        value |= 128; // RIGHT

    return (((value >> pad0_readcount++) & 1));
}

void (*audio_callback_nes)(void *buffer, int length) = NULL;

TimerHandle_t timer_1;

IRAM_ATTR void MEMORY_STATUS()
{
    if (DEBUG)
    {
        Serial.println();
        Serial.println("--------------------------------");
        Serial.print("TOTAL HEAP: ");
        Serial.println(ESP.getHeapSize());
        Serial.print("FREE HEAP: ");
        Serial.println(ESP.getFreeHeap());
        Serial.print("heap_caps_get_free_size: ");
        Serial.println(heap_caps_get_free_size(MALLOC_CAP_8BIT));
        Serial.println("--------------------------------");
    }
}
//................................................................................
// AUDIO SETUP
#define PDM_GPIO_NUM A0
i2s_port_t m_i2s_port = I2S_NUM_0;

typedef struct sndinfo_s
{
    int sample_rate;
    int bps;
} sndinfo_t;

#if SOUND_ENABLED

intr_handle_t i2s_interrupt;

i2s_pin_config_t pin_config = {
    // no bck for PDM
    .bck_io_num = I2S_PIN_NO_CHANGE,
    // use a dummy pin for the LR clock - 45 or 46 is a good options
    // as these are normally not connected
    .ws_io_num = GPIO_NUM_45,
    // where should we send the PDM data
    .data_out_num = PDM_GPIO_NUM,
    // no data to read
    .data_in_num = I2S_PIN_NO_CHANGE};

i2s_config_t audio_cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_PDM),
    .sample_rate = DEFAULT_SAMPLERATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0};
#endif

int init_sound(void)
{
#if SOUND_ENABLED
    // i2s_driver_install(m_i2s_port, &audio_cfg, 0, NULL);
    // i2s_set_pin(m_i2s_port, &pin_config);
    // i2s_zero_dma_buffer(m_i2s_port);
    i2s_driver_install(m_i2s_port, &audio_cfg, 0, NULL);
    // set up the i2s pins
    i2s_set_pin(m_i2s_port, &pin_config);
    // clear the DMA buffers
    i2s_zero_dma_buffer(m_i2s_port);

    i2s_start(m_i2s_port);
#endif
    audio_callback_nes = NULL;
    return 0;
}

void osd_setsound(void (*playfunc)(void *buffer, int length))
{
    // Indicates we should call playfunc() to get more data.
    audio_callback_nes = playfunc;
}

void osd_getsoundinfo(sndinfo_t *info);
void osd_getsoundinfo(sndinfo_t *info)
{
    info->sample_rate = DEFAULT_SAMPLERATE;
    info->bps = 16;
}

//--------------------------------------------------------------------------------
#if SOUND_ENABLED
QueueHandle_t queue;
//  uint16_t audio_frame[DEFAULT_FRAGSIZE];
uint16_t audio_frame[2 * DEFAULT_FRAGSIZE];
#endif

void do_audio_frame()
{
    /// printf("do_audio_frame\n");

#if SOUND_ENABLED
    int left = DEFAULT_SAMPLERATE / NES_REFRESH_RATE;
    while (left)
    {
        int n = DEFAULT_FRAGSIZE;
        if (n > left)
            n = left;
        audio_callback_nes(audio_frame, n); // get more data

        // 16 bit mono -> 32-bit (16 bit r+l)
        for (int i = n - 1; i >= 0; i--)
        {
            //      audio_frame[i] = audio_frame[i] + 0x8000;
            ///      uint16_t a = (audio_frame[i] >> 3); //VEEERY BAD!
            uint16_t a = (audio_frame[i] >> 0);
            audio_frame[i * 2 + 1] = 0x8000 + a;
            audio_frame[i * 2] = 0x8000 - a;

            /// Serial.print(audio_frame[i]);
        }
        size_t i2s_bytes_write;
        i2s_write(I2S_NUM_0, (const char *)audio_frame, 2 * n, &i2s_bytes_write, portMAX_DELAY);
        left -= i2s_bytes_write / 2;
        // i2s_write(I2S_NUM_1, (const char *)audio_frame, 4 * n, &i2s_bytes_write, portMAX_DELAY);
        // left -= i2s_bytes_write / 4;
    }

    // PWM Audio
    pwm_audio_write(*audio_frame, DEFAULT_FRAGSIZE, 0, 0);
    pwm_audio_write((uint8_t *)audio_frame, 2 * DEFAULT_FRAGSIZE, 0, 0);

#endif
}
//--------------------------------------------------------------------------------

//================================================================================
static void lcd_write_frame(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height)
{
    long startTime = millis();

    tft.startWrite();
    for (int px = 0; px < 256; px++)
    {
        for (int py = 0; py < height / 2; py++)
        {
            int xc = px;
            int yc = py * 2;

            uint8_t color = SCREENMEMORY[yc][xc];
            if (started)
            {
                if (color == OLDSCREENMEMORY[yc][xc])
                    continue;
                OLDSCREENMEMORY[yc][xc] = color;
                // tft.writePixel(x, y, nes_16bit[(0x3F & ((uint8_t *)SCREENMEMORY[yc])[xc])]);
            }
            else
            {
                started = 1;
            }
            tft.writePixel(px, py, nes_16bit[(0x3F & ((uint8_t *)SCREENMEMORY[yc])[xc])]);
        }
    }
    tft.endWrite();
    if (DEBUG)
    {
        tft.setCursor(0, 0);
        tft.setTextSize(1);
        tft.setTextColor(0xFFFF, 0x0000);
        tft.print("FPS: ");
        tft.print(1000 / (millis() - startTime));
    }
}

static void videoTask(void *arg)
{
    while (1)
    {
        xQueueReceive(vidQueue, &SCREENMEMORY, portMAX_DELAY);
        lcd_write_frame(0, 0, 256, 240);
    }
}
//--------------------------------------------------------------------------------
int audiovideo_init() /// osd_init()
{
    // init_sound(); // START AUDIO

    vidQueue = xQueueCreate(1, sizeof(unsigned int *));

    // pwm_audio_config_t pac = {
    //     .gpio_num_left      = A0,
    //     .gpio_num_right     = -1,
    //     .ledc_channel_left  = LEDC_CHANNEL_0,
    //     .ledc_channel_right = LEDC_CHANNEL_1,
    //     .ledc_timer_sel     = LEDC_TIMER_0,
    //     .duty_resolution    = LEDC_TIMER_8_BIT,
    //     .ringbuf_len        = 1024 * 8,
    // };
    // pwm_audio_init(&pac);
    // pwm_audio_start();

    xTaskCreatePinnedToCore(&videoTask, "videoTask", 3072, NULL, 0, NULL, 0);

    if (DEBUG)
        Serial.println("videoTask Pinned To Core 0...");
    return 0;
}
//================================================================================
char loadmessage[64];
unsigned char *getromdata(char *ROMFILENAME_)
{
    char namebuffer[256];
    strncpy(namebuffer, (const char *)"/", sizeof(namebuffer));
    strncat(namebuffer, ROMFILENAME_, sizeof(namebuffer));

    Serial.print("FILE SIZE: ");
    Serial.println(fp.size());

    FILE_ROM_SIZE = fp.size();

    uint16_t BLOCKCOUNT = (FILE_ROM_SIZE + SPI_FLASH_SECTOR_SIZE) / SPI_FLASH_SECTOR_SIZE;
    uint16_t BLOCKSIZEPX = 240 / BLOCKCOUNT;
    Serial.print("BLOCKCOUNT: ");
    Serial.print(BLOCKCOUNT);
    Serial.print(" BLOCKSIZEPX: ");
    Serial.print(BLOCKSIZEPX);
    Serial.println();
    BLOCKSIZEPX++;

    // Read NES rom to SPI FLASH!
    uint32_t i = 0;
    for (i = 0; i < fp.size() + SPI_FLASH_SECTOR_SIZE; i++)
    {
        if (i > 0 && i % SPI_FLASH_SECTOR_SIZE == 0)
        {
            /// spi_flash_erase_sector(SPI_FLASH_ADDRESS/SPI_FLASH_SECTOR_SIZE + (i/SPI_FLASH_SECTOR_SIZE)-SPI_FLASH_SECTOR_SIZE);
            delayMicroseconds(300);
            spi_flash_erase_range(SPI_FLASH_ADDRESS + i - SPI_FLASH_SECTOR_SIZE, sizeof(flashdata));
            delayMicroseconds(300);
            spi_flash_write(SPI_FLASH_ADDRESS + i - SPI_FLASH_SECTOR_SIZE, flashdata, sizeof(flashdata));
            delayMicroseconds(300);
        }
        delayMicroseconds(50);
        if (fp.available())
            flashdata[i % SPI_FLASH_SECTOR_SIZE] = fp.read();
        delayMicroseconds(50);
    }
    fp.close();

    FLASH_ROM_SIZE = i; // Size of File and Offset Align

    if (DEBUG)
        Serial.print("FLASH SIZE: ");
    if (DEBUG)
        Serial.println(FLASH_ROM_SIZE);

    ROM = 0;

    /// if (handle1) spi_flash_munmap(handle1);

    printf("Mapping %x (+%x)\n", SPI_FLASH_ADDRESS, FLASH_ROM_SIZE);

    ESP_ERROR_CHECK(spi_flash_mmap(SPI_FLASH_ADDRESS, FLASH_ROM_SIZE, SPI_FLASH_MMAP_DATA, &ROM, &handle1));

    printf("mmap_res: handle=%d ptr=%p\n", handle1, ROM);

    Serial.println("[NES ROM MAPPED ON FLASH!]");

    return (unsigned char *)ROM;
}
//================================================================================

#include "nes.h"

// theese varaiables cannot be in while loop!
rominfo_t *rominfo;
unsigned char *romdata = 0;

void setupNES(char *ROMFILENAME)
{
    Serial.begin(115200);

    MEMORY_STATUS();

    //--------------------------------------------------------------------------------
    // VIDEO MEMORY ALLOCATION

    for (uint32_t tmp = 0; tmp < 240; tmp++)
    {
        SCREENMEMORY[tmp] = (uint8_t *)malloc(256);
        memset(SCREENMEMORY[tmp], 0, 256);

        OLDSCREENMEMORY[tmp] = (uint8_t *)malloc(256);
        memset(OLDSCREENMEMORY[tmp], 0, 256);
    }

    setupHardware();

    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------

    MEMORY_STATUS();

    audiovideo_init(); // Init VIDEO Task and AUDIO I2S

    NESmachine = nes_create();
    if (NULL == NESmachine)
    {
        if (DEBUG)
            Serial.println("Failed to create NES instance.");
        if (DEBUG)
            tft.println("Failed to create NES instance.");
        while (1)
        {
        } // FREEZE
    }

    // ************************************************************************************
    // ************************************************************************************

    // ROMFILENAME = NESMENU();
    // screenmemory_fillscreen(63); // black color
    if (DEBUG)
        Serial.print("SELECTED NES: ");
    if (DEBUG)
        Serial.println(ROMFILENAME);

    romdata = (unsigned char *)getromdata(ROMFILENAME);

    if (DEBUG)
        Serial.println("Inserting cartridge.");
    // if (DEBUG)
        // tft.println("Inserting cartridge.");

    //--------------------------------------------------------------------------------

    if (NULL == rominfo)
        rominfo = (rominfo_t *)malloc(sizeof(rominfo_t));
    if (NULL == rominfo)
        // goto rom_load_fail;
        PANIC("ROMINFO MALLOC FAILED")

    memset(rominfo, 0, sizeof(rominfo_t));

    // Get the header and stick it into rominfo struct
    if (rom_getheader(&romdata, rominfo))
        // goto rom_load_fail;
        PANIC("ROM_GETHEADER FAILED")

    // Make sure we really support the mapper
    if (false == mmc_peek(rominfo->mapper_number))
    {
        if (DEBUG)
        {
            Serial.print("Mapper not yet implemented:");
            Serial.println(rominfo->mapper_number);
            tft.print("Mapper not yet implemented:");
            tft.println(rominfo->mapper_number);
        }
        // goto rom_load_fail;
        PANIC("Mapper not yet implemented")
    }

    // iNES format doesn't tell us if we need SRAM, so we have to always allocate it -- bleh!* UNIF, TAKE ME AWAY!  AAAAAAAAAA!!!
    if (rom_allocsram(rominfo))
        PANIC("ROMALLOC FAILED")
    rom_loadtrainer(&romdata, rominfo);
    if (rom_loadrom(&romdata, rominfo))
        PANIC("ROMLOAD FAILED")

    NESmachine->rominfo = rominfo;

    //********************************************************************************

    if (DEBUG)
    {
        Serial.println("ROMLOAD SUCCESS.");
        // tft.println("ROMLOAD SUCCESS.");
        Serial.println("Inserting Cartridge...");
        // tft.println("Inserting Cartridge...");
    }

    // map cart's SRAM to CPU $6000-$7FFF
    if (NESmachine->rominfo->sram)
    {
        NESmachine->cpu->mem_page[6] = NESmachine->rominfo->sram;
        NESmachine->cpu->mem_page[7] = NESmachine->rominfo->sram + 0x1000;
    }

    // mapper
    NESmachine->mmc = mmc_create(NESmachine->rominfo);
    if (NULL == NESmachine->mmc)
        PANIC("Insert Failed")

    // if there's VRAM, let the PPU know
    if (NULL != NESmachine->rominfo->vram)
        NESmachine->ppu->vram_present = true;
    if (SOUND_ENABLED)
        apu_setext(NESmachine->apu, NESmachine->mmc->intf->sound_ext);

    Serial.println("Sound and ppu set.");
    // osd_setsound(NESmachine->apu->process);
    build_address_handlers(NESmachine);
    Serial.println("Address handlers set.");
    nes_setcontext(NESmachine);
    Serial.println("Resetting");

    nes_reset();
}

void loopNES()
{
    if (DEBUG)
    {
        Serial.println("Insert Cartridge OK.");
        // tft.println("Insert Cartridge OK.");
    }

    // START!
    while (NES_POWER == 1)
    { // NES EMULATION LOOP
        nes_renderframe(true);
    }

    delay(1000);
    NES_POWER = 1;
}
