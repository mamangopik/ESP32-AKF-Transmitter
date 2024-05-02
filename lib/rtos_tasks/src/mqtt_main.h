#include <global_variable.h>
void mqttSender(void *arguments)
{
    vTaskDelay(100 / portTICK_PERIOD_MS);
    const int mqtt_port = 1883;
    String broker = readString(MSTR2);
    broker.trim();
    char buf_broker[100];
    broker.toCharArray(buf_broker, broker.length() + 1);
    String topic = readString(MSTR3);
    client.setTimeout(TCP_Timeout);
    client.begin(buf_broker, net);
    // client.begin("broker.hivemq.com", net);
    while (1)
    {
        client.loop();

        if (!client.connected())
        {
            connect();
        }

#if defined board_v1 || !defined USING_PSRAM
        for (uint32_t i = 0; i < BANK_SIZE; i++)
        {
            if (buffer_ready[i] == 1 && client.connected())
            {
                publishBuffer(i);
                buffer_ready[i] = 0;
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
        }
#endif
#if defined USING_PSRAM && defined board_v2
        // if psram not configured
        if (psram_ready != 1)
        {
            for (uint32_t i = 0; i < BANK_SIZE; i++)
            {
                if (buffer_ready[i] == 1 && client.connected())
                {
                    publishBuffer(i);
                    buffer_ready[i] = 0;
                    vTaskDelay(1 / portTICK_PERIOD_MS);
                }
            }
        }
        else // if psram configured, then copy buffered sensor data to psram
        {
            for (uint32_t i = 0; i < BANK_SIZE; i++)
            {
                if (buffer_ready[i])
                {
                    // copying from DRAM to SPI RAM or PSRAM
                    for (int j = 0; j < DATA_SIZE; j++)
                    {
                        write_x(psram_mon, j, x_values[i][j]);
                        write_y(psram_mon, j, y_values[i][j]);
                        write_z(psram_mon, j, z_values[i][j]);
                    }
                    // Serial.println("{\"INFO\":\"copy from buffer at " + String(i) + " to psram at " + String(psram_mon) + " \"}");
                    psram_buffer_ready[psram_mon] = 1;
                    psram_mon++;
                    if (psram_mon == psram_bank_size)
                    {
                        psram_mon = 0;
                    }
                    buffer_ready[i] = 0;
                }
            }

            for (uint32_t i = 0; i < psram_bank_size; i++) // sending stored sensor data stored on psram
            {
                if (psram_buffer_ready[i] == 1 && client.connected())
                {
                    publishBuffer(i);
                    psram_buffer_ready[i] = 0;
                    vTaskDelay(1 / portTICK_PERIOD_MS);
                }
            }
        }
#endif
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}