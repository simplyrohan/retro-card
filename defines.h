
// Q0 -> Start
// Q1 -> RIGHT
// Q2 -> A
// Q4 -> DOWN
// Q5 -> B
// Q6 -> UP
// Q7 -> LEFT
#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_LEFT 2
#define KEY_RIGHT 3
#define KEY_A 4
#define KEY_B 5
#define KEY_START 6
#define KEY_SELECT 7

// #define GPIO_UP 1
// #define GPIO_DOWN 2
// #define GPIO_LEFT 4
// #define GPIO_RIGHT 5
// #define GPIO_A 6
// #define GPIO_B 7

// Pressed B got RIGHT
// Press down got left
// pressd right got B
// Pressed Up got A
// Pressed left got up
// Pressed A got down

// New gpio to match maps
#define GPIO_UP 6
#define GPIO_DOWN 4
#define GPIO_LEFT 1
#define GPIO_RIGHT 7
#define GPIO_A 2
#define GPIO_B 5

#define INPUT_MODE INPUT_PULLUP

#define TFT_CS RX
#define TFT_RST A3
#define TFT_DC A2
#define TFT_BL TX

#define SD_CS A1

// #define WHITE 0xFFFF
// #define BLACK 0x0000
// #define RED 0xF800
// #define GREEN 0x07E0
// #define BLUE 0x001F
// #define CYAN 0x07FF
// #define MAGENTA 0xF81F
// #define YELLOW 0xFFE0
// #define ORANGE 0xFD20
// #define PINK 0xF81F

// #define FPS 24