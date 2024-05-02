// if define __global_variable__ is not defined
#ifndef __global_variable__
// define __global_variable__ to indicate module already called and prevend redefinition of global variables
#define __global_variable__

#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <MQTT.h>
#include <esp_task_wdt.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "RTClib.h"

// user typedef
#define USING_PSRAM
#define USING_MICRO_SD
#define USING_KALMAN_FILTER
#define USING_LED_STATUS
#define SENSOR_WDG
#define WiFi_WDG
// board version select
#define board_v2
// #define board_v1

#if !defined USING_PSRAM && defined board_v1
#define RXD2 16
#define TXD2 17
#endif

#if defined USING_PSRAM && defined board_v1
#define RXD2 26
#define TXD2 27
#endif

#if defined board_v2
#define RXD2 26
#define TXD2 27
#endif

#define EEPROM_SIZE 1024
#define MSTR0 100
#define MSTR1 200
#define MSTR2 300
#define MSTR3 400
#define MSTR4 500

#define AKF_5_HZ 1
#define AKF_10_HZ 2
#define AKF_25_HZ 3
#define AKF_50_HZ 4
#define AKF_100_HZ 5
#define AKF_200_HZ 6
#define AKF_500_HZ 7

#define PRIORITY_IDLE 0
#define PRIORITY_VERY_LOW 1
#define PRIORITY_LOW 2
#define PRIORITY_MEDIUM 3
#define PRIORITY_HIGH 4
#define PRIORITY_VERY_HIGH 5
#define PRIORITY_REALTIME 6

#define CPU_0 0
#define CPU_1 1

#if defined board_v2
#define LEDSTATUSPIN 25
#define ACTBUTTONPIN 0
#define VSENSE_PIN 35
#endif

#if defined board_v1
#define LEDSTATUSPIN 4
#define ACTBUTTONPIN 0
#define VSENSE_PIN 35
#endif

#define ACTBUTTONPIN_PRESS digitalRead(ACTBUTTONPIN) == 0
#define ACTBUTTONPIN_RELEASE digitalRead(ACTBUTTONPIN) == 1

#define DISCONNECTED 0
#define CONNECTED 1
#define LOWBATT 2
#define CONNECTING_WIFI 3

#define LED_STATUS_SET digitalWrite(LEDSTATUSPIN, 1)
#define LED_STATUS_RESET digitalWrite(LEDSTATUSPIN, 0)
#define LED_STATUS_TOGGLE digitalWrite(LEDSTATUSPIN, !digitalRead(LEDSTATUSPIN))

// global objects
TaskHandle_t SENSOR_TASK;
TaskHandle_t MQTT_TASK;
TaskHandle_t LED_TASK;
TaskHandle_t SERIAL_TASK;
TaskHandle_t BATTERY_TASK;
TaskHandle_t HWINFO_TASK;
TaskHandle_t BUTTON_TASK;
TaskHandle_t RTC_TIME_TASK;
TaskHandle_t PARAM_LOGGER_TASK;

WiFiClient net;
MQTTClient client(512, 512);

// global variables
uint8_t counter = 0;
uint8_t found = 0;
uint8_t command[14];
uint8_t header[4];
uint8_t checksum[1];
uint8_t led_status_mode = DISCONNECTED;
uint8_t QoS = 2;
uint8_t sd_attached = 0;
uint8_t sd_configured = 0;
uint8_t psram_ready = 0;

uint16_t freq_sampling = 50;

uint32_t buffer_mon = 0;
uint32_t psram_mon = 0;
uint32_t data_lost = 0;

uint32_t TCP_Timeout = 10000; // ms

String msg_in = "";
String sensor_topic = "";
String raw = "";

unsigned long unix_timestamp;
uint16_t year, year_sp;
uint8_t month, month_sp;
uint8_t date, date_sp;
uint8_t hour, hour_sp;
uint8_t minute, minute_sp;
uint8_t second, second_sp;
uint8_t set_rtc_flag = 0;

