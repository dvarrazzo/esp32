/* Read from a Grove Galvanic Skin Response Sensor and write
 * the averaged value and Human Resistance on serial.
 */

#include "Arduino.h"

extern "C" void app_main()
{
    const int pin = CONFIG_GSR_IN_GPIO;

    initArduino();

    Serial.begin(CONFIG_MONITOR_BAUD);

    Serial.print("reading from pin ");
    Serial.println(pin);
    delay(1000);

    for (;;) {
        long val = 0;
        int hr;

        // Average reading
        for (int i = 0; i < 8; i++) {
            val += analogRead(pin);
            delay(5);
        }
        // Divide by 8 for an average
        val /= 8;

        // Formula from:
        // http://wiki.seeedstudio.com/Grove-GSR_Sensor/
        // Divide by 4 because the output is described in arduino as 0-1023
        // but what measured in the 'pot' sketch is that output for
        // esp32 is 0-4195
        val /= 4;
        hr = ((1024 + 2 * val) * 10000) / (512 - val);

        Serial.print("val: ");
        Serial.print(val);
        Serial.print(" hr (ohm): ");
        Serial.println(hr);

        delay(200);
    }
}
