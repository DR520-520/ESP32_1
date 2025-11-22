#include "esp_tft.h"
spi_device_handle_t handle_spi_tft;
void st7789_hardware_init()
{
    
    /*---------GPIO初始化----------*/
    gpio_config_t gpio_config_struct = 
    {
        .pin_bit_mask = (1ULL << IO39_LCD_DC),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en =GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_config_struct));

    /*---------SPI初始化----------*/

    spi_bus_config_t bus_config = 
    {
     .mosi_io_num = IO40_LCD_DATA,
     .miso_io_num = -1,
     .sclk_io_num = IO41_LCD_SCLK,
     .quadhd_io_num = -1,
     .quadwp_io_num = -1,
     .max_transfer_sz = TFT_LENGTH * TFT_WIDTH * 2
    };
    ESP_ERROR_CHECK(spi_bus_initialize(TFT_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t dev_config = 
    {
        .mode = 0,
        .clock_speed_hz = 20 * 1000* 1000,
        .spics_io_num = -1,
        .queue_size = 7,
        .pre_cb = NULL
    };
    ESP_ERROR_CHECK(spi_bus_add_device(TFT_SPI_HOST, &dev_config, &handle_spi_tft));

    /*------------背光控制--------------*/
    ledc_timer_config_t timer_conf = 
    {
        .freq_hz = 10* 1000,
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    ledc_channel_config_t pwm_channel_config = 
    {
        .channel = LEDC_CHANNEL_0,
        .duty = 1023,
        .flags.output_invert = 1,
        .gpio_num = IO42_LCD_BL,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel_config));
}

void st7789_SendCMD(uint8_t *data)
{
    if(data == NULL)
    {
        ESP_LOGE("DEBUG", "cmd 是 NULL!");
        return;
    }
    LCD_CS_LOW();
    gpio_set_level(IO39_LCD_DC, 0);


    spi_transaction_t trans_desc = 
    {
        .length = 8,
        .tx_buffer = data,
    };

    ESP_ERROR_CHECK(spi_device_polling_transmit(handle_spi_tft, &trans_desc));
    LCD_CS_HIGH();
}

void st7789_SendData(uint8_t *data, uint8_t len)
{
    
    if(data == NULL)
    {
        ESP_LOGE("DEBUG", "data 是 NULL!");
        return;
    }

    if(len == 0)
    {
        ESP_LOGE("DEBUG", "data len 是 0!");
        return; 
    }
    LCD_CS_LOW();
    gpio_set_level(IO39_LCD_DC, 1);

    spi_transaction_t trans_desc = 
    {
        .length = 8 * len,
        .tx_buffer = data,
    };
    ESP_ERROR_CHECK(spi_device_polling_transmit(handle_spi_tft, &trans_desc));
    LCD_CS_HIGH();
}

void st7789_software_init(void)
{
    
    ESP_LOGI("TFT", "开始软件初始化");
    uint8_t cmd;

    cmd = 0x01;
    st7789_SendCMD(&cmd);
    vTaskDelay(pdMS_TO_TICKS(200));
 
    // 1. 睡眠退出
    cmd = 0x11;
    st7789_SendCMD(&cmd);
    vTaskDelay(pdMS_TO_TICKS(120));
 
    // 2. 内存数据访问控制
    cmd = 0x36;
    st7789_SendCMD(&cmd);
    uint8_t data = 0x60;
    st7789_SendData(&data, 1);

    // 3. 像素格式
    cmd = 0x3A;
    st7789_SendCMD(&cmd);
    data = 0x55;  // RGB565
    st7789_SendData(&data, 1);

    // 4.PORCTRL - 门廊控制
    cmd = 0xB2;
    st7789_SendCMD(&cmd);
    uint8_t porctrl_data[5] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
    st7789_SendData(porctrl_data, 5);
    
    // 5. Gate Control (GCTRL)
    cmd = 0xB7;
    st7789_SendCMD(&cmd);
    data = 0x35;
    st7789_SendData(&data, 1);

    // 6. VCOM 设置
    cmd = 0xBB;
    st7789_SendCMD(&cmd);
    data = 0x19;
    st7789_SendData(&data, 1);

    // 7. LCM Control
    cmd = 0xC0;
    st7789_SendCMD(&cmd);
    data = 0x2C;
    st7789_SendData(&data, 1);

    // 8. VDV and VRH Enable
    cmd = 0xC2;
    st7789_SendCMD(&cmd);
    data = 0x01;
    st7789_SendData(&data, 1);

    // 9. VRH 设置
    cmd = 0xC3;
    st7789_SendCMD(&cmd);
    data = 0x12;
    st7789_SendData(&data, 1);

    // 10. VDV 设置
    cmd = 0xC4;
    st7789_SendCMD(&cmd);
    data = 0x20;
    st7789_SendData(&data, 1);

    // 11. 帧率控制
    cmd = 0xC6;
    st7789_SendCMD(&cmd);
    data = 0x0F;  // 60Hz
    st7789_SendData(&data, 1);

    // 12. Power Control 1
    cmd = 0xD0;
    st7789_SendCMD(&cmd);
    uint8_t pwctrl_data[2] = {0xA4, 0xA1};
    st7789_SendData(pwctrl_data, 2);

    // 13. Positive Voltage Gamma
    cmd = 0xE0;
    st7789_SendCMD(&cmd);
    uint8_t pvgamma[14] = {
        0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 
        0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23
    };
    st7789_SendData(pvgamma, 14);

    // 14. Negative Voltage Gamma
    cmd = 0xE1;
    st7789_SendCMD(&cmd);
    uint8_t nvgamma[14] = {
        0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 
        0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23
    };
    st7789_SendData(nvgamma, 14);

    // 15. 反显关闭
    cmd = 0x21;
    st7789_SendCMD(&cmd);
    
    // 16. 显示开启
    cmd = 0x29;
    st7789_SendCMD(&cmd);
    vTaskDelay(pdMS_TO_TICKS(10));
    
    ESP_LOGI("TFT", "软件初始化完成");
}

void TFT_init(void)
{
   
    st7789_hardware_init();
    LCD_CS_LOW();
    st7789_software_init();
}

void TFT_set_window(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
    uint8_t cmd = ST7789_CASET;
    uint8_t data_col[4] = {xs >> 8, xs, xe >> 8, xe};
    st7789_SendCMD(&cmd);
    st7789_SendData(data_col, 4);

    cmd = ST7789_RASET;
    uint8_t data_row[4] = {ys >> 8, ys, ye >> 8, ye};
    st7789_SendCMD(&cmd);
    st7789_SendData(data_row, 4);
}

void TFT_fill_screen(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
    uint16_t i = 0;
    uint16_t size = (xe - xs + 1) * (ye - ys + 1);
    TFT_set_window(xs, ys, xe, ye);

    uint8_t cmd = ST7789_RAMWR;
    st7789_SendCMD(&cmd);
    uint8_t rgb_color[480];
    for(i = 0; i < 240; i++)
    {
        rgb_color[i*2] = color >> 8;       
        rgb_color[i*2 + 1] = color; 
    }

    LCD_CS_LOW();
    gpio_set_level(IO39_LCD_DC, 1);
    
    for(i = 0; i < (xe - xs + 1); i++)
    {
        spi_transaction_t trans = {
            .length = (ye - ys + 1) * 16,
            .tx_buffer = rgb_color,
        };
        ESP_ERROR_CHECK(spi_device_polling_transmit(handle_spi_tft, &trans));
    }
    LCD_CS_HIGH();
}