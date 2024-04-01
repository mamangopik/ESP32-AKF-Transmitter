#include "header.h"
void setup()
{
    esp_task_wdt_init(0xffffffff, false);
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
    EEPROM.begin(EEPROM_SIZE);
    Serial.begin(115200);

#ifdef USING_MICRO_SD
    if (!SD_MMC.begin())
    {
        Serial.println("SD Card Mount Failed");
        sd_configured = 0;
    }
    else
    {
        sd_configured = 1;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD Card attached");
        sd_attached = 0;
    }
    else
    {
        sd_attached = 1;
    }
#endif
    xTaskCreatePinnedToCore(sensorReader, "sensor reader", 4096, NULL, 7, &SENSOR_TASK, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    delay(2000);
    xTaskCreatePinnedToCore(hardwareStatus, "hardware status", 2048, NULL, 0, &HWINFO_TASK, 1);
    Serial.println("{\"INFO\":\"Voltage sensor is available\"}");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(batteryStatus, "battery status", 4096, NULL, 0, &BATTERY_TASK, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
#ifdef USING_LED_STATUS
    xTaskCreatePinnedToCore(ledStatus, "led status", 2048, NULL, 1, &LED_TASK, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
    xTaskCreatePinnedToCore(actButton, "action button", 2048, NULL, 0, &BUTTON_TASK, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(serialHandler, "serial handler", 2048, NULL, 2, &SERIAL_TASK, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    initWifi();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(mqttSender, "mqtt sender", 4096, NULL, 7, &MQTT_TASK, 1);
}

void loop()
{
// notihing to do here because FreeRTOS Take over the loop job
#ifdef USING_PSRAM
    Serial.println("Free PSRAM:" + String(ESP.getFreePsram()));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
}
