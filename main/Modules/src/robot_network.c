#include "../include/robot_network.h"
#include "../../Drivers/include/wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
void NetworkInit()
{
    Wifi_Init();
    wifi_init_sta("Archaludon","20220419");
}
void WiFiConnect(const char *ssid,const char * passwd)
{

}
void NetworkTask(void *args)
{
    for(;;)
    {
        
    }
}