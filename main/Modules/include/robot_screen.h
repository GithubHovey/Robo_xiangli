/*------------------------------------------------------------------------------
 * @file    SCREEN.H
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/09/15 17:04:07
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __SCREEN_H
#define __SCREEN_H

/* Files includes  -----------------------------------------------------------*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

/* Defines -------------------------------------------------------------------*/
enum GUI_CMD{
    WIFI_CONNECT_START,
    WIFI_CONNECT_FINISH,
    FANS_REPORT,
    START_LOGO,
    MAIN_INTERFACE
};
typedef struct _GUI_cmd{
    uint8_t cmd;
    void * user_data;
}GUI_cmd;

/* Variables -----------------------------------------------------------------*/
extern QueueHandle_t GUI_TxPort;
/* Functions ----------------------------------------------------------------*/
void ScreenInit();
// void screen_task(void *args);
void lvgl_task(void * arg);
#endif
