#include "Arduino.h"

void app_main()
{
    const int led = 2;

    initArduino();
    pinMode(led, OUTPUT);

    for (;;) {
        digitalWrite(led, HIGH);
        delay(250);
        digitalWrite(led, LOW);
        delay(250);
    }
}
