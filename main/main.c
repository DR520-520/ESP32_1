#include <stdio.h>
#include "esp_io_extend.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_tft.h"


void app_main(void)
{
    uint8_t data = 0x3F;
    IIC_Init();
    st7789_hardware_init();
    st7789_SendCMD(&data);

    
}
