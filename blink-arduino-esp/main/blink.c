#include "Arduino.h"

void app_main()
{
    const int led = CONFIG_BLINK_GPIO;

    initArduino();
    pinMode(led, OUTPUT);

    for (;;) {
        digitalWrite(led, HIGH);
        delay(250);
        digitalWrite(led, LOW);
        delay(250);
    }
}
