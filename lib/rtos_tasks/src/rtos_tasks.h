#include <global_variable.h>
void mqttSender(void *arguments)
{
  vTaskDelay(100 / portTICK_PERIOD_MS);
  const int mqtt_port = 1883;
  String broker = readString(MSTR2);
  broker.trim();
  char buf_broker[100];
  broker.toCharArray(buf_broker, broker.length() + 1);
  String topic = readString(MSTR3);
  client.setTimeout(3000);
  client.begin(buf_broker, net);
  // client.begin("broker.hivemq.com", net);
  while (1)
  {
    client.loop();

    if (!client.connected())
    {
      connect();
    }

    if (WiFi.status() == WL_DISCONNECTED)
    {
      Serial.println("{\"ERR\":\"WiFi Error\"}");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
#ifdef WiFi_WDG
      ESP.restart();
#endif
    }

    for (uint32_t i = 0; i < BANK_SIZE; i++)
    {
      if (buffer_ready[i] == 1 && client.connected())
      {
        publishBuffer(i);
        buffer_ready[i] = 0;
        vTaskDelay(1 / portTICK_PERIOD_MS);
      }
    }
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}

void serialHandler(void *arguments)
{
  while (1)
  {
    // Serial.println("loop serial handler");
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

void sensorReader(void *pvParameters)
{
  last_ts = 0;
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  sensor_wdg = millis();
  setAutorate(AKF_500_HZ);
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
    mv = mv / 4700 * (4700 + 10000);
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
  while (1)
  {
    // Serial.println("loop BATTERY");
    String payload = "";
    payload += "{";
    payload += "\"hardware_info\":{\n";
    payload += "\t\t\"battery_voltage\":" + String(v_batt) + ",\n";
    payload += "\t\t\"RSSI\":" + String(WiFi.RSSI()) + ",\n";
    payload += "\t\t\"free_memory\":" + String(esp_get_free_heap_size()) + ",\n";
    payload += "\t\t\"node_qos\":" + String(QoS) + ",\n";
    payload += "\t\t\"led_status\":" + String(led_status_mode) + "\n";
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
