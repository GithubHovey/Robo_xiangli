/*This is tone file*/

const char* tone_uri[] = {
   "flash://tone/0_tone2.mp3",
   "flash://tone/1_tone3.mp3",
   "flash://tone/2_tone4.mp3",
   "flash://tone/3_tone_wake.mp3",
   "flash://tone/4_toneloading.mp3",
   "flash://tone/5_tonewakeold.mp3",
};

int get_tone_uri_num()
{
    return sizeof(tone_uri) / sizeof(char *) - 1;
}
