#ifndef __ESP_IO_EXTEND_H__
#define __ESP_IO_EXTEND_H__

#include "driver/i2c.h"

#define MAX_WRITE_BUFFER            32
#define IIC_PORT_NUM                0
#define IIC_MASTER_TIMOUT_MS        1000

#define QMI8658A_ADDRESS            0x6A
#define PCA9557_ADDRESS             0x19

#define INPUT_PORT_REGISTER         0x00
#define OUTPUT_PORT_REGISTER        0x01
#define POLARITY_INVERSION_REGISTER 0x02
#define CONFIGURATION_REGISTER      0x03

typedef enum
{
    PCA9557_IO_NUM_0 = (1 << 0),
    PCA9557_IO_NUM_1 = (1 << 1),
    PCA9557_IO_NUM_2 = (1 << 2)
} PCA9557_IO_NUM;



#define PCA9557_IO_OUTPUT           0
#define PCA9557_IO_INPUT            1
#define I2C_MASTER_SDA_IO GPIO_NUM_1
#define I2C_MASTER_SCL_IO GPIO_NUM_2

void IIC_Init(void);
esp_err_t IIC_ReadByte(uint8_t device_address, uint8_t reg_address, uint8_t *data, size_t read_size);
esp_err_t IIC_WriteByte(uint8_t device_address, uint8_t reg_address, uint8_t *data, uint8_t len);
esp_err_t PCA9557_Set_IO(uint8_t IO_NUM_OUTPUT_INPUT, uint8_t IO_Polarity_inversion);
esp_err_t PCA9557_Set_Pin(PCA9557_IO_NUM IO_NUM);
esp_err_t PCA9557_Reset_Pin(PCA9557_IO_NUM IO_NUM);
esp_err_t PCA9557_Toggle_Pin(PCA9557_IO_NUM IO_NUM);
uint8_t PCA9557_Read_Pin(PCA9557_IO_NUM IO_NUM);
esp_err_t PCA9557_Write_Pin(PCA9557_IO_NUM IO_NUM, uint8_t PIN_Status);
void LCD_CS_LOW(void);
void LCD_CS_HIGH(void);


#endif // !__ESP_IMU_H__