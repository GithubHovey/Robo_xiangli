/*------------------------------------------------------------------------------
 * @file    ROBOT_HEARING.C
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/10/22 10:50:57
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/
#include "../include/robot_hearing.h"
#include "esp_process_sdkconfig.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "esp_board_init.h" //i2s_init
#include "model_path.h"

static esp_afe_sr_iface_t *afe_handle = NULL;
static esp_afe_sr_data_t *afe_data = NULL;
// static volatile int task_flag = 0;
srmodel_list_t *models = NULL;
int detect_flag = 0;
static int play_voice = -2;
void HearingInit()
{
    ESP_ERROR_CHECK(esp_board_init(16000, 1, 16));
    models = esp_srmodel_init("model");
    //i2s_init
    // ESP_ERROR_CHECK(esp_board_init(16000, 1, 16)); 
    //config AFE
    afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_SR_HANDLE; 
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.wakenet_model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
    afe_config.pcm_config.total_ch_num = 2;
    afe_config.pcm_config.mic_num = 1;
    afe_config.pcm_config.ref_num = 1;
    afe_config.aec_init = false;
    afe_config.se_init = false;
    afe_config.wakenet_mode = DET_MODE_90;
    afe_data = afe_handle->create_from_config(&afe_config);
    if(afe_data == NULL)
    {
        printf("afe_data == NULL\n");
    }else{
        printf("afe_data != NULL\n");
    }
    
}
void HearingTask(void* args)
{
//    esp_afe_sr_data_t *afe_data = arg;
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int nch = afe_handle->get_channel_num(afe_data);
    int feed_channel = esp_get_feed_channel();
    assert(nch <= feed_channel);
    int16_t *i2s_buff = malloc(audio_chunksize * sizeof(int16_t) * feed_channel);
    assert(i2s_buff);
    printf("audio_chunksize = %d , feed_channel = %d \n",audio_chunksize,feed_channel);

    while (1) {
        esp_get_feed_data(false, i2s_buff, audio_chunksize * sizeof(int16_t) * feed_channel);

        afe_handle->feed(afe_data, i2s_buff);
    }
    if (i2s_buff) {
        free(i2s_buff);
        i2s_buff = NULL;
    }
    vTaskDelete(NULL);
}

void detect_Task(void *arg)
{
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    //detect time
    char *mn_name = esp_srmodel_filter(models, ESP_MN_PREFIX, ESP_MN_CHINESE);
    printf("multinet:%s\n", mn_name);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(mn_name);
    printf("multinet->create before:\n");
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    model_iface_data_t *model_data = multinet->create(mn_name, 6000);
    printf("multinet->create after:\n");
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data); // Add speech commands from sdkconfig
    int mu_chunksize = multinet->get_samp_chunksize(model_data);
    assert(mu_chunksize == afe_chunksize);

    //print active speech commands
    multinet->print_active_speech_commands(model_data);
    printf("detect\n");
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    printf("------------detect start------------\n");
    // FILE *fp = fopen("/sdcard/out1", "w");
    // if (fp == NULL) printf("can not open file\n");
    while (1) {
        afe_fetch_result_t* res = afe_handle->fetch(afe_data); 
        if (!res || res->ret_value == ESP_FAIL) {
            printf("fetch error!\n");
            break;
        }
#if CONFIG_IDF_TARGET_ESP32
        if (res->wakeup_state == WAKENET_DETECTED) {
            printf("wakeword detected\n");
            play_voice = -1;
            detect_flag = 1;
            afe_handle->disable_wakenet(afe_data);
            printf("-----------listening-----------\n");
        }
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32P4
        if (res->wakeup_state == WAKENET_DETECTED) {
            printf("WAKEWORD DETECTED\n");
	    multinet->clean(model_data);  // clean all status of multinet
        } else if (res->wakeup_state == WAKENET_CHANNEL_VERIFIED) {
            play_voice = -1;
            detect_flag = 1;
            printf("AFE_FETCH_CHANNEL_VERIFIED, channel index: %d\n", res->trigger_channel_id);
        }
#endif

        if (detect_flag == 1) {
            esp_mn_state_t mn_state = multinet->detect(model_data, res->data);

            if (mn_state == ESP_MN_STATE_DETECTING) {
                continue;
            }

            if (mn_state == ESP_MN_STATE_DETECTED) {
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                for (int i = 0; i < mn_result->num; i++) {
                    printf("TOP %d, command_id: %d, phrase_id: %d, string:%s prob: %f\n", 
                    i+1, mn_result->command_id[i], mn_result->phrase_id[i], mn_result->string, mn_result->prob[i]);
                }
                printf("\n-----------listening-----------\n");
            }

            if (mn_state == ESP_MN_STATE_TIMEOUT) {
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                printf("timeout, string:%s\n", mn_result->string);
                afe_handle->enable_wakenet(afe_data);
                detect_flag = 0;
                printf("\n-----------awaits to be waken up-----------\n");
                continue;
            }
        }
    }
    if (model_data) {
        multinet->destroy(model_data);
        model_data = NULL;
    }
    printf("detect exit\n");
    vTaskDelete(NULL);
}

