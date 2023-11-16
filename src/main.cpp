#include "header.h"
void setup()
{
  esp_task_wdt_init(0xffffffff, false);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  Serial.println("{\"INFO\":\"Voltage sensor is available\"}");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  xTaskCreatePinnedToCore(
      batteryStatus,
      "battery status",
      2048,
      NULL,
      2,
      &BATTERY_TASK,
      1);

  xTaskCreatePinnedToCore(
      ledStatus,
      "led status",
      2048,
      NULL,
      3,
      &LED_TASK,
      0);
  xTaskCreatePinnedToCore(
      hardwareStatus,
      "hardware status",
      2048,
      NULL,
      4,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      serialHandler,    /* Task function. */
      "serial handler", /* name of task. */
      2048,             /* Stack size of task */
      NULL,             /* parameter of the task */
      1,                /* priority of the task */
      &SERIAL_TASK,     /* Task handle to keep track of created task */
      0);               /* pin task to core 1 */

  xTaskCreatePinnedToCore(
      sensorReader,
      "sensor reader",
      4096,
      NULL,
      1,
      &SENSOR_TASK,
      0);

  initWifi();

  xTaskCreatePinnedToCore(
      mqttSender,
      "mqtt sender",
      4096,
      NULL,
      0,
      &MQTT_TASK,
      1);
}

void loop()
{
  // notihing to do here because FreeRTOS Take over the loop job
}
