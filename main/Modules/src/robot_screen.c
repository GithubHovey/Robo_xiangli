/*------------------------------------------------------------------------------
 * @file    SYSINIT.C
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/08/27 10:27:30
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/
#include "../include/robot_screen.h"

#include "../../Middlewares/include/lvgl_porting.h"
#include "../../Middlewares/include/myGUI.h"
#include "../../Drivers/include/ST7701S.h"

static const char* MODULE_SCREEN = "[Module-screen]";
QueueHandle_t GUI_TxPort;

static ST7701S_handle st7701s = NULL;
void ScreenInit()
{
    st7701s = ST7701S_newObject();    
    ST7701S_Init(st7701s); 
    ESP_LOGI(MODULE_SCREEN, "ST7701S_Init success.");
    Enable_lvgl_for_screen(st7701s);
    GUI_TxPort = xQueueCreate(4, sizeof(GUI_cmd));
    ESP_LOGI(MODULE_SCREEN, "Screen init success.");
}
void lvgl_task(void * arg)
{
    const char* tag = pcTaskGetName(xTaskGetCurrentTaskHandle());
    ESP_LOGI(tag, "%s is created.",tag);
    // GUI_init();
    static GUI_cmd rx_gui_cmd;
    for (;;)
    {
        if(xQueueReceive(GUI_TxPort,&rx_gui_cmd,0) == pdPASS)
        {
            switch(rx_gui_cmd.cmd)
            {
                case WIFI_CONNECT_START:
                    // LoadingWave_display();
                    break;
                case WIFI_CONNECT_FINISH:
                    // Expression_display();
                    WiFiStatuReport((char *)(rx_gui_cmd.user_data));
                    break;
                case FANS_REPORT:
                    uint32_t *data = (uint32_t *)(rx_gui_cmd.user_data);
                    FansReport(data[0],data[1]);
                    break; 
                case START_LOGO:
                    BootLogoDisplay(3000);
                    break;
                case MAIN_INTERFACE:
                    break;
                case LOADING_ANIMATION_START:
                    LoadingWave_display();
                    break;
                case LOADING_ANIMATION_FINISH:
                    Expression_display();
                    break;
                default:
                    break;
            }
        }
        vTaskDelay(5);
        lv_timer_handler();
    }
}
void GUICtrl(uint8_t _cmd,void *_user_data)
{
    GUI_cmd tx_gui_cmd = {
        .cmd = _cmd,
        .user_data = _user_data
    };
    xQueueSend(GUI_TxPort,&tx_gui_cmd,0);
}
