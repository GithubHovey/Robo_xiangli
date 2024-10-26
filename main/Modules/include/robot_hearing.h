/*------------------------------------------------------------------------------
 * @file    ROBOT_HEARING.H
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/10/22 10:51:32
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __ROBOT_HEARING_H
#define __ROBOT_HEARING_H

/* Files includes  -----------------------------------------------------------*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/


/* Functions ----------------------------------------------------------------*/
void HearingInit();
void HearingTask(void* args);
void detect_Task(void *arg);
#endif
