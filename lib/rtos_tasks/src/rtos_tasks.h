#include <global_variable.h>
void mqttSender(void *arguments) {
  vTaskDelay(100 / portTICK_PERIOD_MS);
  const int mqtt_port = 1883;
  String broker = readString(MSTR2);
  broker.trim();
  char buf_broker[100];
  broker.toCharArray(buf_broker, broker.length() + 1);
  String topic = readString(MSTR3);
  client.setTimeout(7000);
  client.begin(buf_broker, net);
  while (1) {
    client.loop();
    if (!client.connected()) {
      connect();
    }

    if (WiFi.status() == WL_DISCONNECTED) {
      Serial.println("{\"ERR\":\"WiFi Error\"}");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      ESP.restart();
    }   
    
    for(byte i=0;i<BANK_SIZE;i++){
      if (buffer_ready[i] == 1 && client.connected()) {
        publishBuffer(i);
        buffer_ready[i] = 0;
      }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void serialHandler(void *arguments) {
  while (1) {
    while (Serial.available() > 1) {
      no_serial_in_wdg = millis();
      char char_in = Serial.read();
      if (char_in != '\n') {
        msg_in += char_in;
      }
      sensor_wdg = millis();
    }

    if (millis() - no_serial_in_wdg > 50 && msg_in.length() > 0) {
      parseSerial();
      msg_in = "";
      Serial.flush();
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void ledStatus(void *arguments) {
  pinMode(LEDSTATUSPIN, OUTPUT);
  while (1) {
    if (!client.connected() || WiFi.status() == WL_DISCONNECTED) {
      digitalWrite(LEDSTATUSPIN, !digitalRead(LEDSTATUSPIN));
      vTaskDelay(100 / portTICK_PERIOD_MS);
    } else {
      digitalWrite(LEDSTATUSPIN, 1);
      vTaskDelay(10 / portTICK_PERIOD_MS);
      digitalWrite(LEDSTATUSPIN, 0);

      digitalWrite(LEDSTATUSPIN, 0);
      vTaskDelay(3000 / portTICK_PERIOD_MS);

    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");
  }

}

void sensorReader( void * pvParameters ) {
  last_ts = 0;
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  sensor_wdg = millis();
  setAutorate();
  for (;;) {
    cekSensor();
    while (Serial2.available() > 0) {
      cekSensor();
      byte in = Serial2.read();
      if (in == 104 && found == 0) {
        found = 1;
        ts_timer = micros();
      }
      if (found == 1) {
        raw += String(in, HEX);
        command[counter] = in;
        counter += 1;
      }
      if (counter == 14) {
        processCommand();
      }
    }
  }
}
