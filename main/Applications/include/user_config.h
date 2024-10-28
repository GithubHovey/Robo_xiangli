#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#define USE_SCREEN  1
#define USE_AUDIO   0
#define USE_NETWORK 1
#if USE_AUDIO == 1
    #define USE_ASR 1
#endif
#define USE_VOICE 1
#endif
