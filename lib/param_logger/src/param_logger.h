#include <global_variable.h>
#if !defined _param_logger_h_
#define _param_logger_h_
bool testing_param_logger()
{
    /*
    logging parameter
    QoS
    RSSI
    TCP Timeout
    Date Time
    Data Lost
    Connection Lost
    battery Voltage
    |date time|RSSI|connection counter|QoS|TCP Timeout|v batt|Data lost|
    */
    String path = "/testing_parameter_TA.csv";
    String data;
    data += String(year) + "-";
    data += String(month) + "-";
    data += String(date) + "/";
    data += String(hour) + ":";
    data += String(minute) + ":";
    data += String(second) + ",";
    data += String(WiFi.RSSI()) + ",";
    data += String(connection_counter) + ",";
    data += String(QoS) + ",";
    data += String(TCP_Timeout) + ",";
    data += String(v_batt) + ",";
    data += String(data_lost) + "\n";

    if (sd_append_log(path, data))
    {
        Serial.println("{\"SUCCESS\":\"Parameter Logged to SD Card\"}");
        return 1;
    }
    else
    {
        Serial.println("{\"ERR\":\"SD card not attached or not configured\"}");
        return 0;
    }
}
#endif