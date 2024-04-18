#include <global_variable.h>
#if !defined _wifi_utils_h_
#define _wifi_utils_h_
void initWifi(unsigned long timeout = 10000)
{
    wlan_timer = millis();
    String ssid = readString(MSTR0);
    String password = readString(MSTR1);
    sensor_topic = readString(MSTR3);

    ssid.trim();
    password.trim();
    char buf_SSID[100];
    char buf_PWD[100];
    ssid.toCharArray(buf_SSID, ssid.length() + 1);
    password.toCharArray(buf_PWD, password.length() + 1);

    WiFi.mode(WIFI_STA);
    WiFi.begin(buf_SSID, buf_PWD);
    while (WiFi.status() != WL_CONNECTED)
    {
        led_status_mode = CONNECTING_WIFI;
        delay(500);
        if (millis() - wlan_timer > timeout)
        {
            Serial.println("{\"ERR\":\"WiFi Error\"}");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            ESP.restart();
        }
    }
    Serial.println("{\"SUCCESS\":\"Connected to WiFi\"}");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
#endif