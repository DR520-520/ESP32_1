#include <stdio.h>
#include "esp_io_extend.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_tft.h"


void app_main(void)
{
    IIC_Init();
    PCA9557_Set_IO(0x00, 0x00);
    TFT_init();
    //TFT_set_backlight(0);
    

    
    // 测试横屏分区显示
    TFT_fill_screen(0, 0, 159, 239, 0xFFFF);    // 左半白色
    TFT_fill_screen(160, 0, 319, 239, 0x0000);  // 右半黑色
    vTaskDelay(2000);

    TFT_fill_screen(0, 0, 159, 239, 0xF202);    // 左半白色
    TFT_fill_screen(160, 0, 319, 239, 0x0F0F);  // 右半黑色

}

