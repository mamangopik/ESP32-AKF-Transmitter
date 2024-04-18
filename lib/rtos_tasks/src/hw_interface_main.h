#include <global_variable.h>
#if !defined _hw_interface_main_h_
#define _hw_interface_main_h_
void serialHandler(void *arguments)
{
    while (1)
    {
        while (Serial.available() > 1)
        {
            no_serial_in_wdg = millis();
            char char_in = Serial.read();
            if (char_in != '\n')
            {
                msg_in += char_in;
            }
            sensor_wdg = millis();
        }

        if (millis() - no_serial_in_wdg > 50 && msg_in.length() > 0)
        {
            parseSerial();
            msg_in = "";
            Serial.flush();
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void ledStatus(void *arguments)
{
    pinMode(LEDSTATUSPIN, OUTPUT);
    while (1)
    {
        // Serial.println("loop LED");
        switch (led_status_mode)
        {
        case CONNECTED:
            LED_STATUS_SET;
            vTaskDelay(10 / portTICK_PERIOD_MS);
            LED_STATUS_RESET;
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            break;
        case DISCONNECTED:
            LED_STATUS_TOGGLE;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;
        case LOWBATT:
            LED_STATUS_SET;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            LED_STATUS_RESET;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            LED_STATUS_SET;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            LED_STATUS_RESET;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            LED_STATUS_SET;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            LED_STATUS_RESET;
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            break;
        case CONNECTING_WIFI:
            LED_STATUS_TOGGLE;
            break;
        default:
            LED_STATUS_RESET;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void batteryStatus(void *arguments)
{
    Serial.println("{\"INFO\":\"Voltage sensor is available\"}");
    pinMode(VSENSE_PIN, INPUT);
    analogReadResolution(12); // 12-Bit res
    float sum = 0.0;
    byte i = 0;
    while (1)
    {
        // Serial.println("loop BATTERY");
        float mv = analogReadMilliVolts(VSENSE_PIN);
        mv = mv / 10000 * (10000 + 10000);
        sum += (mv / 1000);
        i++;
        if (i == 5000)
        {
            v_batt = sum / 5000.0;
            i = 0;
            sum = 0;
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void hardwareStatus(void *arguments)
{
    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (1)
            vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    if (!rtc.isrunning())
    {
        Serial.println("RTC is NOT running, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    while (1)
    {
        // Serial.println("loop BATTERY");
        DateTime now = rtc.now();
        String payload = "";
        payload += "{";
        payload += "\"hardware_info\":{\n";
        payload += "\t\t\"battery_voltage\":" + String(v_batt) + ",\n";
        payload += "\t\t\"RSSI\":" + String(WiFi.RSSI()) + ",\n";
        payload += "\t\t\"free_memory\":" + String(esp_get_free_heap_size()) + ",\n";
        payload += "\t\t\"node_qos\":" + String(QoS) + ",\n";
        payload += "\t\t\"led_status\":" + String(led_status_mode) + ",\n";
        payload += "\t\t\"hw_unix_time\":" + String(now.unixtime()) + ",\n";
        payload += "\t\t\"hw_time\":\"" +
                   String(now.year()) + "-" +
                   String(now.month()) + "-" +
                   String(now.day()) + "/" +
                   String(now.hour()) + ":" +
                   String(now.minute()) + ":" +
                   String(now.second()) +
                   "\"\n";
        payload += "\t}\n";
        payload += "}";
        Serial.println(payload);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void actButton(void *pvParameters)
{
    pinMode(ACTBUTTONPIN, INPUT_PULLUP);
    uint8_t last_state = 1;
    while (1)
    {
        if (ACTBUTTONPIN_PRESS && last_state == 1)
        {
            vTaskDelay(50 / portTICK_PERIOD_MS);
            last_state = !last_state;
        }
        if (ACTBUTTONPIN_RELEASE && last_state == 0)
        {
            vTaskDelay(50 / portTICK_PERIOD_MS);
            if (QoS < 2)
            {
                QoS++;
            }
            else
            {
                QoS = 0;
            }
            last_state = !last_state;
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}
#endif