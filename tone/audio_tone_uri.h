#ifndef __AUDIO_TONEURI_H__
#define __AUDIO_TONEURI_H__

extern const char* tone_uri[];

typedef enum {
    TONE_TYPE_TONE2,
    TONE_TYPE_TONE3,
    TONE_TYPE_TONE4,
    TONE_TYPE_TONE_WAKE,
    TONE_TYPE_TONELOADING,
    TONE_TYPE_TONEWAKEOLD,
    TONE_TYPE_MAX,
} tone_type_t;

int get_tone_uri_num();

#endif
