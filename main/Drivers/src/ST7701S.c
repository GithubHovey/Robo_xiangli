#include "../include/ST7701S.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#define SPI_WriteComm(cmd) ST7701S_WriteCommand(St7701S_handle, cmd)
#define SPI_WriteData(data) ST7701S_WriteData(St7701S_handle, data)
#define Delay(ms) vTaskDelay(ms / portTICK_PERIOD_MS)

void ioexpander_init(){};
void ioexpander_write_cmd(){};
void ioexpander_write_data(){};
static const char* TAG_ST7701s = "[Driver-ST7701S]";
/**
 * @brief Example Create an ST7701S object
 * @param SDA SDA pin
 * @param SCL SCL pin
 * @param CS  CS  pin
 * @param channel_select SPI channel selection
 * @param method_select  SPI_METHOD,IOEXPANDER_METHOD
 * @note
*/
ST7701S_handle ST7701S_newObject()
{
    // if you use `malloc()`, please set 0 in the area to be assigned.
    ST7701S_handle st7701s_handle = heap_caps_calloc(1, sizeof(ST7701S), MALLOC_CAP_DEFAULT);
    // st7701s_handle->method_select = SPI_METHOD;
    spi_host_device_t channel_select = SPI3_HOST;
    // if(SPI_METHOD){
        st7701s_handle->spi_io_config_t.miso_io_num = -1;
        st7701s_handle->spi_io_config_t.mosi_io_num = SPI_SDA;
        st7701s_handle->spi_io_config_t.sclk_io_num = SPI_SCL;
        st7701s_handle->spi_io_config_t.quadwp_io_num = -1;
        st7701s_handle->spi_io_config_t.quadhd_io_num = -1;

        st7701s_handle->spi_io_config_t.max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE;

        ESP_ERROR_CHECK(spi_bus_initialize(channel_select, &(st7701s_handle->spi_io_config_t),SPI_DMA_CH_AUTO));

        st7701s_handle->st7701s_protocol_config_t.command_bits = 1;
        st7701s_handle->st7701s_protocol_config_t.address_bits = 8;
        st7701s_handle->st7701s_protocol_config_t.clock_speed_hz = 10000000;
        st7701s_handle->st7701s_protocol_config_t.mode = 0;
        st7701s_handle->st7701s_protocol_config_t.spics_io_num = SPI_CS;
        st7701s_handle->st7701s_protocol_config_t.queue_size = 1;

        ESP_ERROR_CHECK(spi_bus_add_device(channel_select, &(st7701s_handle->st7701s_protocol_config_t),
                                        &(st7701s_handle->spi_device)));



    st7701s_handle->panel_config = (esp_lcd_rgb_panel_config_t){
        .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
        .psram_trans_align = 64,
        .num_fbs = LCD_NUM_FB,
#if CONFIG_USE_BOUNCE_BUFFER
        .bounce_buffer_size_px = 10 * LCD_H_RES,
#endif
        .clk_src = LCD_CLK_SRC_PLL240M,
        .disp_gpio_num = PIN_NUM_DISP_EN,
        .pclk_gpio_num = PIN_NUM_PCLK,
        .vsync_gpio_num = PIN_NUM_VSYNC,
        .hsync_gpio_num = PIN_NUM_HSYNC,
        .de_gpio_num = PIN_NUM_DE,
        .data_gpio_nums = {
            PIN_NUM_DATA0,
            PIN_NUM_DATA1,
            PIN_NUM_DATA2,
            PIN_NUM_DATA3,
            PIN_NUM_DATA4,
            PIN_NUM_DATA5,
            PIN_NUM_DATA6,
            PIN_NUM_DATA7,
            PIN_NUM_DATA8,
            PIN_NUM_DATA9,
            PIN_NUM_DATA10,
            PIN_NUM_DATA11,
            PIN_NUM_DATA12,
            PIN_NUM_DATA13,
            PIN_NUM_DATA14,
            PIN_NUM_DATA15,
        },
        .timings = {
            .pclk_hz = LCD_PIXEL_CLOCK_HZ,
            .h_res = LCD_H_RES,
            .v_res = LCD_V_RES, 
            .hsync_back_porch = 10,
            .hsync_front_porch = 50,
            .hsync_pulse_width = 8,
            .vsync_back_porch = 8,
            .vsync_front_porch = 8,
            .vsync_pulse_width = 3,     
            .flags.pclk_active_neg = false,
        },
        .flags.fb_in_psram = true, // allocate frame buffer in PSRAM
        .bounce_buffer_size_px = 5*LCD_H_RES*2
    };   
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&st7701s_handle->panel_config, &st7701s_handle->panel_handle));

    ESP_LOGI(TAG_ST7701s, "Register event callbacks");
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = on_vsync_event,
    };
    
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(st7701s_handle->panel_handle, &cbs, NULL));


