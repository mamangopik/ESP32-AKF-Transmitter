#include <global_variable.h>
#if !defined _sd_utils_h_
#define _sd_utils_h_
void init_sd_card()
{
    if (!SD_MMC.begin())
    {
        Serial.println("SD Card Mount Failed");
        sd_configured = 0;
    }
    else
    {
        sd_configured = 1;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD Card attached");
        sd_attached = 0;
    }
    else
    {
        sd_attached = 1;
    }
}

bool sd_append_log(String filename, String data)
{
    if (sd_attached == 1 && sd_configured == 1)
    {
        fs::FS &fs = SD_MMC;
        File file = fs.open(filename.c_str(), FILE_APPEND);
        if (!file)
        {
            file.close();
            return 0;
        }
        else
        {
            file.print(data.c_str());
            file.close();
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

#endif