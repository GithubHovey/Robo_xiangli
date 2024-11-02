/*------------------------------------------------------------------------------
 * @file    ROBOT_VOICE.H
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/10/28 22:18:09
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __ROBOT_VOICE_H
#define __ROBOT_VOICE_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>
#include "audio_tone_uri.h"

/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/


/* Functions ----------------------------------------------------------------*/
void RobotVoiceInit();
void RobotCommunicationTask(void* args);
void PlayTargetVioce(uint8_t voice_id);
#endif
