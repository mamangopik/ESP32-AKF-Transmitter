#include <global_variable.h>
#if !defined _mqtt_utils_h_
#define _mqtt_utils_h_

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
  byte fail_cnt = 0;
  while (fail_cnt < 10)
  {
    if (fail_cnt > 0)
    {
      Serial.println("{\"WARN\":\"retrying to sending message\"}");
    }
    if (client.publish(buf_sensor_topic, payload.c_str(), payload.length(), false, QoS))
    {
      Serial.println("{\"SUCCESS\":\"Message sent\"}");
      fail_cnt = 0; // reset fail_cnt counter to prevent SD card Logging
      break;        // break the loop so data only sent once
    }
    else
    {
      fail_cnt++; // increment fail_cnt counter if failed sending message
    }
  }
  if (fail_cnt > 0)
  {
#if defined USING_MICRO_SD && defined board_v2
    Serial.println("{\"WARN\":\"Message doesn't sent\"}");
    log_to_sd(payload);
#endif

#if !defined USING_MICRO_SD || defined board_v1
    Serial.println("{\"ERR\":\"Message doesn't sent\"}");
#endif
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

#endif

// String jsonify(uint32_t buffer_loc)
// {
//   uint32_t id = 0;

//   String json_data = "{";

// #if !defined USING_PSRAM || defined board_v1
//   id = buffer_loc;
//   json_data += "\"x_values\":[";
//   for (int i = 0; i < DATA_SIZE; i++)
//   {
//     json_data += String(x_values[buffer_loc][i]);
//     if (i != DATA_SIZE - 1)
//     {
//       json_data += ",";
//     }
//   }
//   json_data += "],\"y_values\":[";
//   for (int i = 0; i < DATA_SIZE; i++)
//   {
//     json_data += String(y_values[buffer_loc][i]);
//     if (i != DATA_SIZE - 1)
//     {
//       json_data += ",";
//     }
//   }
//   json_data += "],\"z_values\":[";
//   for (int i = 0; i < DATA_SIZE; i++)
//   {
//     json_data += String(z_values[buffer_loc][i]);
//     if (i != DATA_SIZE - 1)
//     {
//       json_data += ",";
//     }
//   }
//   json_data += "],";
// #if defined USING_KALMAN_FILTER
// #endif
// #if defined USING_PSRAM && defined board_v2
//   if (psram_ready == 1)
//   {
//     // copying from DRAM to SPI RAM or PSRAM
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       write_x(psram_buff_loc, i, x_values[buffer_loc][i]);
//       write_y(psram_buff_loc, i, y_values[buffer_loc][i]);
//       write_z(psram_buff_loc, i, z_values[buffer_loc][i]);
//     }

//     // read data from SPI RAM or PSRAM
//     json_data += "\"x_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(read_x(psram_buff_loc, i));
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
//     json_data += "],\"y_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(read_y(psram_buff_loc, i));
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
//     json_data += "],\"z_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(read_z(psram_buff_loc, i));
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }

//     json_data += "],";
// #if defined USING_KALMAN_FILTER
//     json_data += "],";
//     json_data += "\"xkf_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(x_kalman(read_x(psram_buff_loc, i)));
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
//     json_data += "],\"ykf_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(y_kalman(read_y(psram_buff_loc, i)));
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
//     json_data += "],\"zkf_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(z_kalman(read_z(psram_buff_loc, i)));
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
// #endif

//     id = psram_buff_loc;

//     // address rolling mechanism for SPI RAM or PSRAM
//     psram_buff_loc++;
//     if (psram_buff_loc == psram_bank_size)
//     {
//       psram_buff_loc = 0;
//     }
//   }
//   else // if psram enabled but error was occured
//   {
//     id = buffer_loc;
//     json_data += "\"x_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(x_values[buffer_loc][i]);
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
//     json_data += "],\"y_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(y_values[buffer_loc][i]);
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
//     json_data += "],\"z_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(z_values[buffer_loc][i]);
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
//     json_data += "],";
// #if defined USING_KALMAN_FILTER
//     json_data += "],";
//     json_data += "\"xkf_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(x_kalman(read_x(psram_buff_loc, i)));
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
//     json_data += "],\"ykf_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(y_kalman(read_y(psram_buff_loc, i)));
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
//     json_data += "],\"zkf_values\":[";
//     for (int i = 0; i < DATA_SIZE; i++)
//     {
//       json_data += String(z_kalman(read_z(psram_buff_loc, i)));
//       if (i != DATA_SIZE - 1)
//       {
//         json_data += ",";
//       }
//     }
// #endif
//   }

//   // JSON footer
//   DateTime now = rtc.now();
//   json_data += "\"sensor_type\":";
//   json_data += '"';
//   json_data += "accelerometer";
//   json_data += '"';
//   json_data += ",\"battery_voltage\":";
//   json_data += String(v_batt);
//   json_data += ",\"signal_strength\":" + String(WiFi.RSSI());
//   json_data += ",\"id_data\":" + String(id);
//   json_data += ",\"connection_lost\":" + String(connection_counter);
//   json_data += ",\"sampling_frequency\":" + String(freq_sampling);
//   json_data += ",\"packet_size\":" + String(DATA_SIZE);
//   json_data += ",\"hw_unix_time\":" + String(now.unixtime()) + "\n";
//   json_data += ",\"hw_time\":\"" +
//                String(now.year()) + "-" +
//                String(now.month()) + "-" +
//                String(now.day()) + "/" +
//                String(now.hour()) + ":" +
//                String(now.minute()) + ":" +
//                String(now.second()) +
//                "\"\n";
//   json_data += "}";

//   return json_data;
// }
#endif