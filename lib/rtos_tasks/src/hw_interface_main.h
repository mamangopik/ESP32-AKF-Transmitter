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
    double batt_Q = 0.1;      // Process noise covariance
    double batt_R = 0.1;      // Measurement noise covariance
    double batt_P = 0.1;      // Estimation error covariance
    double batt_K = 0.1;      // Kalman gain
    double batt_X;            // The state
    byte i = 0;
    while (1)
    {
        v_batt = analogReadMilliVolts(VSENSE_PIN);
        v_batt = v_batt / 10000 * (10000 + 10000);
        v_batt = v_batt / 1000;
        // Prediction update
        batt_P += batt_Q;

        // Measurement update
        batt_K = batt_P / (batt_P + batt_R);
        batt_X += batt_K * (v_batt - batt_X);
        batt_P *= (1 - batt_K);

        v_batt = batt_X;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void parameter_logger(void *arguments)
{
    while (1)
    {
        testing_param_logger();
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
}

void rtc_clock(void *arguments)
{
    RTC_DS1307 rtc;
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
    }
    while (1)
    {
        DateTime now = rtc.now();
        unix_timestamp = now.unixtime();
        year = now.year();
        month = now.month();
        date = now.day();
        hour = now.hour();
        minute = now.minute();
        second = now.second();
        if (set_rtc_flag == 1)
        {
            rtc.adjust(
                DateTime(JSONyear.toInt(),
                         JSONmonth.toInt(),
                         JSONday.toInt(),
                         JSONhour.toInt(),
                         JSONminute.toInt(),
                         JSONsecond.toInt()));
            set_rtc_flag = 0;
        };
        Serial.println("RTC set");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void hardwareStatus(void *arguments)
{

    while (1)
    {
        String payload = "";
        payload += "{";
        payload += "\"hardware_info\":{\n";
        payload += "\t\t\"battery_voltage\":" + String(v_batt) + ",\n";
        payload += "\t\t\"RSSI\":" + String(WiFi.RSSI()) + ",\n";
        payload += "\t\t\"free_memory\":" + String(esp_get_free_heap_size()) + ",\n";
        payload += "\t\t\"node_qos\":" + String(QoS) + ",\n";
        payload += "\t\t\"led_status\":" + String(led_status_mode) + ",\n";
        payload += "\t\t\"hw_unix_time\":" + String(unix_timestamp) + ",\n";
        payload += "\t\t\"hw_time\":\"" +
                   String(year) + "-" +
                   String(month) + "-" +
                   String(date) + "/" +
                   String(hour) + ":" +
                   String(minute) + ":" +
                   String(second) +
                   "\"\n";
        payload += "\t}\n";
        payload += "}";
        Serial.println(payload);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
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