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
// for old board or psram is not used
#if defined board_v1 || !defined USING_PSRAM
        for (uint32_t i = 0; i < BANK_SIZE; i++)
        {
            if (packet_id >= 1024)
            {
                packet_id = 0;
            }
            if (buffer_ready[i] == 1 && client.connected())
            {
                publishBuffer(i);
                buffer_ready[i] = 0;
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
        }

#endif
// for new board and using psram
#if defined USING_PSRAM && defined board_v2
        // if psram error
        if (psram_ready != 1)
        {
            for (uint32_t i = 0; i < BANK_SIZE; i++)
            {
                if (packet_id >= 1024)
                {
                    packet_id = 0;
                }
                if (buffer_ready[i] == 1 && client.connected())
                {
                sending_no_psram:
                    if (publishBuffer(i))
                    {
                        packet_id++;
                        psram_buffer_ready[i] = 0;
                        Serial.println("{\"SUCCESS\":\"message sent!\"}");
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                    }
                    else
                    {
                        Serial.println("{\"WARN\":\"Resending message\"}");
                        connect();
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                        goto sending_no_psram;
                    }
                }
            }
        }
        // if psram OK
        else
        {
            for (uint32_t i = 0; i < psram_bank_size; i++) // sending stored sensor data that stored on psram
            {
                if (psram_buffer_ready[i] == 1 && client.connected())
                {
                sending:
                    if (publishBuffer(i))
                    {
                        packet_id++;
                        psram_buffer_ready[i] = 0;
                        Serial.println("{\"SUCCESS\":\"message sent!\"}");
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                    }
                    else
                    {
                        Serial.println("{\"WARN\":\"Resending message\"}");
                        connect();
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                        goto sending;
                    }
                }
            }
        }
#endif
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}