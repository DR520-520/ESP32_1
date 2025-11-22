#ifndef __ESP_TFT_H__
#define __ESP_TFT_H__

#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_io_extend.h"


#define TFT_WIDTH       320
#define TFT_LENGTH      240
#define TFT_SPI_HOST    SPI2_HOST
#define IO39_LCD_DC     GPIO_NUM_39
#define IO40_LCD_DATA   GPIO_NUM_40
#define IO41_LCD_SCLK   GPIO_NUM_41
#define IO42_LCD_BL     GPIO_NUM_42 


    //                      System Function Commands                     
#define    ST7789_NOP            0x00   ///< No Operation
#define    ST7789_SWRESET        0x01   ///< Software Reset
#define    ST7789_RDDID          0x04   ///< Read Display ID
#define    ST7789_RDDST          0x09   ///< Read Display Status
#define    ST7789_RDDPM          0x0A   ///< Read Display Power Mode
#define    ST7789_RDDMADCTL      0x0B   ///< Read Display MADCTL
#define    ST7789_RDDCOLMOD      0x0C   ///< Read Display Pixel Format
#define    ST7789_RDDIM          0x0D   ///< Read Display Image Mode
#define    ST7789_RDDSM          0x0E   ///< Read Display Signal Mode
#define    ST7789_RDDSDR         0x0F   ///< Read Display Self-Diagnostic Result
    
#define    ST7789_SLPIN          0x10   ///< Sleep In
#define    ST7789_SLPOUT         0x11   ///< Sleep Out
#define    ST7789_PTLON          0x12   ///< Partial Display Mode On
#define    ST7789_NORON          0x13   ///< Normal Display Mode On
    
#define    ST7789_INVOFF         0x20   ///< Display Inversion Off
#define    ST7789_INVON          0x21   ///< Display Inversion On
#define    ST7789_GAMSET         0x26   ///< Gamma Set
#define    ST7789_DISPOFF        0x28   ///< Display Off
#define    ST7789_DISPON         0x29  ///< Display On
#define    ST7789_CASET          0x2A  ///< Column Address Set
#define    ST7789_RASET          0x2B   ///< Row Address Set
#define    ST7789_RAMWR          0x2C   ///< Memory Write
#define    ST7789_RAMRD          0x2E   ///< Memory Read
    
#define    ST7789_PTLAR          0x30   ///< Partial Area
#define    ST7789_VSCRDEF        0x33   ///< Vertical Scrolling Definition
#define    ST7789_TEOFF          0x34   ///< Tearing Effect Line OFF
#define    ST7789_TEON           0x35   ///< Tearing Effect Line On
#define    ST7789_MADCTL         0x36   ///< Memory Data Access Control
#define    ST7789_VSCSAD         0x37   ///< Vertical Scroll Start Address of RAM
#define    ST7789_IDMOFF         0x38   ///< Idle Mode Off
#define    ST7789_IDMON          0x39   ///< Idle Mode On
#define    ST7789_COLMOD         0x3A   ///< Interface Pixel Format
#define    ST7789_WRMEMC         0x3C   ///< Write Memory Continue
#define    ST7789_RDMEMC         0x3E   ///< Read Memory Continue
    
#define    ST7789_STE            0x44   ///< Set Tear Scanline
#define    ST7789_GSCAN          0x45   ///< Get Scanline
   
#define    ST7789_WRDISBV        0x51   ///< Write Display Brightness
#define    ST7789_RDDISBV        0x52   ///< Read Display Brightness Value
#define    ST7789_WRCTRLD        0x53   ///< Write CTRL Display
#define    ST7789_RDCTRLD        0x54   ///< Read CTRL Value Display
#define    ST7789_WRCACE         0x55   ///< Write Content Adaptive Brightness Control
#define    ST7789_RDCABC         0x56   ///< Read Content Adaptive Brightness Control
#define    ST7789_WRCABCMB       0x5E   ///< Write CABC Minimum Brightness
#define    ST7789_RDCABCMB       0x5F   ///< Read CABC Minimum Brightness
   
#define    ST7789_RDABCSDR       0x68   ///< Read Automatic Brightness Control Self-Diagnostic Result  
#define    ST7789_RAMCTRL        0xB0   ///< RAM Control
#define    ST7789_RGBCTRL        0xB1   ///< RGB Interface Control
#define    ST7789_PORCTRL        0xB2   ///< Porch Setting
#define    ST7789_FRCTRL1        0xB3   ///< Frame Rate Control 1 (In partial mode/idle colors)
#define    ST7789_PARCTRL        0xB5   ///< Partial Control
#define    ST7789_GCTRL          0xB7   ///< Gate Control
#define    ST7789_GTADJ          0xB8   ///< Gate On Timing Adjustment
#define    ST7789_DGMEN          0xBA   ///< Digital Gamma Enable
#define    ST7789_VCOMS          0xBB   ///< VCOM Setting
#define    ST7789_POWSAVE        0xBC   ///< Power Saving Mode
#define    ST7789_DLPOFFSAVE     0xBD   ///< Display Off Power Save
    