const int DATA_SIZE = 256;
const int BANK_SIZE = (int)(10000 / DATA_SIZE);
short x_values[BANK_SIZE][DATA_SIZE];
short y_values[BANK_SIZE][DATA_SIZE];
short z_values[BANK_SIZE][DATA_SIZE];

#if defined USING_MICRO_SD
#include "FS.h"
#include "SD_MMC.h"
void init_sd_card();
bool sd_append_log(String filename, String data);
#include <sd_utils.h>
#endif

#if defined USING_PSRAM
const uint32_t psram_bank_size = (uint32_t)(510000 / DATA_SIZE);
const uint32_t psram_packet_size = DATA_SIZE;
byte psram_buffer_ready[psram_bank_size] = {0};

uint32_t psram_buff_loc = 0;

short *spi_ram_x;
short *spi_ram_y;
short *spi_ram_z;

void psram_begin();
void write_x(uint16_t row, uint16_t col, int data);
void write_y(uint16_t row, uint16_t col, int data);
void write_z(uint16_t row, uint16_t col, int data);
short read_x(uint16_t row, uint16_t col);
short read_y(uint16_t row, uint16_t col);
short read_z(uint16_t row, uint16_t col);
#include <psram_utils.h>
#endif

#if defined USING_KALMAN_FILTER
float X_XT, Y_XT, Z_XT;
float X_PT = 100, Y_PT = 100, Z_PT = 100;
float X_KT, Y_KT, Z_KT;
float X_XT_prev, Y_XT_prev, Z_XT_prev;
float X_PT_prev, Y_PT_prev, Z_PT_prev;
float X_XT_update, Y_XT_update, Z_XT_update;
float X_PT_update, Y_PT_update, Z_PT_update;
float Q = 0.1;
float R = 1;
float x_kalman(float Xvalue);
float y_kalman(float Xvalue);
float z_kalman(float Xvalue);
#endif

int data_count = 0;
int last_ts = 0;

int buffer_ready[BANK_SIZE] = {0};

unsigned long start_time = 0;
unsigned long wlan_timer = 0;
unsigned long sensor_wdg = 0;
unsigned long ts_timer = 0;
unsigned long id_data = 0;
unsigned long no_serial_in_wdg = 0;
unsigned long connection_counter = 0;

float v_batt = 0.0;

String JSONssid;
String JSONpassword;
String JSONtopic;
String JSONbroker;

String JSONyear;
String JSONmonth;
String JSONday;
String JSONhour;
String JSONminute;
String JSONsecond;

// Function declarations
// RTOS tasks
void init_RTOS();
void mqttSender(void *arguments);
void serialHandler(void *arguments);
void ledStatus(void *arguments);
void sensorReader(void *pvParameters);
void rtc_clock(void *arguments);

// WiFi and MQTT-related functions
void initWifi(unsigned long timeout);
void cek_wifi();
void connect();
void publishBuffer(uint32_t buffer_loc);
void log_to_sd(String data);
String jsonify(uint32_t buffer_loc);
String kalmanJSON(byte storage_PSRAM, uint32_t buff_loc);

// Sensor and hardware control functions
void cekSensor();
void setAutorate(uint8_t data_rate);

// Data processing and parsing functions
void processCommand();
int createXRaw(uint8_t command[]);
int createYRaw(uint8_t command[]);
int createZRaw(uint8_t command[]);
int calculateValue(uint8_t nibbles[]);
unsigned char calculateChecksum(unsigned char command[], unsigned char length);

// EEPROM data read/write functions
void writeString(uint16_t address, String data);
String readString(uint16_t address);

// Serial communication and JSON parsing functions
void parseSerial();
void parseJsonData(const String &jsonData);
void parseTimeData(const String &jsonData);

bool testing_param_logger();

#include <json_parser.h>
#include <rtos_tasks.h>
#include <mqtt_utils.h>
#include <sensor_utils.h>
#include <serial_parser.h>
#include <eeprom_storage.h>
#include <wifi_utils.h>
#include <kalman_filter.h>
#include <param_logger.h>
#endif