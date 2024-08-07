#include <global_variable.h>
#if !defined _rtos_tasks_h_
#define _rtos_tasks_h_

#include "mqtt_main.h"
#include "sensor_main.h"
#include "hw_interface_main.h"
#include "data_queue.h"

void init_RTOS()
{
    xTaskCreatePinnedToCore(
        sensorReader,
        "sensor reader",
        4096,
        NULL,
        PRIORITY_REALTIME,
        &SENSOR_TASK,
        CPU_0);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(
        hardwareStatus,
        "hardware status",
        2048,
        NULL,
        PRIORITY_IDLE,
        &HWINFO_TASK,
        CPU_1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(
        batteryStatus,
        "battery status",
        2048,
        NULL,
        PRIORITY_VERY_LOW,
        &BATTERY_TASK,
        CPU_1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    // xTaskCreatePinnedToCore(
    //     parameter_logger,
    //     "testing parameter logger",
    //     4096,
    //     NULL,
    //     PRIORITY_VERY_LOW,
    //     &PARAM_LOGGER_TASK,
    //     CPU_1);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(
        rtc_clock,
        "time reader from RTC",
        2048,
        NULL,
        PRIORITY_VERY_HIGH,
        &RTC_TIME_TASK,
        CPU_1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
#if defined USING_LED_STATUS
    xTaskCreatePinnedToCore(
        ledStatus,
        "led status",
        1024,
        NULL,
        PRIORITY_LOW,
        &LED_TASK,
        CPU_1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
    xTaskCreatePinnedToCore(
        actButton,
        "action button",
        1024,
        NULL,
        PRIORITY_IDLE,
        &BUTTON_TASK,
        CPU_1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(
        serialHandler,
        "serial handler",
        2048,
        NULL,
        PRIORITY_MEDIUM,
        &SERIAL_TASK,
        CPU_1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(
        data_queue,
        "data queue",
        8192,
        NULL,
        PRIORITY_REALTIME,
        &QUEUE_TASK,
        CPU_1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    initWifi(10000);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(
        mqttSender,
        "mqtt sender",
        4096,
        NULL,
        PRIORITY_VERY_HIGH,
        &MQTT_TASK,
        CPU_1);
}

#endif