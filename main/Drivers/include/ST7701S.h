#ifndef RGB_PANEL_ST7701S_H
#define RGB_PANEL_ST7701S_H

#include "driver/spi_master.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "../include/hardware_config.h"

#define SPI_METHOD 1
#define IOEXPANDER_METHOD 0

typedef struct{
    char method_select;
    //SPI config_t
    spi_device_handle_t spi_device;
    spi_bus_config_t spi_io_config_t;
    spi_device_interface_config_t st7701s_protocol_config_t;
    esp_lcd_rgb_panel_config_t panel_config;
    esp_lcd_panel_handle_t panel_handle;
}ST7701S;

typedef ST7701S * ST7701S_handle;

//Create new object
ST7701S_handle ST7701S_newObject();
//Screen initialization
void ST7701S_Init(ST7701S_handle St7701S_handle);



void ST7701S_screen_config(ST7701S_handle St7701S_handle);
void LCD_SPIInit(ST7701S_handle St7701S_handle, unsigned char type);

//Delete object
void ST7701S_delObject(ST7701S_handle St7701S_handle);

//SPI write instruction
void ST7701S_WriteCommand(ST7701S_handle St7701S_handle, uint8_t cmd);

//SPI write data
void ST7701S_WriteData(ST7701S_handle St7701S_handle, uint8_t data);
void flush_color(ST7701S_handle St7701S_handle, int x_start, int y_start, int x_end, int y_end, const void *color_data);
bool on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data);
#endif
