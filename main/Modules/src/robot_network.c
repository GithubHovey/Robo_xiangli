#include "../include/robot_network.h"
#include "../../Drivers/include/wifi.h"
#include "../../Modules/include/robot_voice.h"
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
    int ret = 0;
    for(;;)
    {
        if(xQueueReceive(NetworkQueueHandle,&rx_network_cmd,portMAX_DELAY) == pdPASS)
        {
            switch(rx_network_cmd.cmd)
            {
                case NETWORK_CMD_WIFI_CONNECT:
                    if(get_wifi_status()!=WIFI_ONLINE)
                    {
                        robot_wifi_connect("Archaludon","20220419"); 
                    }
                    GUICtrl(LOADING_ANIMATION_FINISH,NULL);
                    break;
                case NETWORK_CMD_FANS_REPORT:
                    GUICtrl(LOADING_ANIMATION_FINISH,NULL);
                    if(get_wifi_status()==WIFI_ONLINE)
                    {    
                        robot_net_status.fans_numb[1] = 0; 
                        robot_net_status.fans_numb[0] = GetFansNumb();
                        GUICtrl(FANS_REPORT,&robot_net_status.fans_numb);
                        PlayTargetVioce(TONE_TYPE_TONE2);
                    }
                    break;
                case NETWORK_CMD_FANS_UPDATE:
                    if(get_wifi_status()==WIFI_ONLINE)
                    {
                        robot_net_status.fans_numb[1] = robot_net_status.fans_numb[0]; 
                        robot_net_status.fans_numb[0] = GetFansNumb();
                        if(robot_net_status.fans_numb[0] != robot_net_status.fans_numb[1])
                        {
                            GUICtrl(FANS_REPORT,&robot_net_status.fans_numb);
                            PlayTargetVioce(TONE_TYPE_TONE2);
                        }
                    }  
                    break;
                case NETWORK_CMD_PC_CTRL:
                    if(PCPowerCtrl(*(int *)rx_network_cmd.user_data)==0)
                    {
                        PlayTargetVioce(TONE_TYPE_TONELOADING);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
void set_wifi_status(uint8_t _status,const char * _ssid)
{
    robot_net_status.Wifi_online_status = _status;
    robot_net_status.ssid = _ssid;
}
uint8_t get_wifi_status()
{
    return robot_net_status.Wifi_online_status;
}
uint32_t GetFansNumb(void)
{
    cJSON *rx_json = NULL;
    cJSON *data,*follower;
    uint32_t ret = 0;
    if(get_wifi_status() == WIFI_ONLINE)
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
            return ret;
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
                    ret = (uint32_t)follower->valueint;
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
    return ret;  
}
int robot_wifi_connect(const char *ssid,const char * passwd)
{
    int ret = WiFiConnect(ssid,passwd);
    if(ret)
    {
        set_wifi_status(WIFI_ONLINE,ssid);
        ESP_LOGI(MODULE_NETWORK, "wifi connect success:%s\n",ssid);
        GUICtrl(WIFI_CONNECT_FINISH,ssid);
        PlayTargetVioce(TONE_TYPE_TONE3);
    }
    return ret;
}
int PCPowerCtrl(int _cmd)
{
    char pc_control_token[160] = {0};
    cJSON *body = NULL;
    cJSON *rx_json = NULL;
    int content_length = 0;
    int ret = 0;
    static bool pc_online = false;
    esp_http_client_config_t config = {
        .url = "https://open.iot.vlinkc.com/api/open/pc/list"
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if(client == NULL)
    {
        ESP_LOGE(MODULE_NETWORK, "client == NULL");
        return ret;
    }
    // POST Request
    body =  cJSON_CreateObject();
    cJSON_AddStringToObject(body, "phone", "13229460306");
    cJSON_AddStringToObject(body, "password", "071da392728843b9ff08e8138e84414f");
    cJSON_AddNumberToObject(body, "expiryDate", 2080748036);
    char *post_data =  cJSON_Print(body);
    esp_http_client_set_url(client, "https://open.iot.vlinkc.com/api/open/pc/list");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_err_t err = esp_http_client_open(client, strlen(post_data));
    if (err != ESP_OK) {
        ESP_LOGE(MODULE_NETWORK, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    } else {
        int wlen = esp_http_client_write(client, post_data, strlen(post_data));
        if (wlen < 0) {
            ESP_LOGE(MODULE_NETWORK, "Write failed");
        }
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(MODULE_NETWORK, "HTTP client fetch headers failed");
        } else {
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(MODULE_NETWORK, "HTTP POST Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                ESP_LOGW(MODULE_NETWORK,"%s",output_buffer);
                rx_json = cJSON_Parse(output_buffer);//malloc
                if(rx_json == NULL)
                {
                    ESP_LOGE(MODULE_NETWORK, " rx_json json parse fail");
                }
                cJSON *data = cJSON_GetObjectItem(rx_json, "data");
                if(data == NULL)
                {
                    ESP_LOGE(MODULE_NETWORK, "data json parse fail");
                }
                cJSON *devices = cJSON_GetObjectItem(data, "devices");
                if(devices == NULL)
                {
                    ESP_LOGE(MODULE_NETWORK, "devices json parse fail");
                }
                cJSON *pc = cJSON_GetArrayItem(devices, 0);
                if(pc == NULL)
                {
                    ESP_LOGE(MODULE_NETWORK, "pc json parse fail");
                }
                cJSON *controlToken = cJSON_GetObjectItem(pc, "controlToken");
                if (controlToken && cJSON_IsString(controlToken)) {
                    strncpy(pc_control_token, controlToken->valuestring, sizeof(pc_control_token) - 1);
                    pc_control_token[sizeof(pc_control_token) - 1] = '\\0';  // 确保 null 终止
                } else {
                    strcpy(pc_control_token, "");
                }

            } else {
                ESP_LOGE(MODULE_NETWORK, "Failed to read response");
            }
        }
        if(body != NULL)
        {
            cJSON_Delete(body);
            body = NULL;
        }
        if(rx_json != NULL)
        {
            cJSON_Delete(rx_json);
            rx_json = NULL;
        }
        esp_http_client_close(client);
        strcpy(output_buffer, "");
    }
/*check pc status*/
    body =  cJSON_CreateObject();
    cJSON_AddStringToObject(body, "controlToken",pc_control_token);
    post_data =  cJSON_Print(body);
    ESP_LOGW(MODULE_NETWORK,"%s",post_data);
    esp_http_client_set_url(client, "https://open.iot.vlinkc.com/api/open/pc/status/get");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    err = esp_http_client_open(client, strlen(post_data));
    if (err != ESP_OK) {
        ESP_LOGE(MODULE_NETWORK, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    } else {
        int wlen = esp_http_client_write(client, post_data, strlen(post_data));
        if (wlen < 0) {
            ESP_LOGE(MODULE_NETWORK, "Write failed");
        }
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(MODULE_NETWORK, "HTTP client fetch headers failed");
        } else {
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(MODULE_NETWORK, "HTTP POST Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                ESP_LOGW(MODULE_NETWORK,"%s",output_buffer);
                rx_json = cJSON_Parse(output_buffer);//malloc
                if(rx_json == NULL)
                {
                    ESP_LOGE(MODULE_NETWORK, " rx_json json parse fail");
                }
                cJSON *data = cJSON_GetObjectItem(rx_json, "data");
                if(data == NULL)
                {
                    ESP_LOGE(MODULE_NETWORK, "data json parse fail");
                }
                cJSON* pc_power_status = cJSON_GetObjectItem(data, "power");
                if(pc_power_status == NULL)
                {
                    ESP_LOGE(MODULE_NETWORK, "data json parse fail");
                }
                pc_online = cJSON_IsTrue(pc_power_status);
            } else {
                ESP_LOGE(MODULE_NETWORK, "Failed to read response");
            }
        }
        if(body != NULL)
        {
            cJSON_Delete(body);
            body = NULL;
        }
        if(rx_json != NULL)
        {
            cJSON_Delete(rx_json);
            rx_json = NULL;
        }
        esp_http_client_close(client);
        strcpy(output_buffer, "");
    }
    


/*ctrl my PC power*/
#if 1
    if(!pc_online || _cmd > 0)
    {
        body =  cJSON_CreateObject();
        cJSON_AddNumberToObject(body, "action",_cmd);
        cJSON_AddStringToObject(body, "controlToken",pc_control_token);
        post_data =  cJSON_Print(body);
        ESP_LOGW(MODULE_NETWORK,"%s",post_data);
        esp_http_client_set_url(client, "https://open.iot.vlinkc.com/api/open/pc/status/set");
        esp_http_client_set_method(client, HTTP_METHOD_POST);
        esp_http_client_set_header(client, "Content-Type", "application/json");
        err = esp_http_client_open(client, strlen(post_data));
        if (err != ESP_OK) {
            ESP_LOGE(MODULE_NETWORK, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        } else {
            int wlen = esp_http_client_write(client, post_data, strlen(post_data));
            if (wlen < 0) {
                ESP_LOGE(MODULE_NETWORK, "Write failed");
            }
            content_length = esp_http_client_fetch_headers(client);
            if (content_length < 0) {
                ESP_LOGE(MODULE_NETWORK, "HTTP client fetch headers failed");
            } else {
                int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
                if (data_read >= 0) {
                    ESP_LOGI(MODULE_NETWORK, "HTTP POST Status = %d, content_length = %"PRId64,
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
                    ESP_LOGW(MODULE_NETWORK,"%s",output_buffer);
                    rx_json = cJSON_Parse(output_buffer);//malloc
                    if(rx_json == NULL)
                    {
                        ESP_LOGE(MODULE_NETWORK, " rx_json json parse fail");
                    }
                    cJSON *data = cJSON_GetObjectItem(rx_json, "data");
                    if(data == NULL)
                    {
                        ESP_LOGE(MODULE_NETWORK, "data json parse fail");
                    }
                    cJSON *result = cJSON_GetObjectItem(data, "result");
                    if (result == NULL || !cJSON_IsNumber(result)) {
                        ESP_LOGE(MODULE_NETWORK, "result json parse fail");
                    }else{
                        ret = result->valueint;
                    }

                } else {
                    ESP_LOGE(MODULE_NETWORK, "Failed to read response");
                }
            }
            if(body != NULL)
            {
                cJSON_Delete(body);
                body = NULL;
            }
            if(rx_json != NULL)
            {
                cJSON_Delete(rx_json);
                rx_json = NULL;
            }
            esp_http_client_close(client);
            strcpy(output_buffer, "");
        }
    }else{
        ret = 2;
        ESP_LOGW(MODULE_NETWORK, "PC is already online.");
    }
#endif
/*free mem*/
    if(body != NULL)
    {
        cJSON_Delete(body);
        body = NULL;
    }
    if(rx_json != NULL)
    {
        cJSON_Delete(rx_json);
        rx_json = NULL;
    }
    esp_http_client_cleanup(client);
    ESP_LOGW(MODULE_NETWORK, "power ctrl ret :%d",ret);
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