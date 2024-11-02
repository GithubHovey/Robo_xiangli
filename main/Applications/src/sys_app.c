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
// #include "driver/gpio.h"
#define CPU0 0
#define CPU1 1
TaskHandle_t lvgl_handle;

TaskHandle_t audio_handle;
TaskHandle_t network_handle;
TaskHandle_t main_task_handle;
void Main_task(void * arg);
void play_startup_anim(uint32_t playtime);

void AppInit()
{
#if USE_SCREEN == 1
    xTaskCreatePinnedToCore(lvgl_task,"app.lvgl",6144,NULL,5,&lvgl_handle,CPU1);
#endif 
#if USE_AUDIO == 1
    xTaskCreatePinnedToCore(Audio_task,"app.audio",3072,NULL,1,&audio_handle,CPU1);
#endif 

#if USE_NETWORK == 1
    xTaskCreatePinnedToCore(NetworkTask,"app.network",3584,NULL,5,&network_handle,CPU1);
#endif 
    xTaskCreatePinnedToCore(Main_task,"app.main",3584,NULL,5,&main_task_handle,CPU1);
#ifdef USE_ASR
    #if USE_ASR == 1
    // xTaskCreatePinnedToCore(&detect_Task, "app.detect", 4 * 1024, NULL, 1, NULL, CPU1);
    // xTaskCreatePinnedToCore(&HearingTask, "app.feed", 4 * 1024, NULL, 1, NULL, CPU1);
    #endif
#endif
}
void Main_task(void * arg)
{
    const char* tag = pcTaskGetName(xTaskGetCurrentTaskHandle());
    static char InfoBuffer[1024] = {0}; 
    static char TaskLoadingBuffer[1024] = {0};
    ESP_LOGI(tag, "%s is created.",tag);
    play_startup_anim(7000);
    vTaskDelay(7000);
    // robot_wifi_connect("Archaludon","20220419");
    
    while (1) {
        vTaskList((char *) &InfoBuffer);
        vTaskGetRunTimeStats((char *)&TaskLoadingBuffer); 
        printf("--------------\n");
        printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
        printf("\r\n%s\r\n", InfoBuffer);
        printf("任务名         运行计数        CPU使用率 \r\n");
        printf("=================================================\r\n");
        printf("%s\r\n",TaskLoadingBuffer);
        printf("=================================================\r\n");
        heap_caps_print_heap_info(MALLOC_CAP_8BIT);
        // gpio_dump_io_configuration(stdout, (1ULL << 8) | (1ULL << 18));
        if(get_wifi_status()==WIFI_ONLINE)
        {
            NetworkCtrl(NETWORK_CMD_FANS_UPDATE,NULL);
        }        
        vTaskDelay(10000);
    } 
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
    printf("startlogo:\n");
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    #if USE_AUDIO == 1
    RobotVoicePlay(ROBOT_INFORM);
    #endif
    PlayTargetVioce(TONE_TYPE_TONE4);
}
