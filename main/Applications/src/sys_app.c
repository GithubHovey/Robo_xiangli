/*------------------------------------------------------------------------------
 * @file    SYS_APP.C
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/08/27 19:32:58
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/
#include "../include/sys_init.h"
#include "../include/sys_internal.h"

#define CPU0 0
#define CPU1 1
TaskHandle_t lvgl_handle;

TaskHandle_t audio_handle;
TaskHandle_t network_handle;
TaskHandle_t main_task_handle;
void Main_task(void * arg);
void play_startup_anim(uint32_t playtime);
void robot_wifi_connect(const char *ssid,const char * passwd);

void AppInit()
{
#if USE_SCREEN == 1
    xTaskCreatePinnedToCore(lvgl_task,"app.lvgl",20480,NULL,1,&lvgl_handle,CPU0);
#endif 
#if USE_AUDIO == 1
    xTaskCreatePinnedToCore(Audio_task,"app.audio",10240,NULL,1,&audio_handle,CPU1);
#endif 

#if USE_NETWORK == 1
    xTaskCreatePinnedToCore(NetworkTask,"app.network",10240,NULL,1,&network_handle,CPU1);
#endif 
    xTaskCreatePinnedToCore(Main_task,"app.main",2048,NULL,1,&main_task_handle,CPU1);
}
void Main_task(void * arg)
{
    const char* tag = pcTaskGetName(xTaskGetCurrentTaskHandle());
    ESP_LOGI(tag, "%s is created.",tag);
    play_startup_anim(7000);
    vTaskDelay(7000);
    robot_wifi_connect("Archaludon","20220419");
    for(;;)
    {
        vTaskDelay(10);
    }
}
void play_startup_anim(uint32_t playtime)
{
    GUI_cmd tx_gui_cmd = {
        .cmd = START_LOGO,
        .user_data = &playtime
    };
    xQueueSend(GUI_TxPort,&tx_gui_cmd,0);
    RobotVoicePlay(ROBOT_INFORM);
}
void robot_wifi_connect(const char *ssid,const char * passwd)
{

    GUI_cmd tx_gui_cmd = {
        .cmd = WIFI_CONNECT_START,
        .user_data = ssid
    };
    xQueueSend(GUI_TxPort,&tx_gui_cmd,0);
    if(WiFiConnect(ssid,passwd))
    {
        set_wifi_status(WIFI_ONLINE,ssid);
        printf("wifi connect success:%s\n",ssid);
    }
    tx_gui_cmd.cmd = WIFI_CONNECT_FINISH;
    xQueueSend(GUI_TxPort,&tx_gui_cmd,0);   
}