#define    ST7789_LCMCTRL        0xC0   ///< LCM Control
#define    ST7789_IDSET          0xC1   ///< ID Code Setting
#define    ST7789_VDVVRHEN       0xC2   ///< VDV and VRH Command Enable
#define    ST7789_VRHS           0xC3   ///< VRH Set
#define    ST7789_VDVS           0xC4   ///< VDV Set
#define    ST7789_VCMOFSET       0xC5   ///< VCOM Offset Set
#define    ST7789_FRCTRL2        0xC6   ///< Frame Rate Control in Normal Mode
#define    ST7789_CABCCTRL       0xC7   ///< CABC Control
#define    ST7789_REGSEL1        0xC8   ///< Register Value Selection 1
#define    ST7789_REGSEL2        0xCA   ///< Register Value Selection 2
#define    ST7789_PWMFRSEL       0xCC   ///< PWM Frequency Selection
    
#define    ST7789_PWCTRL1        0xD0   ///< Power Control 1
#define    ST7789_VAPVANEN       0xD2   ///< Enable VAP/VAN Signal Output
    
#define    ST7789_RDID1          0xDA   ///< Read ID1
#define    ST7789_RDID2          0xDB   ///< Read ID2
#define    ST7789_RDID3          0xDC   ///< Read ID3
    
#define    ST7789_CMD2EN         0xDF   ///< Command 2 Enable
    
#define    ST7789_PVGAMCTRL      0xE0   ///< Positive Voltage Gamma Control
#define    ST7789_NVGAMCTRL      0xE1   ///< Negative Voltage Gamma Control
#define    ST7789_DGMLUTR        0xE2   ///< Digital Gamma Look-up Table for Red
#define    ST7789_DGMLUTB        0xE3   ///< Digital Gamma Look-up Table for Blue
#define    ST7789_GATECTRL       0xE4   ///< Gate Control
#define    ST7789_SPI2EN         0xE7   ///< SPI2 Enable
#define    ST7789_PWCTRL2        0xE8   ///< Power Control 2
#define    ST7789_EQCTRL         0xE9   ///< Equalize Time Control
    
#define    ST7789_PROMCTRL       0xEC   ///< Program Mode Control
    
#define    ST7789_PROMEN         0xFA   ///< Program Mode Enable
#define    ST7789_NVMSET         0xFC   ///< NVM Setting
#define    ST7789_PROMACT        0xFE   ///< Program Action

#define    ST7789_MADCTL_MY     0x80   ///< Row Address Order
#define    ST7789_MADCTL_MX     0x40   ///< Column Address Order
#define    ST7789_MADCTL_MV     0x20   ///< Row/Column Exchange
#define    ST7789_MADCTL_ML     0x10   ///< Vertical Refresh Order
#define    ST7789_MADCTL_RGB    0x00   ///< RGB Order
#define    ST7789_MADCTL_BGR    0x08   ///< BGR Order
#define    ST7789_MADCTL_MH     0x04   ///< Horizontal Refresh Order

#define    ST7789_COLMOD_RGB444    0x03   ///< 12-bit/pixel (RGB444)
#define    ST7789_COLMOD_RGB565    0x05   ///< 16-bit/pixel (RGB565)
#define    ST7789_COLMOD_RGB666    0x06   ///< 18-bit/pixel (RGB666)
#define    ST7789_COLMOD_RGB888    0x07   ///< 24-bit/pixel (RGB888) - Truncated

#define    ST7789_ROTATION_0      0x00                                     ///< 0° rotation
#define    ST7789_ROTATION_90     ST7789_MADCTL_MV | ST7789_MADCTL_MX    ///< 90° rotation
#define    ST7789_ROTATION_180    ST7789_MADCTL_MX | ST7789_MADCTL_MY    ///< 180° rotation
#define    ST7789_ROTATION_270    ST7789_MADCTL_MV | ST7789_MADCTL_MY    ///< 270° rotation

#define    ST7789_GAMMA_CURVE_1   0x01   ///< Gamma Curve 1 (G2.2)
#define    ST7789_GAMMA_CURVE_2   0x02   ///< Gamma Curve 2 (G1.8)
#define    ST7789_GAMMA_CURVE_3   0x04   ///< Gamma Curve 3 (G2.5)
#define    ST7789_GAMMA_CURVE_4   0x08   ///< Gamma Curve 4 (G1.0)

#define    ST7789_TE_MODE_VBLANK   0x00   ///< V-Blanking only
#define    ST7789_TE_MODE_VHBLANK   0x01  ///< V-Blanking and H-Blanking

#define    ST7789_CABC_OFF           0x00   ///< CABC Off
#define    ST7789_CABC_USER_IF       0x01   ///< User Interface Image
#define    ST7789_CABC_STILL_PIC     0x02   ///< Still Picture
#define    ST7789_CABC_MOVING_PIC    0x03   ///< Moving Picture

void st7789_hardware_init();

void st7789_SendCMD(uint8_t *data);

void st7789_SendData(uint8_t *data,  uint8_t len);

void st7789_software_init(void);

void TFT_init(void);

void TFT_set_window(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);

void TFT_fill_screen(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);

//void TFT_set_backlight(uint16_t brightness);

#endif