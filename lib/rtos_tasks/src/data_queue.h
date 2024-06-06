#include <global_variable.h>
void data_queue(void *param)
{
    while (1)
    {
#if defined USING_PSRAM && defined board_v2
        // if psram not configured ,for old board or psram error only doing os.sleep
        if (psram_ready != 1)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
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
                    // dump data to sd_card for unsent data
                    if (psram_buffer_ready[psram_mon] == 1 && !client.connected())
                    {
                        Serial.println("{\"ERR\":\"buffer overflow, dump data to micro SD\"}");
                        for (uint32_t k = 0; k < psram_bank_size; k++)
                        {
#if defined USING_MICRO_SD && defined board_v2
                            if (psram_buffer_ready[k] == 1)
                            {
                                String payload = jsonify(k);
                                log_to_sd(payload);
                                psram_buffer_ready[k] = 0;
                            }
#endif
#if !defined USING_MICRO_SD || defined board_v1
                            Serial.println("{\"ERR\":\"Message doesn't sent\"}");
#endif
                        }
                    }

                    buffer_ready[i] = 0;
                }
            }
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
#endif

// for old board or psram error only doing os.sleep
#if !defined USING_PSRAM || defined board_v1
        vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
    }
}