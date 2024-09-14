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

#define CPU0 1
#define CPU1 2
TaskHandle_t lvgl_handle;
TaskHandle_t audio_handle;
void task_lvgl(void * arg);
// void task_audio(void * arg);
void AppInit()
{
#if USE_SCREEN == 1
    xTaskCreatePinnedToCore(task_lvgl,"app.lvgl",20480,NULL,1,&lvgl_handle,CPU0);
#endif 
#if USE_AUDIO == 1
    // xTaskCreatePinnedToCore(task_audio,"app._audio",20480,NULL,1,&lvgl_handle,CPU0);
    xTaskCreatePinnedToCore(Audio_task,"app.audio",20480,NULL,1,&lvgl_handle,CPU0);
#endif 
   
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
// #define EXAMPLE_BUFF_SIZE               2048
// void task_audio(void * arg)
// {
//     static const char*  task_name = "app.audio";
//     ESP_LOGI(task_name, "%s is created.",task_name);
//     uint8_t *w_buf = (uint8_t *)calloc(1, EXAMPLE_BUFF_SIZE);
//     assert(w_buf); // Check if w_buf allocation success
//     MAX98357_Init();
//     /* Assign w_buf */
//     for (int i = 0; i < EXAMPLE_BUFF_SIZE; i += 8) {
//         // w_buf[i]     = rand()%256;
//         // w_buf[i + 1] = rand()%256;
//         // w_buf[i + 2] = rand()%256;
//         // w_buf[i + 3] = rand()%256;
//         // w_buf[i + 4] = rand()%256;
//         // w_buf[i + 5] = rand()%256;
//         // w_buf[i + 6] = rand()%256;
//         // w_buf[i + 7] = rand()%256;
//         w_buf[i]     = 0;
//         w_buf[i + 1] = 0;
//         w_buf[i + 2] = 0;
//         w_buf[i + 3] = 0;
//         w_buf[i + 4] = 0;
//         w_buf[i + 5] = 0;
//         w_buf[i + 6] = 0;
//         w_buf[i + 7] = 0;
//     }

//     size_t w_bytes = 20480;

//     /* (Optional) Preload the data before enabling the TX channel, so that the valid data can be transmitted immediately */
//     while (w_bytes == EXAMPLE_BUFF_SIZE) {
//         /* Here we load the target buffer repeatedly, until all the DMA buffers are preloaded */
//         ESP_ERROR_CHECK(i2s_channel_preload_data(tx_chan, w_buf, EXAMPLE_BUFF_SIZE, &w_bytes));
//     }

//     /* Enable the TX channel */
//     ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
//     while (1) {
//         /* Write i2s data */
//         if (i2s_channel_write(tx_chan, w_buf, EXAMPLE_BUFF_SIZE, &w_bytes, 1000) == ESP_OK) {
//             printf("Write Task: i2s write %d bytes\n", w_bytes);
//         } else {
//             printf("Write Task: i2s write failed\n");
//         }
//         vTaskDelay(pdMS_TO_TICKS(200));
//     }
//     free(w_buf);
//     vTaskDelete(NULL);
// }