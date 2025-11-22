#include <stdio.h>
#include "esp_io_extend.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_tft.h"

void test_madctl_values(void)
{
    uint8_t madctl_values[] = {0x00, 0x08, 0x40, 0x48, 0x80, 0x88, 0xC0, 0xC8};
    const char* names[] = {"0x00", "0x08", "0x40", "0x48", "0x80", "0x88", "0xC0", "0xC8"};
    
    for(int i = 0; i < 8; i++) {
        // 设置MADCTL
        st7789_SendCMD(0x36);
        st7789_SendData(&madctl_values[i], 1);
        
        ESP_LOGI("TEST", "测试MADCTL = %s", names[i]);
        
        // 红色
        TFT_fill_screen(0, 0, 79, 319, 0xF800);
        // 绿色
        TFT_fill_screen(80, 0, 159, 319, 0x07E0);
        // 蓝色
        TFT_fill_screen(160, 0, 239, 319, 0x001F);
        
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}


void app_main(void)
{
    IIC_Init();
    PCA9557_Set_IO(0x00, 0x00);
    TFT_init();
    TFT_set_backlight(0);
    

    
    // 测试横屏分区显示
    TFT_fill_screen(0, 0, 159, 239, 0xF800);    // 左半白色
    TFT_fill_screen(160, 0, 319, 239, 0x0000);  // 右半黑色
}

