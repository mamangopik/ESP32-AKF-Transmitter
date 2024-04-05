#include <global_variable.h>

void connect()
{
  String channel = String(WiFi.macAddress());
  Serial.print("\nconnecting...");
  connection_counter++;
  if (client.connect(channel.c_str(), "public", "public"))
  {
    led_status_mode = CONNECTED;
    Serial.println("\nconnected!");
  }
  vTaskDelay(10 / portTICK_PERIOD_MS);
}

void publishBuffer(uint32_t buffer_loc)
{
  sensor_topic.trim();
  char buf_sensor_topic[100];
  sensor_topic.toCharArray(buf_sensor_topic, sensor_topic.length() + 1);

  String payload = jsonify(buffer_loc);

  if (client.publish(buf_sensor_topic, payload.c_str(), payload.length(), false, QoS))
  {
    Serial.println("{\"SUCCESS\":\"Message sent\"}");
  }
  else
  {
    Serial.println("{\"ERR\":\"Message doesn't sent\"}");
    log_to_sd(payload);
  }
}

void log_to_sd(String data)
{
  if (sd_attached == 1 && sd_configured == 1)
  {
    fs::FS &fs = SD_MMC;
    // Path where new picture will be saved in SD Card
    String path = "/BRAINS_LOG_" + String(99) + ".csv";
    File file = fs.open(path.c_str(), FILE_APPEND);
    if (!file)
    {
      Serial.println("Failed to open file in writing mode");
    }
    else
    {
      String buffer = String("12/12/2024;" + data + "\n");
      file.print(buffer.c_str());
    }
    file.close();
    Serial.println("{\"SUCCESS\":\"Data Logged to SD Card\"}");
  }
  else
  {
    Serial.println("{\"ERR\":\"SD card not attached or not configured\"}");
  }
}

String jsonify(uint32_t buffer_loc)
{
  int rawValue = analogRead(VSENSE_PIN);
  v_batt = 0.4 + (3.3 / 4095.0) * rawValue * ((10000 + 10000) / 10000);

  String json_data = "{";

#if !defined USING_PSRAM
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
#endif
#if defined USING_PSRAM
  if (psram_ready == 1)
  {
    // copying from DRAM to SPI RAM or PSRAM
    for (int i = 0; i < DATA_SIZE; i++)
    {
      write_x(psram_buff_loc, i, x_values[buffer_loc][i]);
      write_y(psram_buff_loc, i, y_values[buffer_loc][i]);
      write_z(psram_buff_loc, i, z_values[buffer_loc][i]);
    }

    // read data from SPI RAM or PSRAM
    json_data += "\"x_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      json_data += String(read_x(psram_buff_loc, i));
      if (i != DATA_SIZE - 1)
      {
        json_data += ",";
      }
    }
    json_data += "],\"y_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      json_data += String(read_y(psram_buff_loc, i));
      if (i != DATA_SIZE - 1)
      {
        json_data += ",";
      }
    }
    json_data += "],\"z_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      json_data += String(read_z(psram_buff_loc, i));
      if (i != DATA_SIZE - 1)
      {
        json_data += ",";
      }
    }

    // address rolling mechanism for SPI RAM or PSRAM
    psram_buff_loc++;
    if (psram_buff_loc == psram_bank_size)
    {
      psram_buff_loc = 0;
    }
  }
  else
  {
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
  }
#endif
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
  json_data += ",\"sampling_frequency\":" + String(freq_sampling);
  json_data += ",\"packet_size\":" + String(DATA_SIZE);
  json_data += "}";

  return json_data;
}