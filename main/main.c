#include <stdio.h>
#include "esp_io_extend.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_tft.h"
#define IMAGES_HEIGHT 194
#define IMAGES_WIDTH 259

void app_main(void)
{
    IIC_Init();
    PCA9557_Set_IO(0x00, 0x00);
    TFT_init();
    TFT_set_background_light(10);

    //TFT_draw_bitmap(0, 0, 320, 240, windmill_7963566_640);
 
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

