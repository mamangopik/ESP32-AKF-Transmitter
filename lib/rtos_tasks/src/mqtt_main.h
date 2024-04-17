#include <arduino.h>
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
    client.setTimeout(10000);
    client.begin(buf_broker, net);
    // client.begin("broker.hivemq.com", net);
    while (1)
    {
        client.loop();

        if (!client.connected())
        {
            connect();
        }

        if (WiFi.status() == WL_DISCONNECTED)
        {
            Serial.println("{\"ERR\":\"WiFi Error\"}");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
#ifdef WiFi_WDG
            ESP.restart();
#endif
        }

        for (uint32_t i = 0; i < BANK_SIZE; i++)
        {
            if (buffer_ready[i] == 1 && client.connected())
            {
                publishBuffer(i);
                buffer_ready[i] = 0;
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}