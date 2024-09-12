/*------------------------------------------------------------------------------
 * @file    SYS_APP.C
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/08/27 19:32:58
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/
#include "../include/sys_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#define CPU0 1
#define CPU1 2
TaskHandle_t lvgl_handle;
void task_lvgl(void * arg);
void AppInit()
{
    xTaskCreatePinnedToCore(task_lvgl,"handler.task_lvgl",20480,NULL,1,&lvgl_handle,CPU0);
}
void task_lvgl(void * arg)
{
    for (;;)
    {
        //ESP_LOGI(TAG, "my module tag");
        vTaskDelay(5);
        lv_timer_handler();
    }
}