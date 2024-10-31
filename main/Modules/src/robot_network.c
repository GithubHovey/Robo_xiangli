#include "../include/robot_network.h"
#include "../../Drivers/include/wifi.h"
#include "../include/robot_screen.h"
#include "cJSON.h"
#include "esp_http_client.h"
#include "lwip/err.h"
#include "lwip/sys.h"
static const char* MODULE_NETWORK = "[Module-network]";
QueueHandle_t NetworkQueueHandle;
// #define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 512
static char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};   // Buffer to store response of http request
static robot_network_status robot_net_status = {
    .Wifi_online_status = 0,
    .fans_numb = {0}
};
void NetworkInit()
{
    Wifi_Init();
    NetworkQueueHandle = xQueueCreate(4, sizeof(GUI_cmd));
}
int WiFiConnect(const char *ssid,const char * passwd)
{
    return wifi_init_sta(ssid,passwd);
}
void NetworkTask(void *args)
{
    static Network_cmd rx_network_cmd;
    static GUI_cmd tx_gui_cmd;
    for(;;)
    {
        if(xQueueReceive(NetworkQueueHandle,&rx_network_cmd,portMAX_DELAY) == pdPASS)
        {
            switch(rx_network_cmd.cmd)
            {
                case NETWORK_CMD_WIFI_CONNECT:
                    if(gei_wifi_status()!=WIFI_ONLINE)
                    {
                        robot_wifi_connect("Archaludon","20220419");
                    }
                    break;
                case NETWORK_CMD_FANS_REPORT:
                    break;
                case NETWORK_CMD_PC_CTRL:
                    break;
                default:
                    break;
            }
        }
        // robot_net_status.fans_numb[1] = robot_net_status.fans_numb[0]; 
        // FansUpdate();
        // if(robot_net_status.fans_numb[0]!=robot_net_status.fans_numb[1])
        // {
        //     tx_gui_cmd.cmd = FANS_REPORT;
        //     tx_gui_cmd.user_data = &robot_net_status.fans_numb;
        //     xQueueSend(GUI_TxPort,&tx_gui_cmd,0);
        // }
        // vTaskDelay(10000);
    }
}
void set_wifi_status(uint8_t _status,const char * _ssid)
{
    robot_net_status.Wifi_online_status = _status;
    robot_net_status.ssid = _ssid;
}
uint8_t gei_wifi_status()
{
    return robot_net_status.Wifi_online_status;
}
void FansUpdate(void)
{
    static cJSON *rx_json,*data,*follower;
    
    if(gei_wifi_status() == WIFI_ONLINE)
    {
        // Declare local_response_buffer with size (MAX_HTTP_OUTPUT_BUFFER + 1) to prevent out of bound access when
    // it is used by functions like strlen(). The buffer should only be used upto size MAX_HTTP_OUTPUT_BUFFER
    
    int content_length = 0;
    esp_http_client_config_t config = {
        // .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/get",
        .url = "https://api.bilibili.com/x/relation/stat?vmid=33582262"
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if(client == NULL)
    {
        ESP_LOGE(MODULE_NETWORK, "client == NULL");
        return;
    }
    // GET Request
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(MODULE_NETWORK, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(MODULE_NETWORK, "HTTP client fetch headers failed");
        } else {
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(MODULE_NETWORK, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                // ESP_LOG_BUFFER_HEX(MODULE_NETWORK, output_buffer, data_read);
                // ESP_LOGW(MODULE_NETWORK,"%s",output_buffer);
                rx_json = cJSON_Parse(output_buffer);//malloc
                if(rx_json == NULL)
                {
                    ESP_LOGE(MODULE_NETWORK, "json parse fail");
                    goto finish;
                }
                data = cJSON_GetObjectItem(rx_json, "data");
                if(data == NULL)
                {
                    ESP_LOGE(MODULE_NETWORK, "json parse fail");
                    goto finish;
                }
                cJSON *follower = cJSON_GetObjectItem(data, "follower");
                if (follower == NULL || !cJSON_IsNumber(follower)) {
                    ESP_LOGE(MODULE_NETWORK, "json parse fail");
                    goto finish;
                }
                ESP_LOGW(MODULE_NETWORK, "follower: %lu",(uint32_t)follower->valueint);
                robot_net_status.fans_numb[0] = (uint32_t)follower->valueint;
            } else {
                ESP_LOGE(MODULE_NETWORK, "Failed to read response");
            }
        }
    }
finish:
    if(rx_json != NULL)
    {
        cJSON_Delete(rx_json);
        rx_json = NULL;
    }
    // heap_caps_print_heap_info(MALLOC_CAP_8BIT);    
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    }  
}
int robot_wifi_connect(const char *ssid,const char * passwd)
{

    // GUI_cmd tx_gui_cmd = {
    //     .cmd = WIFI_CONNECT_START,
    //     .user_data = ssid
    // };
    // xQueueSend(GUI_TxPort,&tx_gui_cmd,0);
    int ret = WiFiConnect(ssid,passwd);
    if(ret)
    {
        set_wifi_status(WIFI_ONLINE,ssid);
        printf("wifi connect success:%s\n",ssid);
        // GUI_cmd tx_gui_cmd = {
        //     .cmd = WIFI_CONNECT_FINISH,
        //     .user_data = ssid
        // };
        // xQueueSend(GUI_TxPort,&tx_gui_cmd,0);
        GUICtrl(WIFI_CONNECT_FINISH,ssid);
    }
    return ret;
}
void NetworkCtrl(uint8_t _cmd,void *_user_data)
{
    Network_cmd tx_gui_cmd = {
        .cmd = _cmd,
        .user_data = _user_data
    };
    xQueueSend(NetworkQueueHandle,&tx_gui_cmd,0);
}