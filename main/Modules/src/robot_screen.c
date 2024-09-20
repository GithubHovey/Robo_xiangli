/*------------------------------------------------------------------------------
 * @file    SYSINIT.C
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/08/27 10:27:30
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/
#include "../include/robot_screen.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "../../Middlewares/include/lvgl_porting.h"
#include "../../Middlewares/include/myGUI.h"
#include "../../Drivers/include/ST7701S.h"

static const char* MODULE_SCREEN = "[Module-screen]";


static ST7701S_handle st7701s = NULL;
void ScreenInit()
{
    st7701s = ST7701S_newObject();    
    ST7701S_Init(st7701s); 
    ESP_LOGI(MODULE_SCREEN, "ST7701S_Init success.");
    Enable_lvgl_for_screen(st7701s);
    ESP_LOGI(MODULE_SCREEN, "Screen init success.");
}
void screen_task(void *args)
{
    const char* tag = pcTaskGetName(xTaskGetCurrentTaskHandle());
    ESP_LOGI(tag, "%s is created.",tag);
    
    for(;;)
    {
        vTaskDelay(5);
    }
}
void lvgl_task(void * arg)
{
    GUI_init();
    for (;;)
    {
        vTaskDelay(5);
        lv_timer_handler();
    }
}
