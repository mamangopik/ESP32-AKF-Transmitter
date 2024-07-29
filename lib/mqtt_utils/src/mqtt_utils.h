#include <global_variable.h>
#if !defined _mqtt_utils_h_
#define _mqtt_utils_h_

void connect()
{
  led_status_mode = DISCONNECTED;
  String channel = String(WiFi.macAddress());
  Serial.print("connecting...");
  connection_counter++;
  if (client.connect(channel.c_str(), "public", "public"))
  {
    led_status_mode = CONNECTED;
    Serial.println("\nconnected!");
  }
  vTaskDelay(100 / portTICK_PERIOD_MS);
}

byte publishBuffer(uint32_t buffer_loc)
{
  sensor_topic.trim();
  char buf_sensor_topic[100];
  sensor_topic.toCharArray(buf_sensor_topic, sensor_topic.length() + 1);

  String payload = jsonify(buffer_loc);
  byte fail_cnt = 0;
  if (client.publish(buf_sensor_topic, payload.c_str(), payload.length(), false, QoS))
  {
    return 1;
  }
  else
  {
    log_to_sd(payload);
    return 0;
  }
}

void log_to_sd(String data)
{
  data_lost++;
  String datetimeString = "";
  datetimeString += String(year) + "-";
  datetimeString += String(month) + "-";
  datetimeString += String(date);
  data = datetimeString + ',' + data;
  String path = "/ACC_LOG_" + datetimeString + ".csv";
  if (sd_append_log(path, data))
  {
    Serial.println("{\"SUCCESS\":\"Data Logged to SD Card\"}");
  }
  else
  {
    Serial.println("{\"ERR\":\"SD card not attached or not configured\"}");
  }
}
#if !defined USING_PSRAM || defined board_v1
String jsonify(uint32_t buffer_loc)
{
  uint32_t id = 0;
  String json_data = "{";
  id = buffer_loc;
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
  id = packet_id;
  json_data += "],";
  json_data += "\"sensor_type\":";
  json_data += '"';
  json_data += "accelerometer";
  json_data += '"';
  json_data += ",\"battery_voltage\":";
  json_data += String(v_batt);
  json_data += ",\"signal_strength\":" + String(WiFi.RSSI());
  json_data += ",\"id_data\":" + String(id);
  json_data += ",\"connection_lost\":" + String(connection_counter);
  json_data += ",\"sampling_frequency\":" + String(freq_sampling);
  json_data += ",\"packet_size\":" + String(DATA_SIZE);
  json_data += ",\"wifi_tx_power\":" + String(WiFi.getTxPower());
  json_data += ",\"hw_unix_time\":" + String(unix_timestamp) + "\n";
  json_data += ",\"hw_time\":\"" +
               String(year) + "-" +
               String(month) + "-" +
               String(date) + "/" +
               String(hour) + ":" +
               String(minute) + ":" +
               String(second) +
               "\"\n";
  json_data += "}";
  return json_data;
}
#endif
#if defined USING_PSRAM && defined board_v2
String jsonify(uint32_t buffer_loc)
{
  uint32_t id = 0;
  String json_data = "{";
  if (psram_ready == 1)
  {
    // read data from SPI RAM or PSRAM
    json_data += "\"x_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      json_data += String(read_x(buffer_loc, i));
      if (i != DATA_SIZE - 1)
      {
        json_data += ",";
      }
    }
    json_data += "],\"y_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      json_data += String(read_y(buffer_loc, i));
      if (i != DATA_SIZE - 1)
      {
        json_data += ",";
      }
    }
    json_data += "],\"z_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      json_data += String(read_z(buffer_loc, i));
      if (i != DATA_SIZE - 1)
      {
        json_data += ",";
      }
    }

    json_data += "],";

#if defined USING_KALMAN_FILTER
    json_data += kalmanJSON(1, buffer_loc);
#endif

    id = buffer_loc;
  }
  else // if psram enabled but error was occured
  {
    id = buffer_loc;
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

#if defined USING_KALMAN_FILTER
    json_data += kalmanJSON(0, buffer_loc);
#endif
  }

  id = packet_id;
  // JSON footer
  json_data += "\"sensor_type\":";
  json_data += '"';
  json_data += "accelerometer";
  json_data += '"';
  json_data += ",\"battery_voltage\":";
  json_data += String(v_batt);
  json_data += ",\"signal_strength\":" + String(WiFi.RSSI());
  json_data += ",\"id_data\":" + String(id);
  json_data += ",\"connection_lost\":" + String(connection_counter);
  json_data += ",\"sampling_frequency\":" + String(freq_sampling);
  json_data += ",\"packet_size\":" + String(DATA_SIZE);
  json_data += ",\"wifi_tx_power\":" + String(WiFi.getTxPower());
  json_data += ",\"hw_unix_time\":" + String(unix_timestamp) + "\n";
  json_data += ",\"hw_time\":\"" +
               String(year) + "-" +
               String(month) + "-" +
               String(date) + "/" +
               String(hour) + ":" +
               String(minute) + ":" +
               String(second) +
               "\"\n";
  json_data += "}";
  return json_data;
}

String kalmanJSON(byte storage_PSRAM, uint32_t buff_loc)
{
  String payload = "";
  if (storage_PSRAM == 1)
  {
    payload += "\"xkf_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      payload += String(x_kalman((float)read_x(buff_loc, i)));
      if (i != DATA_SIZE - 1)
      {
        payload += ",";
      }
    }
    payload += "],\"ykf_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      payload += String(y_kalman((float)read_y(buff_loc, i)));
      if (i != DATA_SIZE - 1)
      {
        payload += ",";
      }
    }
    payload += "],\"zkf_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      payload += String(z_kalman((float)read_z(buff_loc, i)));
      if (i != DATA_SIZE - 1)
      {
        payload += ",";
      }
    }
  }
  else
  {
    payload += "\"xkf_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      payload += String(x_kalman((float)x_values[buff_loc][i]));
      if (i != DATA_SIZE - 1)
      {
        payload += ",";
      }
    }
    payload += "],\"ykf_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      payload += String(y_kalman((float)y_values[buff_loc][i]));
      if (i != DATA_SIZE - 1)
      {
        payload += ",";
      }
    }
    payload += "],\"zkf_values\":[";
    for (int i = 0; i < DATA_SIZE; i++)
    {
      payload += String(z_kalman((float)z_values[buff_loc][i]));
      if (i != DATA_SIZE - 1)
      {
        payload += ",";
      }
    }
  }
  payload += "],";
  return payload;
}

#endif // endif board selector or psram
#endif // eof