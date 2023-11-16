#include <global_variable.h>

void connect()
{
  String channel = String(WiFi.macAddress());
  Serial.print("\nconnecting...");
  connection_counter++;
  while (!client.connect(channel.c_str(), "public", "public"))
  {
    Serial.print(".");
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  led_status_mode = CONNECTED;
  Serial.println("\nconnected!");
}

void publishBuffer(byte buffer_loc)
{
  int rawValue = analogRead(VSENSE_PIN);
  v_batt = 0.4 + (3.3 / 4095.0) * rawValue * ((10000 + 4700) / 4700);

  String json_data = "{";
  json_data += "\"x_values\":[";
  for (int i = 0; i < DATA_SIZE; i++)
  {
    json_data += String(x_values[buffer_loc][i]);
    if (i != DATA_SIZE - 1)
    {
      json_data += ",";
    }
  }
  json_data += "],\"y_values\":[";
  for (int i = 0; i < DATA_SIZE; i++)
  {
    json_data += String(y_values[buffer_loc][i]);
    if (i != DATA_SIZE - 1)
    {
      json_data += ",";
    }
  }
  json_data += "],\"z_values\":[";
  for (int i = 0; i < DATA_SIZE; i++)
  {
    json_data += String(z_values[buffer_loc][i]);
    if (i != DATA_SIZE - 1)
    {
      json_data += ",";
    }
  }
  json_data += "],";
  json_data += "\"sensor_type\":";
  json_data += '"';
  json_data += "accelerometer";
  json_data += '"';
  json_data += ",\"battery_voltage\":";
  json_data += String(v_batt);
  json_data += ",\"signal_strength\":" + String(WiFi.RSSI());
  json_data += ",\"id_data\":" + String(id_data);
  json_data += ",\"connection_lost\":" + String(connection_counter);
  json_data += "}";

  sensor_topic.trim();
  char buf_sensor_topic[100];
  sensor_topic.toCharArray(buf_sensor_topic, sensor_topic.length() + 1);

  if (client.publish(buf_sensor_topic, json_data.c_str(), json_data.length(), false, QoS))
  {
    Serial.println("{\"SUCCESS\":\"Message sent\"}");
  }
  else
  {
    Serial.println("{\"ERR\":\"Message doesn't sent\"}");
  }
}
