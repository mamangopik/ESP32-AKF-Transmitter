#include <global_variable.h>
#if !defined _sensor_utils_h_
#define _sensor_utils_h_

void cekSensor()
{
  if (millis() - sensor_wdg > 5000)
  {
    Serial.println("{\"ERR\":\"sensor not responding, trying to recall\"}");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
#ifdef SENSOR_WDG
    ESP.restart();
#endif
  }
}
void setAutorate(uint8_t data_rate)
{
  vTaskDelay(500 / portTICK_PERIOD_MS);
  Serial2.updateBaudRate(9600);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  byte command_changebaud[6] = {0x68, 0x05, 0x00, 0x0b, 0x05, 0xff};
  command_changebaud[5] = calculateChecksum(command_changebaud, 5);
  for (byte i = 0; i < 6; i++)
  {
    Serial2.write(command_changebaud[i]);
  }
  vTaskDelay(500 / portTICK_PERIOD_MS);
  Serial2.updateBaudRate(115200);
  vTaskDelay(500 / portTICK_PERIOD_MS);

  byte command_autorate[6] = {0x68, 0x05, 0x00, 0x0c, 0xff, 0xff};

  switch (data_rate)
  {
  case AKF_5_HZ:
    command_autorate[4] = 0x01;
    freq_sampling = 5;
    break;
  case AKF_10_HZ:
    command_autorate[4] = 0x02;
    freq_sampling = 10;
    break;
  case AKF_25_HZ:
    command_autorate[4] = 0x03;
    freq_sampling = 25;
    break;
  case AKF_50_HZ:
    command_autorate[4] = 0x04;
    freq_sampling = 50;
    break;
  case AKF_100_HZ:
    command_autorate[4] = 0x05;
    freq_sampling = 100;
    break;
  case AKF_200_HZ:
    command_autorate[4] = 0x06;
    freq_sampling = 200;
    break;
  case AKF_500_HZ:
    command_autorate[4] = 0x07;
    freq_sampling = 500;
    break;
  }
  command_autorate[5] = calculateChecksum(command_autorate, 5);
  for (byte i = 0; i < 6; i++)
  {
    Serial2.write(command_autorate[i]);
  }
}

void processCommand()
{
  last_ts = micros() - ts_timer;
  sensor_wdg = millis();
  int x_raw = createXRaw(command);
  int y_raw = createYRaw(command);
  int z_raw = createZRaw(command);
  checksum[0] = command[13];
  raw = "";
  counter = 0;
  found = 0;

  uint8_t x_nibbles[6];
  for (byte i = 0; i < 6; i++)
  {
    x_nibbles[i] = (x_raw >> (i * 4)) & 0x0F;
  }
  int x_value = calculateValue(x_nibbles);

  uint8_t y_nibbles[6];
  for (byte i = 0; i < 6; i++)
  {
    y_nibbles[i] = (y_raw >> (i * 4)) & 0x0F;
  }
  int y_value = calculateValue(y_nibbles);

  uint8_t z_nibbles[6];
  for (byte i = 0; i < 6; i++)
  {
    z_nibbles[i] = (z_raw >> (i * 4)) & 0x0F;
  }
  int z_value = calculateValue(z_nibbles);

  byte calculated_checksum = calculateChecksum(command, 13);
  if (calculated_checksum == checksum[0])
  {
    if (x_value >= -4000 && x_value <= 4000 && y_value >= -4000 && y_value <= 4000 && z_value >= -4000 && z_value <= 4000)
    {
      // Store data in arrays
      x_values[buffer_mon][data_count] = x_value;
      y_values[buffer_mon][data_count] = y_value;
      z_values[buffer_mon][data_count] = z_value;
      data_count++;
    }
  }
  if (data_count == DATA_SIZE)
  {
    buffer_ready[buffer_mon] = 1;
    buffer_mon++;
    if (id_data < 0xffffffff)
      id_data++; // increment packet ID
    else
      id_data = 0;
    if (buffer_mon == BANK_SIZE)
    {
      buffer_mon = 0;
    }
    data_count = 0; // Reset arrays
    unsigned long duration = millis() - start_time;
    String timedata = String(duration);
    start_time = millis(); // Start new time measurement
    // Serial.println("{\"INFO\":\"buffer_mon at " + String(buffer_mon) + " psram_mon at " + String(psram_mon) + " \"}");
  }
}

int createXRaw(byte command[])
{
  int x_raw = 0;
  for (byte i = 4; i <= 6; i++)
  {
    x_raw <<= 8;
    x_raw |= command[i];
  }
  return x_raw;
}

int createYRaw(byte command[])
{
  int y_raw = 0;
  for (byte i = 7; i <= 9; i++)
  {
    y_raw <<= 8;
    y_raw |= command[i];
  }
  return y_raw;
}

int createZRaw(byte command[])
{
  int z_raw = 0;
  for (byte i = 10; i <= 12; i++)
  {
    z_raw <<= 8;
    z_raw |= command[i];
  }
  return z_raw;
}

int calculateValue(uint8_t nibbles[])
{
  uint16_t x_sign = nibbles[5];
  uint16_t x_num = (nibbles[4] * 10) + nibbles[3];
  uint16_t x_prec = (nibbles[2] * 100) + (nibbles[1] * 10) + nibbles[0];

  int value = (x_num * 1000) + (x_prec);
  if (x_sign > 0)
  {
    value *= -1;
  }

  return value;
}

unsigned char calculateChecksum(unsigned char command[], unsigned char length)
{
  unsigned char sum = 0;
  for (unsigned char i = 1; i < length; i++)
  {
    sum += command[i];
  }
  sum &= 0xFF; // Masking the sum to ensure it fits within a byte
  return sum;
}

#endif