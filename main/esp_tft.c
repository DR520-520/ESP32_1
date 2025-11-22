// esp_tft.c - 完整替换

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
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_config_struct));
    
    // 初始状态设置
    gpio_set_level(IO39_LCD_DC, 0);

    /*---------SPI初始化----------*/
    spi_bus_config_t bus_config = 
    {
        .mosi_io_num = IO40_LCD_DATA,
        .miso_io_num = -1,
        .sclk_io_num = IO41_LCD_SCLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = TFT_LENGTH * TFT_WIDTH * 2 + 8
    };
    ESP_ERROR_CHECK(spi_bus_initialize(TFT_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t dev_config = 
    {
        .mode = 0,
        .clock_speed_hz = 10 * 1000 * 1000,  // 先降到10MHz测试
        .spics_io_num = -1,
        .queue_size = 7,
        .flags = SPI_DEVICE_HALFDUPLEX,  // 添加半双工标志
        .pre_cb = NULL
    };
    ESP_ERROR_CHECK(spi_bus_add_device(TFT_SPI_HOST, &dev_config, &handle_spi_tft));

    /*------------背光控制--------------*/
    ledc_timer_config_t timer_conf = 
    {
        .freq_hz = 5000,  // 降低频率
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    ledc_channel_config_t pwm_channel_config = 
    {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,  // 初始关闭
        .flags.output_invert = 0,
        .gpio_num = IO42_LCD_BL,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel_config));
    
    ESP_LOGI("TFT", "硬件初始化完成");
}

void st7789_SendCMD(uint8_t cmd)  // 改为直接传值,不用指针
{
    LCD_CS_LOW();
    gpio_set_level(IO39_LCD_DC, 0);
    
    spi_transaction_t trans_desc = {
        .length = 8,
        .tx_buffer = &cmd,
    };
    
    ESP_ERROR_CHECK(spi_device_polling_transmit(handle_spi_tft, &trans_desc));
    LCD_CS_HIGH();
}

void st7789_SendData(uint8_t *data, uint16_t len)  // len改为uint16_t
{
    if(data == NULL || len == 0) return;
    
    LCD_CS_LOW();
    gpio_set_level(IO39_LCD_DC, 1);
    
    spi_transaction_t trans_desc = {
        .length = 8 * len,
        .tx_buffer = data,
    };
    
    ESP_ERROR_CHECK(spi_device_polling_transmit(handle_spi_tft, &trans_desc));
    LCD_CS_HIGH();
}

// esp_tft.c - 关键修改

void st7789_software_init(void)
{
    ESP_LOGI("TFT", "开始软件初始化");
    
    // 加长初始延迟
    vTaskDelay(pdMS_TO_TICKS(120));
    
    // 1. 软件复位
    st7789_SendCMD(0x01);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    // 2. 退出睡眠
    st7789_SendCMD(0x11);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    // 3. **关键** - MADCTL设置(根据你的硬件调整)
    st7789_SendCMD(0x36);
    uint8_t madctl = 0x60;  // 试试这些值: 0x00, 0x60, 0xC0, 0xA0
    st7789_SendData(&madctl, 1);
    
    // 4. 颜色模式
    st7789_SendCMD(0x3A);
    uint8_t colmod = 0x05;
    st7789_SendData(&colmod, 1);
    
    // 5. Porch设置
    st7789_SendCMD(0xB2);
    uint8_t porch[5] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
    st7789_SendData(porch, 5);
    
    // 6. Gate控制
    st7789_SendCMD(0xB7);
    uint8_t gctrl = 0x35;
    st7789_SendData(&gctrl, 1);
    
    // 7. **重要** VCOM设置
    st7789_SendCMD(0xBB);
    uint8_t vcom = 0x19;  // 或者试试 0x20, 0x35
    st7789_SendData(&vcom, 1);
    
    // 8. LCM控制
    st7789_SendCMD(0xC0);
    uint8_t lcm = 0x2C;
    st7789_SendData(&lcm, 1);
    
    // 9. VDV/VRH使能
    st7789_SendCMD(0xC2);
    uint8_t vdvvrhen = 0x01;
    st7789_SendData(&vdvvrhen, 1);
    
    // 10. VRH设置
    st7789_SendCMD(0xC3);
    uint8_t vrh = 0x12;
    st7789_SendData(&vrh, 1);
    
    // 11. VDV设置
    st7789_SendCMD(0xC4);
    uint8_t vdv = 0x20;
    st7789_SendData(&vdv, 1);
    
    // 12. **关键** 帧率控制 - 降低帧率更稳定
    st7789_SendCMD(0xC6);
    uint8_t frc = 0x0F;  // 60Hz,或试试 0x01(highest), 0x1F(lowest)
    st7789_SendData(&frc, 1);
    
    // 13. 电源控制
    st7789_SendCMD(0xD0);
    uint8_t pwr[2] = {0xA4, 0xA1};
    st7789_SendData(pwr, 2);
    
    // 14. Gamma设置(使用ST7789标准值)
    st7789_SendCMD(0xE0);
    uint8_t pvgamma[14] = {
        0xD0, 0x08, 0x11, 0x08, 0x0C, 0x15, 0x39,
        0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2D
    };
    st7789_SendData(pvgamma, 14);
    
    st7789_SendCMD(0xE1);
    uint8_t nvgamma[14] = {
        0xD0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39,
        0x44, 0x51, 0x0B, 0x16, 0x14, 0x2F, 0x31
    };
    st7789_SendData(nvgamma, 14);
    
    // 15. **必须** 关闭反显
    st7789_SendCMD(0x21);  // INVOFF
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // 16. **必须** 开启显示
    st7789_SendCMD(0x29);  // DISPON
    vTaskDelay(pdMS_TO_TICKS(120));  // 加长延迟
    
    // 17. **新增** 写入一次空数据激活显示
    st7789_SendCMD(0x2C);  // RAMWR
    
    ESP_LOGI("TFT", "软件初始化完成");
}

void TFT_init(void)
{
    st7789_hardware_init();
    LCD_CS_LOW();  // 先拉低CS
    vTaskDelay(pdMS_TO_TICKS(20));
    st7789_software_init();
}

void TFT_set_window(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
    // CASET - 列地址设置
    st7789_SendCMD(ST7789_CASET);
    uint8_t data_col[4] = {xs >> 8, xs & 0xFF, xe >> 8, xe & 0xFF};
    st7789_SendData(data_col, 4);
    
    // RASET - 行地址设置
    st7789_SendCMD(ST7789_RASET);
    uint8_t data_row[4] = {ys >> 8, ys & 0xFF, ye >> 8, ye & 0xFF};
    st7789_SendData(data_row, 4);
}

void TFT_fill_screen(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
    uint32_t size = (uint32_t)(xe - xs + 1) * (ye - ys + 1);
    
    TFT_set_window(xs, ys, xe, ye);
    st7789_SendCMD(ST7789_RAMWR);
    
    LCD_CS_LOW();
    gpio_set_level(IO39_LCD_DC, 1);
    
    // 直接发送,不转换字节序
    uint8_t color_buf[480];  // 240个像素 * 2字节
    for(int i = 0; i < 240; i++) {
        color_buf[i*2] = color >> 8;        // 高字节
        color_buf[i*2 + 1] = color & 0xFF;  // 低字节
    }
    
    for(uint32_t row = 0; row < (ye - ys + 1); row++) {
        spi_transaction_t trans = {
            .length = (xe - xs + 1) * 16,
            .tx_buffer = color_buf,
        };
        ESP_ERROR_CHECK(spi_device_polling_transmit(handle_spi_tft, &trans));
    }
    
    LCD_CS_HIGH();
}

// 添加背光控制函数
void TFT_set_backlight(uint16_t brightness)  // 0-1023
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, brightness);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}