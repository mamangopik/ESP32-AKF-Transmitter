#include <header.h>

void setup() {
  esp_task_wdt_init(0xffffffff, false);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
#ifdef VSENSE_PIN
  Serial.println("{\"INFO\":\"Voltage sensor is available\"}");
  pinMode(VSENSE_PIN,INPUT);
  analogReadResolution(12); // 12-Bit res
#endif
#ifndef VSENSE_PIN
  Serial.println("{\"INFO\":\"Voltage sensor is not available\"}");
#endif
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  xTaskCreatePinnedToCore(
    serialHandler,   /* Task function. */
    "serial handler",     /* name of task. */
    2048,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &SERIAL_TASK,      /* Task handle to keep track of created task */
    0);          /* pin task to core 1 */


  xTaskCreatePinnedToCore(
    sensorReader,
    "sensor reader",
    2048,
    NULL,
    1,
    &SENSOR_TASK,
    1);

  xTaskCreatePinnedToCore(
    ledStatus,
    "led status",
    2048,
    NULL,
    2,
    &LED_TASK,
    0);

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
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() - wlan_timer > 10000) {
      Serial.println("{\"ERR\":\"WiFi Error\"}");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      ESP.restart();
    }
  }
  Serial.println("{\"SUCCESS\":\"Connected to WiFi\"}");
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  xTaskCreatePinnedToCore(
    mqttSender,
    "mqtt sender",
    2048,
    NULL,
    0,
    &MQTT_TASK,
    0);
}

void loop() {
}
