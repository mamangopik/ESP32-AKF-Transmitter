#include <global_variable.h>
void setup()
{
    // board preparation
    esp_task_wdt_init(0xffffffff, false);
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
    EEPROM.begin(EEPROM_SIZE);
    Serial.begin(115200);
#if defined USING_PSRAM
    psram_begin();
#endif
#if defined USING_MICRO_SD
    init_sd_card();
#endif
    init_RTOS();
}

void loop()
{
    // notihing to do here because FreeRTOS Take over the loop job
}
