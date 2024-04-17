#include <global_variable.h>
#include "mqtt_main.h"
#include "sensor_main.h"
#include "hw_interface_main.h"

void init_RTOS()
{
  xTaskCreatePinnedToCore(sensorReader, "sensor reader", 4096, NULL, 7, &SENSOR_TASK, 0);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  delay(2000);
  xTaskCreatePinnedToCore(hardwareStatus, "hardware status", 2048, NULL, 0, &HWINFO_TASK, 1);
  Serial.println("{\"INFO\":\"Voltage sensor is available\"}");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  xTaskCreatePinnedToCore(batteryStatus, "battery status", 4096, NULL, 0, &BATTERY_TASK, 1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
#if defined USING_LED_STATUS
  xTaskCreatePinnedToCore(ledStatus, "led status", 2048, NULL, 1, &LED_TASK, 1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
  xTaskCreatePinnedToCore(actButton, "action button", 2048, NULL, 0, &BUTTON_TASK, 1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  xTaskCreatePinnedToCore(serialHandler, "serial handler", 2048, NULL, 2, &SERIAL_TASK, 1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  initWifi();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  xTaskCreatePinnedToCore(mqttSender, "mqtt sender", 8192, NULL, 7, &MQTT_TASK, 1);
}