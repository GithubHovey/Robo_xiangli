/*------------------------------------------------------------------------------
 * @file    SYSINIT.C
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/08/27 10:27:30
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/
#include "../include/sys_init.h"
#include "../include/sys_internal.h"

void Module_init()
{
#if USE_SCREEN == 1
    ScreenInit();
#endif 
#if USE_AUDIO == 1
    Audio_init();
#endif 
#if USE_NETWORK == 1
    NetworkInit();
#endif
}