#include <arduino.h>
#include <global_variable.h>
void sensorReader(void *pvParameters)
{
    last_ts = 0;
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    sensor_wdg = millis();
    setAutorate(AKF_100_HZ);
    vTaskDelay(1000);
    for (;;)
    {
        // Serial.println("loop sensor");
        cekSensor();
        while (Serial2.available() > 0)
        {
            cekSensor();
            byte in = Serial2.read();
            if (in == 104 && found == 0)
            {
                found = 1;
                ts_timer = micros();
            }
            if (found == 1)
            {
                raw += String(in, HEX);
                command[counter] = in;
                counter += 1;
            }
            if (counter == 14)
            {
                processCommand();
            }
        }
    }
}