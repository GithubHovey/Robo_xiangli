#ifndef WIFI_H
#define WIFI_H
#ifdef __cplusplus
extern "C"
{
#endif

void Wifi_Init();
int wifi_init_sta(const char *_ssid,const char *_passwd);
#ifdef __cplusplus
}
#endif
#endif