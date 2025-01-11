#include "src/globals.h"
#include "src/gnuboy/gbc.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("Retro Card");

    rc_system_init();

    tft.fillScreen(0x0000);

    setupGBC("Super Mario Bros Deluxe.gbc");

}

void loop() {
    loopGBC();
}