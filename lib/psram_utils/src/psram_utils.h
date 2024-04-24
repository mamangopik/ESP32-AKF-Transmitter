#include <global_variable.h>
#if !defined _psram_utils_h_
#define _psram_utils_h_
void psram_begin()
{
    if (psramInit())
    {
        spi_ram_x = (short *)ps_malloc(psram_bank_size * psram_packet_size * sizeof(short));
        spi_ram_y = (short *)ps_malloc(psram_bank_size * psram_packet_size * sizeof(short));
        spi_ram_z = (short *)ps_malloc(psram_bank_size * psram_packet_size * sizeof(short));

        if (spi_ram_x != nullptr && spi_ram_y != nullptr && spi_ram_z != nullptr)
        {
            Serial.println("PSRAM Allocalion Success");
            psram_ready = 1;
        }
        else
        {
            Serial.println("PSRAM Allocalion failed!");
            psram_ready = 0;
        }
    }
    else
    {
        Serial.println("PSRAM init failed!");
        psram_ready = 0;
    }
}

void write_x(uint16_t row, uint16_t col, int data)
{
    spi_ram_x[row * col] = data;
}
void write_y(uint16_t row, uint16_t col, int data)
{
    spi_ram_y[row * col] = data;
}
void write_z(uint16_t row, uint16_t col, int data)
{
    spi_ram_z[row * col] = data;
}

short read_x(uint16_t row, uint16_t col)
{
    return spi_ram_x[row * col];
}
short read_y(uint16_t row, uint16_t col)
{
    return spi_ram_y[row * col];
}
short read_z(uint16_t row, uint16_t col)
{
    return spi_ram_z[row * col];
}

#endif