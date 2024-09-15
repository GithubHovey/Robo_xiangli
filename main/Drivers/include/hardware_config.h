#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

/*LCD CONFIG*/
#define SPI_SDA 1
#define SPI_SCL 2
#define SPI_CS  42
#define SCREEN_TYPE 2 /*1:  4 inch               2:  2.1 inch*/
/*size*/
#define LCD_H_RES              480
#define LCD_V_RES              480
/*BUFFER MODE*/
#define LCD_NUM_FB             2
/*rgb_clock*/
#define LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)
/*RGB_PINS*/
#define PIN_NUM_HSYNC          38
#define PIN_NUM_VSYNC          39
#define PIN_NUM_DE             40
#define PIN_NUM_PCLK           41
#define PIN_NUM_DATA0          5  // B0
#define PIN_NUM_DATA1          45 // B1
#define PIN_NUM_DATA2          48 // B2
#define PIN_NUM_DATA3          47 // B3
#define PIN_NUM_DATA4          21 // B4
#define PIN_NUM_DATA5          14 // G0
#define PIN_NUM_DATA6          13 // G1
#define PIN_NUM_DATA7          12 // G2
#define PIN_NUM_DATA8          11 // G3
#define PIN_NUM_DATA9          10 // G4
#define PIN_NUM_DATA10         9  // G5
#define PIN_NUM_DATA11         46 // R0
#define PIN_NUM_DATA12         3  // R1
#define PIN_NUM_DATA13         8  // R2
#define PIN_NUM_DATA14         18 // R3
#define PIN_NUM_DATA15         17 // R4
#define PIN_NUM_DISP_EN        -1


#endif


