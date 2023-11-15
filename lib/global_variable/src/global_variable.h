//if define __global_variable__ is not defined
#ifndef __global_variable__ 
//define __global_variable__ to indicate module already called and prevend redefinition of global variables
#define __global_variable__ 

#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <MQTT.h>
#include <esp_task_wdt.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// user typedef
#define RXD2 16
#define TXD2 17
#define EEPROM_SIZE 1024
#define MSTR0 100
#define MSTR1 200
#define MSTR2 300
#define MSTR3 400
#define MSTR4 500

#define LEDSTATUSPIN 4
#define VSENSE_PIN 33
#define QoS 2
// global objects 
TaskHandle_t SENSOR_TASK;
TaskHandle_t MQTT_TASK;
TaskHandle_t LED_TASK;
TaskHandle_t SERIAL_TASK;
WiFiClient net;
MQTTClient client(256);

// global variables 
uint8_t counter = 0;
uint8_t found = 0;
uint8_t command[14];
uint8_t header[4];
uint8_t checksum[1];
unsigned int buffer_mon = 0;

String msg_in = "";
String sensor_topic = "";
String raw = "";

const int DATA_SIZE = 256;
const int BANK_SIZE = 50;

short x_values[BANK_SIZE][DATA_SIZE];
short y_values[BANK_SIZE][DATA_SIZE];
short z_values[BANK_SIZE][DATA_SIZE];

int data_count = 0;
int last_ts = 0;

uint8_t buffer_ready[BANK_SIZE]={0};

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

// Function declarations

//RTOS tasks
void mqttSender(void *arguments);
void serialHandler(void *arguments);
void ledStatus(void *arguments);
void sensorReader( void * pvParameters );

// WiFi and MQTT-related functions
void connect();
void publishBuffer(uint8_t buffer_loc);

// Sensor and hardware control functions
void cekSensor();
void setAutorate();

// Data processing and parsing functions
void processCommand();
int createXRaw(uint8_t command[]);
int createYRaw(uint8_t command[]);
int createZRaw(uint8_t command[]);
int calculateValue(uint8_t  nibbles[]);
unsigned char calculateChecksum(unsigned char command[], unsigned char length);

// EEPROM data read/write functions
void writeString(uint16_t address, String data);
String readString(uint16_t address);

// Serial communication and JSON parsing functions
void parseSerial();
void parseJsonData(const String &jsonData);
#endif