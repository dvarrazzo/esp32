#include "Arduino.h"

extern "C" void app_main()
{
    const int pin = CONFIG_POT_IN_GPIO;

    initArduino();

    Serial.begin(CONFIG_MONITOR_BAUD);

    Serial.print("reading from pin ");
    Serial.println(pin);
    delay(1000);

    for (;;) {
        int val = analogRead(pin);
        Serial.println(val);
        delay(500);
    }
}