//         st7701s_handle->panel_config.data_width = 16; // RGB565 in parallel mode, thus 16bit in width
//         st7701s_handle->panel_config.psram_trans_align = 64;
//         st7701s_handle->panel_config.num_fbs = LCD_NUM_FB;
// #if CONFIG_USE_BOUNCE_BUFFER
//         st7701s_handle->panel_config.bounce_buffer_size_px = 10 * LCD_H_RES,
// #endif
//         st7701s_handle->panel_config.clk_src = LCD_CLK_SRC_PLL240M;
//         st7701s_handle->panel_config.disp_gpio_num = PIN_NUM_DISP_EN;
//         st7701s_handle->panel_config.pclk_gpio_num = PIN_NUM_PCLK;
//         st7701s_handle->panel_config.vsync_gpio_num = PIN_NUM_VSYNC;
//         st7701s_handle->panel_config.hsync_gpio_num = PIN_NUM_HSYNC;
//         st7701s_handle->panel_config.de_gpio_num = PIN_NUM_DE;
//         st7701s_handle->panel_config.data_gpio_nums = {
//             PIN_NUM_DATA0,
//             PIN_NUM_DATA1,
//             PIN_NUM_DATA2,
//             PIN_NUM_DATA3,
//             PIN_NUM_DATA4,
//             PIN_NUM_DATA5,
//             PIN_NUM_DATA6,
//             PIN_NUM_DATA7,
//             PIN_NUM_DATA8,
//             PIN_NUM_DATA9,
//             PIN_NUM_DATA10,
//             PIN_NUM_DATA11,
//             PIN_NUM_DATA12,
//             PIN_NUM_DATA13,
//             PIN_NUM_DATA14,
//             PIN_NUM_DATA15,
//         };
//         st7701s_handle->panel_config.timings = {
//             .pclk_hz = LCD_PIXEL_CLOCK_HZ,
//             .h_res = LCD_H_RES,
//             .v_res = LCD_V_RES, 
//             .hsync_back_porch = 10,
//             .hsync_front_porch = 50,
//             .hsync_pulse_width = 8,
//             .vsync_back_porch = 8,
//             .vsync_front_porch = 8,
//             .vsync_pulse_width = 3,     
//             .flags.pclk_active_neg = false,
//         },
//         st7701s_handle->panel_config.flags.fb_in_psram = true;// allocate frame buffer in PSRAM
        return st7701s_handle;
    // }else{
    //     ioexpander_init();
    // }
    // return NULL;
}

