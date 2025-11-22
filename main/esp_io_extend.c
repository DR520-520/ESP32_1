#include "esp_io_extend.h"

void IIC_Init(void)
{
    int i2c_master_port = IIC_PORT_NUM;
    i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = 400000
    };

    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, I2C_MODE_MASTER, 0, 0, 0);
}

esp_err_t IIC_ReadByte(uint8_t device_address, uint8_t reg_address, uint8_t *data, size_t read_size)
{
    esp_err_t ret = i2c_master_write_read_device(IIC_PORT_NUM, device_address, &reg_address, 1, data, read_size, IIC_MASTER_TIMOUT_MS / portTICK_PERIOD_MS);
    return ret;
}

esp_err_t IIC_WriteByte(uint8_t device_address, uint8_t reg_address, uint8_t *data, uint8_t len)
{
    uint8_t write_buffer[MAX_WRITE_BUFFER];
    write_buffer[0] = reg_address;
    for(uint8_t i = 0; i < len; i++)
    {
        write_buffer[i + 1] = data[i];
    }
    esp_err_t ret = i2c_master_write_to_device(IIC_PORT_NUM, device_address, write_buffer, len + 1, IIC_MASTER_TIMOUT_MS / portTICK_PERIOD_MS);
    return ret;
}

/**
 * @brief 
 * 
 * @param IO_NUM_OUTPUT_INPUT :输入一个二进制数，低位为IO0,高位为IO7， 0输出， 1输入
 * @param IO_Polarity_inversion ：同理， 1反转， 0不反转
 * @return 
 */
esp_err_t PCA9557_Set_IO(uint8_t IO_NUM_OUTPUT_INPUT, uint8_t IO_Polarity_inversion)
{
    esp_err_t ret;

    ret = IIC_WriteByte(PCA9557_ADDRESS, CONFIGURATION_REGISTER, &IO_NUM_OUTPUT_INPUT, 1);
    ret = IIC_WriteByte(PCA9557_ADDRESS, POLARITY_INVERSION_REGISTER, &IO_Polarity_inversion, 1);
    return ret;
}

esp_err_t PCA9557_Set_Pin(PCA9557_IO_NUM IO_NUM)
{
    esp_err_t ret;
    uint8_t Reg_Data;

    IIC_ReadByte(PCA9557_ADDRESS, OUTPUT_PORT_REGISTER, &Reg_Data, 1);
    Reg_Data |= IO_NUM;
    ret =  IIC_WriteByte(PCA9557_ADDRESS, OUTPUT_PORT_REGISTER, &Reg_Data, 1);
    if(ret != ESP_OK)
    {
        return ret; 
    }

    return ESP_OK;
}

esp_err_t PCA9557_Reset_Pin(PCA9557_IO_NUM IO_NUM)
{
    esp_err_t ret;
    uint8_t Reg_Data;
    ret = IIC_ReadByte(PCA9557_ADDRESS, OUTPUT_PORT_REGISTER, &Reg_Data, 1);
    if(ret != ESP_OK)
    {
        return ret; 
    }
    Reg_Data &= ~IO_NUM;
    ret =  IIC_WriteByte(PCA9557_ADDRESS, OUTPUT_PORT_REGISTER, &Reg_Data, 1);
    if(ret != ESP_OK)
    {
        return ret; 
    }
    return ESP_OK;
}

esp_err_t PCA9557_Toggle_Pin(PCA9557_IO_NUM IO_NUM)
{
    esp_err_t ret;
    uint8_t Reg_Data;
    ret = IIC_ReadByte(PCA9557_ADDRESS, OUTPUT_PORT_REGISTER, &Reg_Data, 1);
    if(ret != ESP_OK)
    {
        return ret; 
    }
    Reg_Data ^= IO_NUM;
    ret =  IIC_WriteByte(PCA9557_ADDRESS, OUTPUT_PORT_REGISTER, &Reg_Data, 1);
    if(ret != ESP_OK)
    {
        return ret; 
    }
    return ESP_OK;
}

uint8_t PCA9557_Read_Pin(PCA9557_IO_NUM IO_NUM)
{
    esp_err_t ret;
    uint8_t Reg_Data;
    ret = IIC_ReadByte(PCA9557_ADDRESS, INPUT_PORT_REGISTER, &Reg_Data, 1);
    if(ret != ESP_OK)
    {
        return 2; 
    }
    return (Reg_Data & IO_NUM) ? 1 : 0;
}

esp_err_t PCA9557_Write_Pin(PCA9557_IO_NUM IO_NUM, uint8_t PIN_Status)
{
    esp_err_t ret;
    uint8_t Reg_Data;
    if(PIN_Status == 1)
    {
        ret = PCA9557_Set_Pin(IO_NUM);
        if(ret != ESP_OK)
        {
            return ret;
        }
    }
    else if(PIN_Status == 0)
    {
        ret = PCA9557_Reset_Pin(IO_NUM);
        if(ret != ESP_OK)
        {
            return ret;
        }
    }

    return ESP_OK;
}

void LCD_CS_LOW(void)
{
    PCA9557_Reset_Pin(PCA9557_IO_NUM_0);
}

void LCD_CS_HIGH(void)
{
    PCA9557_Set_Pin(PCA9557_IO_NUM_0);
}