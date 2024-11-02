/*------------------------------------------------------------------------------
 * @file    ROBOT_NETWORK.H
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/09/19 21:44:20
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __ROBOT_NETWORK_H
#define __ROBOT_NETWORK_H

/* Files includes  -----------------------------------------------------------*/
// #include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
/* Defines -------------------------------------------------------------------*/
enum WIFI_STATUS{
    WIFI_OFFLINE,
    WIFI_ONLINE
};
typedef struct _robot_network_status{
    uint8_t Wifi_online_status;
    uint32_t fans_numb[2]; //0 :now 1: last
    char * ssid;
}robot_network_status;
enum NETWORK_CMD{
    NETWORK_CMD_WIFI_CONNECT,
    NETWORK_CMD_FANS_REPORT,
    NETWORK_CMD_FANS_UPDATE,
    NETWORK_CMD_PC_CTRL
};
typedef struct _Network_cmd{
    uint8_t cmd;
    void * user_data;
}Network_cmd;
/* Variables -----------------------------------------------------------------*/


/* Functions ----------------------------------------------------------------*/
void NetworkInit();
int WiFiConnect(const char *ssid,const char * passwd);
void NetworkTask(void *args);
void set_wifi_status(uint8_t _status,const char * _ssid);
uint8_t get_wifi_status();
uint32_t GetFansNumb(void);
int PCPowerCtrl(int _cmd);
int robot_wifi_connect(const char *ssid,const char * passwd);
void NetworkCtrl(uint8_t _cmd,void *_user_data);
#endif