/**
 * @brief Screen initialization
 * @param St7701S_handle 
 * @param type 
 * @note
*/
void ST7701S_screen_config(ST7701S_handle St7701S_handle)
{
    switch(SCREEN_TYPE)
    {
        case 1:/*4 inch*/
            {
                SPI_WriteComm(0x11); 
                Delay(120); //Delay 120ms 

                SPI_WriteComm(0xFF);
                SPI_WriteData(0x77);
                SPI_WriteData(0x01);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x10);

                SPI_WriteComm(0xC0);
                SPI_WriteData(0x3B);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xC1);
                SPI_WriteData(0x0D);
                SPI_WriteData(0x02);

                SPI_WriteComm(0xC2);
                SPI_WriteData(0x21);
                SPI_WriteData(0x08);

                SPI_WriteComm(0xCD);
                SPI_WriteData(0x08);


                SPI_WriteComm(0xB0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x11);
                SPI_WriteData(0x18);
                SPI_WriteData(0x0E);
                SPI_WriteData(0x11);
                SPI_WriteData(0x06);
                SPI_WriteData(0x07);
                SPI_WriteData(0x08);
                SPI_WriteData(0x07);
                SPI_WriteData(0x22);
                SPI_WriteData(0x04);
                SPI_WriteData(0x12);
                SPI_WriteData(0x0F);
                SPI_WriteData(0xAA);
                SPI_WriteData(0x31);
                SPI_WriteData(0x18);

                SPI_WriteComm(0xB1);
                SPI_WriteData(0x00);
                SPI_WriteData(0x11);
                SPI_WriteData(0x19);
                SPI_WriteData(0x0E);
                SPI_WriteData(0x12);
                SPI_WriteData(0x07);
                SPI_WriteData(0x08);
                SPI_WriteData(0x08);
                SPI_WriteData(0x08);
                SPI_WriteData(0x22);
                SPI_WriteData(0x04);
                SPI_WriteData(0x11);
                SPI_WriteData(0x11);
                SPI_WriteData(0xA9);
                SPI_WriteData(0x32);
                SPI_WriteData(0x18);

                SPI_WriteComm(0xFF);

                SPI_WriteData(0x77);
                SPI_WriteData(0x01);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x11);

                SPI_WriteComm(0xB0);
                SPI_WriteData(0x60);

                SPI_WriteComm(0xB1);
                SPI_WriteData(0x30);

                SPI_WriteComm(0xB2);
                SPI_WriteData(0x87);

                SPI_WriteComm(0xB3);
                SPI_WriteData(0x80);

                SPI_WriteComm(0xB5);
                SPI_WriteData(0x49);

                SPI_WriteComm(0xB7);
                SPI_WriteData(0x85);

                SPI_WriteComm(0xB8);
                SPI_WriteData(0x21);

                SPI_WriteComm(0xC1);
                SPI_WriteData(0x78);

                SPI_WriteComm(0xC2);
                SPI_WriteData(0x78);

                Delay(20);
                SPI_WriteComm(0xE0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x1B);
                SPI_WriteData(0x02);

                SPI_WriteComm(0xE1);
                SPI_WriteData(0x08);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x07);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x44);
                SPI_WriteData(0x44);

                SPI_WriteComm(0xE2);
                SPI_WriteData(0x11);
                SPI_WriteData(0x11);
                SPI_WriteData(0x44);
                SPI_WriteData(0x44);
                SPI_WriteData(0xED);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0xEC);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00); 

                SPI_WriteComm(0xE3);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x11);
                SPI_WriteData(0x11);

                SPI_WriteComm(0xE4);
                SPI_WriteData(0x44);
                SPI_WriteData(0x44);

                SPI_WriteComm(0xE5);
                SPI_WriteData(0x0A);
                SPI_WriteData(0xE9);
                SPI_WriteData(0xD8);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x0C);
                SPI_WriteData(0xEB);
                SPI_WriteData(0xD8);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x0E);
                SPI_WriteData(0xED);
                SPI_WriteData(0xD8);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x10);
                SPI_WriteData(0xEF);
                SPI_WriteData(0xD8);
                SPI_WriteData(0xA0);

                SPI_WriteComm(0xE6);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x11);
                SPI_WriteData(0x11);

                SPI_WriteComm(0xE7);
                SPI_WriteData(0x44);
                SPI_WriteData(0x44);

                SPI_WriteComm(0xE8);
                SPI_WriteData(0x09);
                SPI_WriteData(0xE8);
                SPI_WriteData(0xD8);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x0B);
                SPI_WriteData(0xEA);
                SPI_WriteData(0xD8);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x0D);
                SPI_WriteData(0xEC);
                SPI_WriteData(0xD8);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x0F);
                SPI_WriteData(0xEE);
                SPI_WriteData(0xD8);
                SPI_WriteData(0xA0);

                SPI_WriteComm(0xEB);
                SPI_WriteData(0x02);
                SPI_WriteData(0x00);
                SPI_WriteData(0xE4);
                SPI_WriteData(0xE4);
                SPI_WriteData(0x88);
                SPI_WriteData(0x00);
                SPI_WriteData(0x40);

                SPI_WriteComm(0xEC);
                SPI_WriteData(0x3C);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xED);
                SPI_WriteData(0xAB);
                SPI_WriteData(0x89);
                SPI_WriteData(0x76);
                SPI_WriteData(0x54);
                SPI_WriteData(0x02);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);
                SPI_WriteData(0x20);
                SPI_WriteData(0x45);
                SPI_WriteData(0x67);
                SPI_WriteData(0x98);
                SPI_WriteData(0xBA);

                SPI_WriteComm(0xFF);
                SPI_WriteData(0x77);
                SPI_WriteData(0x01);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);



                SPI_WriteComm(0x36); 
                SPI_WriteData(0x00); 
                SPI_WriteComm(0x3A); 
                SPI_WriteData(0x66); 

                SPI_WriteComm(0x21);
                Delay (120);
                SPI_WriteComm(0x29); 
            }
            break;
        case 2:/*2.1inch*/
            {
                Delay(120);
                SPI_WriteComm(0x11);
                Delay(120);
                SPI_WriteComm(0xFF);
                SPI_WriteData(0x77);
                SPI_WriteData(0x01);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x10);

                SPI_WriteComm(0xC0);
                SPI_WriteData(0x3B);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xC1);
                SPI_WriteData(0x0B); // VBP
                SPI_WriteData(0x02);

                SPI_WriteComm(0xC2);
                SPI_WriteData(0x00);
                SPI_WriteData(0x02);

                SPI_WriteComm(0xCC);
                SPI_WriteData(0x10);

                SPI_WriteComm(0xCD);
                SPI_WriteData(0x08);

                SPI_WriteComm(0xB0); // Positive Voltage Gamma Control
                SPI_WriteData(0x02);
                SPI_WriteData(0x13);
                SPI_WriteData(0x1B);
                SPI_WriteData(0x0D);
                SPI_WriteData(0x10);
                SPI_WriteData(0x05);
                SPI_WriteData(0x08);
                SPI_WriteData(0x07);
                SPI_WriteData(0x07);
                SPI_WriteData(0x24);
                SPI_WriteData(0x04);
                SPI_WriteData(0x11);
                SPI_WriteData(0x0E);
                SPI_WriteData(0x2C);
                SPI_WriteData(0x33);
                SPI_WriteData(0x1D);

                SPI_WriteComm(0xB1); // Negative Voltage Gamma Control
                SPI_WriteData(0x05);
                SPI_WriteData(0x13);
                SPI_WriteData(0x1B);
                SPI_WriteData(0x0D);
                SPI_WriteData(0x11);
                SPI_WriteData(0x05);
                SPI_WriteData(0x08);
                SPI_WriteData(0x07);
                SPI_WriteData(0x07);
                SPI_WriteData(0x24);
                SPI_WriteData(0x04);
                SPI_WriteData(0x11);
                SPI_WriteData(0x0E);
                SPI_WriteData(0x2C);
                SPI_WriteData(0x33);
                SPI_WriteData(0x1D);

                SPI_WriteComm(0xFF);
                SPI_WriteData(0x77);
                SPI_WriteData(0x01);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x11);

                SPI_WriteComm(0xB0);
                SPI_WriteData(0x5d); // 5d

                SPI_WriteComm(0xB1);  // VCOM amplitude setting
                SPI_WriteData(0x43); // 43

                SPI_WriteComm(0xB2);  // VGH Voltage setting
                SPI_WriteData(0x81); // 12V

                SPI_WriteComm(0xB3);
                SPI_WriteData(0x80);

                SPI_WriteComm(0xB5);  // VGL Voltage setting
                SPI_WriteData(0x43); //-8.3V

                SPI_WriteComm(0xB7);
                SPI_WriteData(0x85);

                SPI_WriteComm(0xB8);
                SPI_WriteData(0x20);

                SPI_WriteComm(0xC1);
                SPI_WriteData(0x78);

                SPI_WriteComm(0xC2);
                SPI_WriteData(0x78);

                SPI_WriteComm(0xD0);
                SPI_WriteData(0x88);

                SPI_WriteComm(0xE0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x02);

                SPI_WriteComm(0xE1);
                SPI_WriteData(0x03);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x04);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x20);
                SPI_WriteData(0x20);

                SPI_WriteComm(0xE2);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xE3);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x11);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xE4);
                SPI_WriteData(0x22);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xE5);
                SPI_WriteData(0x05);
                SPI_WriteData(0xEC);
                SPI_WriteData(0xA0);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x07);
                SPI_WriteData(0xEE);
                SPI_WriteData(0xA0);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xE6);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x11);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xE7);
                SPI_WriteData(0x22);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xE8);
                SPI_WriteData(0x06);
                SPI_WriteData(0xED);
                SPI_WriteData(0xA0);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x08);
                SPI_WriteData(0xEF);
                SPI_WriteData(0xA0);
                SPI_WriteData(0xA0);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xEB);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x40);
                SPI_WriteData(0x40);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);

                SPI_WriteComm(0xED);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xBA);
                SPI_WriteData(0x0A);
                SPI_WriteData(0xBF);
                SPI_WriteData(0x45);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);
                SPI_WriteData(0x54);
                SPI_WriteData(0xFB);
                SPI_WriteData(0xA0);
                SPI_WriteData(0xAB);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);
                SPI_WriteData(0xFF);

                SPI_WriteComm(0xEF);
                SPI_WriteData(0x10);
                SPI_WriteData(0x0D);
                SPI_WriteData(0x04);
                SPI_WriteData(0x08);
                SPI_WriteData(0x3F);
                SPI_WriteData(0x1F);

                SPI_WriteComm(0xFF);
                SPI_WriteData(0x77);
                SPI_WriteData(0x01);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x13);

                SPI_WriteComm(0xEF);
                SPI_WriteData(0x08);

                SPI_WriteComm(0xFF);
                SPI_WriteData(0x77);
                SPI_WriteData(0x01);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);
                SPI_WriteData(0x00);

                SPI_WriteComm(0x36);//nadctl
                SPI_WriteData(0x00);

                SPI_WriteComm(0x3A);
                SPI_WriteData(0x60);
                
                SPI_WriteComm(0x11);
                Delay(120);
                SPI_WriteComm(0x29);
            }
            break;
    }
}
/**
 * @brief Example Delete the ST7701S object
 * @param St7701S_handle 
*/
/**
* @brief  
* @param  
* @return 
*/
void ST7701S_Init(ST7701S_handle St7701S_handle)
{   
    ESP_LOGI(TAG_ST7701s, "Initialize RGB LCD panel");
    ST7701S_screen_config(St7701S_handle);
    ESP_ERROR_CHECK(esp_lcd_panel_reset(St7701S_handle->panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(St7701S_handle->panel_handle));
}
void ST7701S_delObject(ST7701S_handle St7701S_handle)
{
    assert(St7701S_handle != NULL);
    free(St7701S_handle);
}

/**
 * @brief SPI write instruction
 * @param St7701S_handle 
 * @param cmd instruction
*/
void ST7701S_WriteCommand(ST7701S_handle St7701S_handle, uint8_t cmd)
{
    // if(St7701S_handle->method_select){
        spi_transaction_t spi_tran = {
            .rxlength = 0,
            .length = 0,
            .cmd = 0,
            .addr = cmd,
        };
        spi_device_transmit(St7701S_handle->spi_device, &spi_tran);
    // }else{
    //     ioexpander_write_cmd();
    // }
}

/**
 * @brief SPI write data
 * @param St7701S_handle
 * @param data 
*/
void ST7701S_WriteData(ST7701S_handle St7701S_handle, uint8_t data)
{
    // if(St7701S_handle->method_select){
        spi_transaction_t spi_tran = {
            .rxlength = 0,
            .length = 0,
            .cmd = 1,
            .addr = data,
        };
        spi_device_transmit(St7701S_handle->spi_device, &spi_tran);
    // }else{
    //     ioexpander_write_data();
    // }
}

void flush_color(ST7701S_handle St7701S_handle, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    esp_lcd_panel_draw_bitmap(St7701S_handle->panel_handle, x_start, y_start, x_end, y_end, color_data);
}
/**
* @brief  
* @param  
* @return 
*/
bool on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
#if CONFIG_AVOID_TEAR_EFFECT_WITH_SEM
    if (xSemaphoreTakeFromISR(sem_gui_ready, &high_task_awoken) == pdTRUE) {
        xSemaphoreGiveFromISR(sem_vsync_end, &high_task_awoken);
    }
#endif
    return high_task_awoken == pdTRUE;
}