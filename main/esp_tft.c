#include "esp_tft.h"
spi_device_handle_t handle_spi_tft;
void st7789_hardware_init()
{
    /*---------GPIO初始化----------*/
    gpio_config_t gpio_config_struct = 
    {
        .pin_bit_mask = IO39_LCD_DC,
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
     .max_transfer_sz = TFT_LENGTH * TFT_WIDTH * 16
    };
    ESP_ERROR_CHECK(spi_bus_initialize(TFT_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t dev_config = 
    {
        .mode = 0,
        .clock_speed_hz = 20 * 1000* 1000,
        .spics_io_num = -1,
        .queue_size = 10,
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
        .duty = 512,
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


