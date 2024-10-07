/*------------------------------------------------------------------------------
 * @file    MYGUI.C
 * @author  Hovey https://space.bilibili.com/33582262?spm_id_from=333.1007.0.0
 * @date    2024/08/17 18:07:24
 * @brief   
 * -----------------------------------------------------------------------------
 * @attention 
 
------------------------------------------------------------------------------*/
#include "../include/myGUI.h"
#include "../../Drivers/include/ST7701S.h"

#include <math.h>


/*statement static function*/

/*-------------------------------------------------------------*/
static lv_obj_t * GUI_modules[GUI_MODULES_NUMB] = {NULL}; 
LV_IMG_DECLARE(logo_huanglong);
LV_IMG_DECLARE(headshot);
LV_IMG_DECLARE(bilibili);
LV_IMG_DECLARE(wifi_logo);
static uint8_t lv_bg_status = 0;
static lv_point_t l_eye_points[] = {
    {LCD_H_RES/2-EYES_INTERVAL/2,EYES_Y_POS-EYES_LENTH_MAX/2},
    {LCD_H_RES/2-EYES_INTERVAL/2,EYES_Y_POS+EYES_LENTH_MAX/2}
};
static lv_point_t r_eye_points[] = {
    {LCD_H_RES/2+EYES_INTERVAL/2,EYES_Y_POS-EYES_LENTH_MAX/2},
    {LCD_H_RES/2+EYES_INTERVAL/2,EYES_Y_POS+EYES_LENTH_MAX/2}
};
static lv_point_t mouth_points[] = {
    {220,130},
    {260,130}
};
static lv_obj_t *face[] = {
    NULL,
    NULL,
    NULL
};
static LoadingWave RoboLoading;
static uint32_t now_fans_number;
static lv_obj_t * roller[5]; 
static Animation animation[] = {
    {  
        .duration = BLINK_duration,
        .repeat_delay = BLINK_repeat_delay,
        .repeat_cnt = BLINK_repeat_cnt,
        .step_value_range = {BLINK_step_value_start,BLINK_step_value_end},
        // .anim = NULL,
        .animation_exec_func = BlinkExec
    },
    {
        .duration = BOOTLOGO_duration,
        .repeat_delay = BOOTLOGO_repeat_delay,
        .repeat_cnt = BOOTLOGO_repeat_cnt,
        .step_value_range = {BOOTLOGO_step_value_start,BOOTLOGO_step_value_end},
        // .anim = NULL,
        .animation_exec_func = BootlogoExec,
        .animation_complete_func = BootLogo_complete_cb
    },
    {
        .duration = LOADINGWAVE_duration,
        .repeat_delay = LOADINGWAVE_repeat_delay,
        .repeat_cnt = LOADINGWAVE_repeat_cnt,
        .step_value_range = {LOADINGWAVE_step_value_start,LOADINGWAVE_step_value_end},
        // .anim = NULL,
        .animation_exec_func = LoadingWaveExec
    },
    {
        .duration = FANREPORT_duration,
        .repeat_delay = FANREPORT_repeat_delay,
        .repeat_cnt = FANREPORT_repeat_cnt,
        .step_value_range = {FANREPORT_step_value_start,FANREPORT_step_value_end},
        // .anim = NULL,
        .animation_exec_func = FanReportExec,
        .animation_complete_func = FanReport_complete_cb
    }
};
void GUI_module_init(uint8_t GUI_MODULES)
{
    if(GUI_MODULES < FANREPORT)
    {
        GUI_modules[GUI_MODULES] = lv_image_create(lv_screen_active());
        lv_bg_status = GUI_MODULES;
    }else{
        GUI_modules[GUI_MODULES] = lv_image_create(lv_layer_top());
    }
    // lv_obj_set_style_bg_color(GUI_modules[GUI_MODULES] , lv_color_black(), 0); 
    lv_obj_set_style_bg_color(GUI_modules[GUI_MODULES] , lv_color_make(0x00,0x00,0x00) , 0); 
    lv_obj_set_style_bg_opa(GUI_modules[GUI_MODULES], 255, LV_PART_MAIN);
    lv_obj_set_size(GUI_modules[GUI_MODULES], LCD_H_RES, LCD_V_RES);
    lv_obj_align(GUI_modules[GUI_MODULES], LV_ALIGN_CENTER, 0, 0);   
}
void GUI_module_delete(uint8_t GUI_MODULES)
{
    lv_anim_delete(NULL,animation[GUI_MODULES].animation_exec_func);
    lv_obj_del(GUI_modules[GUI_MODULES]); 
    GUI_modules[GUI_MODULES] = NULL;
}
void GUI_init(void)
{
    // Expression_init();
    // Blink();
    // LoadingWaveInit();
    // WaveLoading();
    // LoadingWaveExec(&RoboLoading,1);
    // LoadingWaveExec(&RoboLoading,1);
    // LoadingWaveExec(&RoboLoading,1);
}
int Blink(void)
{   
    lv_anim_start(&animation[EXPRESSION].anim);  //动画开始
    return 0;
}

int WaveLoading(void)
{
    lv_anim_start(&animation[LOADINGWAVE].anim);
    return 0;
}
void BootLogoDisplay(uint32_t playtime)
{
    GUI_module_init(BOOTLOGO);
    lv_obj_t * booting_logo = lv_image_create(GUI_modules[BOOTLOGO]);
    lv_image_set_src(booting_logo, &logo_huanglong);
    lv_obj_center(booting_logo);

    lv_anim_init(&animation[BOOTLOGO].anim);
    lv_anim_set_var(&animation[BOOTLOGO].anim, booting_logo);
    lv_anim_set_exec_cb(&animation[BOOTLOGO].anim, animation[BOOTLOGO].animation_exec_func); //运行函数
    lv_anim_set_completed_cb(&animation[BOOTLOGO].anim,animation[BOOTLOGO].animation_complete_func);
    lv_anim_set_time(&animation[BOOTLOGO].anim,playtime);//运行时间
    lv_anim_set_repeat_count(&animation[BOOTLOGO].anim, animation[BOOTLOGO].repeat_cnt);    //动画重复次数
    lv_anim_set_repeat_delay(&animation[BOOTLOGO].anim,  animation[BOOTLOGO].repeat_delay);  //重播延时时间
    lv_anim_set_values(&animation[BOOTLOGO].anim, animation[BOOTLOGO].step_value_range[0], animation[BOOTLOGO].step_value_range[1]); //value范围
    lv_anim_start(&animation[BOOTLOGO].anim);
}
void BootlogoExec(void * _obj, int v)
{
    static uint8_t opa_value = 0;
    lv_obj_t * obj = (lv_obj_t *)_obj;
    if(v <= 50)
    {
        opa_value = 5 * v;
    }
    else if(v >= 150 && v <= 200)
    {
        opa_value = (200 - v)*5;
    }else if(v > 200 ){
        opa_value = 0;
    }else{
        opa_value = 255;
    }
    lv_obj_set_style_image_opa(obj,opa_value,LV_PART_MAIN);
    // lv_style_set_opa(&style_grass, value_opa);
    // lv_obj_refresh_style(img01, &style_grass, 0);
}
void BootLogo_complete_cb(lv_anim_t * a)
{
    Expression_display();
}
/**
* @brief  初始化表情相关的lvgl部件
* @param  
* @return 
*/
void Expression_display()
{
    GUI_module_delete(lv_bg_status);
    GUI_module_init(EXPRESSION);

    face[LEFT_EYE]=lv_line_create(GUI_modules[EXPRESSION]);
    face[RIGHT_EYE]=lv_line_create(GUI_modules[EXPRESSION]);
    face[MOUTH] = lv_line_create(GUI_modules[EXPRESSION]);
    //左眼
    lv_line_set_points(face[LEFT_EYE],l_eye_points,2);
    lv_obj_set_style_line_width(face[LEFT_EYE],EYES_WIDTH,LV_PART_MAIN);
    lv_obj_set_style_line_rounded(face[LEFT_EYE],true,LV_PART_MAIN);
    lv_obj_set_style_line_color(face[LEFT_EYE],lv_palette_main(LV_PALETTE_BLUE),LV_PART_MAIN);
    //右眼
    lv_line_set_points(face[RIGHT_EYE],r_eye_points,2);
    lv_obj_set_style_line_width(face[RIGHT_EYE],EYES_WIDTH,LV_PART_MAIN);
    lv_obj_set_style_line_rounded(face[RIGHT_EYE],true,LV_PART_MAIN);
    lv_obj_set_style_line_color(face[RIGHT_EYE],lv_palette_main(LV_PALETTE_BLUE),LV_PART_MAIN);

    //嘴巴
    lv_line_set_points(face[MOUTH],mouth_points,2);
    lv_obj_set_style_line_width(face[MOUTH],10,LV_PART_MAIN);
    lv_obj_set_style_line_rounded(face[MOUTH],true,LV_PART_MAIN);
    lv_obj_set_style_line_color(face[MOUTH],lv_palette_main(LV_PALETTE_BLUE),LV_PART_MAIN);
    //动画
        
    lv_anim_init(&animation[EXPRESSION].anim);
    lv_anim_set_var(&animation[EXPRESSION].anim, face);
    lv_anim_set_exec_cb(&animation[EXPRESSION].anim, animation[EXPRESSION].animation_exec_func); //运行函数
    lv_anim_set_time(&animation[EXPRESSION].anim,animation[EXPRESSION].duration);//运行时间
    lv_anim_set_repeat_count(&animation[EXPRESSION].anim, animation[EXPRESSION].repeat_cnt);    //动画重复次数
    lv_anim_set_repeat_delay(&animation[EXPRESSION].anim,  animation[EXPRESSION].repeat_delay);  //重播延时时间
    lv_anim_set_values(&animation[EXPRESSION].anim, animation[EXPRESSION].step_value_range[0], animation[EXPRESSION].step_value_range[1]); //value范围
    lv_anim_start(&animation[EXPRESSION].anim);  //动画开始
}
void BlinkExec(void * _obj, int v)
{
    float y1,y2,k,t,c;
    int16_t y;
    y1 = EYES_LENTH_MAX/2;
    y2 = EYES_LENTH_MIN/2;
    t = BLINK_step_value_end * 0.6;
    k = (y1-y2)/((BLINK_step_value_end-BLINK_step_value_start)/2-t/2);
    c = y1 -BLINK_step_value_end * k;
    if(v < BLINK_step_value_end/2-t/2)
    {
        y = -k*v + y1;
    }
    else if(v > BLINK_step_value_end/2+t/2){
        y = k*v + c;
    }
    else{
        y = y2;
    }
    l_eye_points[0].y = EYES_Y_POS - y;
    l_eye_points[1].y = EYES_Y_POS + y;
    r_eye_points[0].y = EYES_Y_POS - y;
    r_eye_points[1].y = EYES_Y_POS + y;
    // line1_points[0].y = EYES_Y_POS - y;
    // line1_points[1].y = EYES_Y_POS + y;
    lv_obj_t ** obj = (lv_obj_t **)_obj;
    lv_line_set_points(obj[LEFT_EYE],l_eye_points,2);
    lv_line_set_points(obj[RIGHT_EYE],r_eye_points,2);
}

void LoadingWave_display()
{
    GUI_module_delete(lv_bg_status);
    GUI_module_init(LOADING);
    /*部件初始化*/
    float a = (float)(WAVE_MAX_HEIGHT - WAVE_MIN_HEIGHT)/((WAVES_TYPE_NUMB - 1)*(WAVES_TYPE_NUMB - 1));
    int delta_y = 0;
    for (int i = 0; i < WAVES_NUMB; i++)
    {
        RoboLoading.wave[i] = lv_line_create(GUI_modules[LOADING]); 
        RoboLoading.wave_point[i][0].x = (LCD_H_RES - WAVES_WIDTH)/2 + (1 + i) *(WAVES_WIDTH/(WAVES_NUMB+1));
        RoboLoading.wave_point[i][1].x = (LCD_H_RES - WAVES_WIDTH)/2 + (1 + i) *(WAVES_WIDTH/(WAVES_NUMB+1));  
        lv_obj_set_style_line_width(RoboLoading.wave[i],WAVES_WIDTH/9/2,LV_PART_MAIN);
        // lv_obj_set_style_line_rounded(RoboLoading.wave[i],true,LV_PART_MAIN);
        lv_obj_set_style_line_color(RoboLoading.wave[i],lv_palette_main(LV_PALETTE_BLUE),LV_PART_MAIN);
        if(i < WAVES_TYPE_NUMB)
        {
            delta_y = (i*i*a + WAVE_MIN_HEIGHT);
            delta_y = (delta_y > WAVES_MAX_LENTH/2-10) ? WAVES_MAX_LENTH/2-15 : delta_y;//限制最大值
            // RoboLoading.wave_point[i][0].y = LCD_V_RES/2 - delta_y;
            // RoboLoading.wave_point[i][1].y = LCD_V_RES/2 + delta_y;
        }
        else if(i >= WAVES_TYPE_NUMB)
        {
            int symmetry = WAVES_NUMB-1-i;
            delta_y = (symmetry*symmetry*a + WAVE_MIN_HEIGHT);            
        }
        RoboLoading.wave_point[i][0].y = LCD_V_RES/2 - delta_y;
        RoboLoading.wave_point[i][1].y = LCD_V_RES/2 + delta_y;
        lv_line_set_points(RoboLoading.wave[i],RoboLoading.wave_point[i],2);
    }
    /*动画初始化*/
    lv_anim_init(&animation[LOADINGWAVE].anim);
    lv_anim_set_var(&animation[LOADINGWAVE].anim,&RoboLoading);
    lv_anim_set_exec_cb(&animation[LOADINGWAVE].anim, animation[LOADINGWAVE].animation_exec_func); //运行函数
    lv_anim_set_time(&animation[LOADINGWAVE].anim,animation[LOADINGWAVE].duration);//运行时间
    lv_anim_set_repeat_count(&animation[LOADINGWAVE].anim, animation[LOADINGWAVE].repeat_cnt);    //动画重复次数
    lv_anim_set_repeat_delay(&animation[LOADINGWAVE].anim,  animation[LOADINGWAVE].repeat_delay);  //重播延时时间
    lv_anim_set_values(&animation[LOADINGWAVE].anim, animation[LOADINGWAVE].step_value_range[0], animation[LOADINGWAVE].step_value_range[1]); //value范围   
    lv_anim_start(&animation[LOADINGWAVE].anim);
}
void LoadingWaveExec(void * _obj, int v)
{
    // float a = (float)(WAVE_MAX_HEIGHT - WAVE_MIN_HEIGHT)/((WAVES_TYPE_NUMB - 1)*(WAVES_TYPE_NUMB - 1));
    LoadingWave * obj = _obj;
    for (int i = 0; i < WAVES_NUMB; i++)
    {
        // int delta_y = 0;
        // if(i < WAVES_TYPE_NUMB)
        // {
        //     delta_y = (i *i*a + WAVE_MIN_HEIGHT);
        //     delta_y = (delta_y > WAVES_MAX_LENTH/2-10) ? WAVES_MAX_LENTH/2-15 : delta_y;//限制最大值
        // }
        // else if(i >= WAVES_TYPE_NUMB)
        // {
        //     int symmetry = WAVES_NUMB-1-i;
        //     delta_y = (symmetry*symmetry*a + WAVE_MIN_HEIGHT);            
        // }
        // obj->wave_point[i][0].y = LCD_V_RES/2 - delta_y;
        // obj->wave_point[i][1].y = LCD_V_RES/2 + delta_y;


        if(i != WAVES_NUMB - 1)
        {
            obj->wave_point[i][0].y = obj->wave_point[i+1][0].y;
            obj->wave_point[WAVES_NUMB-1-i][1].y = obj->wave_point[WAVES_NUMB-2-i][1].y;
        }else{
            obj->wave_point[i][0].y = obj->wave_point[0][0].y;
            obj->wave_point[WAVES_NUMB-1-i][1].y = obj->wave_point[8][1].y;
        }
            
    }
    for (int j = 0; j < WAVES_NUMB; j++)
    {
        lv_line_set_points(obj->wave[j],obj->wave_point[j],2);    
    }
        
}

void FansReport(uint32_t _fans_numb,uint32_t _fans_numb_last)
{
    static lv_style_t style[5]; 
    
    // lv_style_set_bg_opa(&style, LV_OPA_TRANSP);
    const char * opts = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9";
    GUI_module_init(FANREPORT);
    lv_obj_t * headshot_pic = lv_image_create(GUI_modules[FANREPORT]);
    lv_image_set_src(headshot_pic, &headshot);
    lv_obj_set_pos(headshot_pic, 280, 200);
    

    lv_obj_t * bilibili_pic = lv_image_create(GUI_modules[FANREPORT]);
    lv_image_set_src(bilibili_pic, &bilibili);
    lv_obj_center(bilibili_pic);
    lv_obj_set_pos(bilibili_pic, 0, 120);

    lv_obj_t * my_id = lv_label_create(GUI_modules[FANREPORT]);
    lv_label_set_text(my_id, BILI_ID);
    lv_obj_set_pos(my_id, 240, 260);
    lv_obj_set_style_transform_rotation(my_id,1800,LV_PART_MAIN);
    lv_obj_set_style_text_font(my_id,&lv_font_montserrat_30,LV_PART_MAIN);
    lv_obj_set_style_text_color(my_id, lv_color_white(), 0);

    /*A roller on the left with left aligned text, and custom width*/
    for(int i = 0;i<5;++i)
    {
        lv_style_init(&style[i]);
        lv_style_set_bg_color(&style[i], lv_color_black());
        lv_style_set_text_color(&style[i], lv_color_make(0xff,0xc0,0xcb));
        lv_style_set_text_font(&style[i], &lv_font_montserrat_48);
        // lv_style_set_text_color(&style[i], lv_color_make(0xff,0x00,0x00));
        lv_style_set_border_width(&style[i], 0);
        lv_style_set_radius(&style[i], 0);
        roller[i] = lv_roller_create(GUI_modules[FANREPORT]);
        lv_roller_set_options(roller[i], opts, LV_ROLLER_MODE_INFINITE);
        lv_obj_set_width(roller[i], 40);
        lv_obj_add_style(roller[i], &style[i], 0);
        lv_obj_add_style(roller[i], &style[i], LV_PART_SELECTED);
        lv_roller_set_visible_row_count(roller[i], 1);
        lv_obj_set_pos(roller[i], 340-40*i, 160);
        lv_obj_set_style_transform_rotation(roller[i],1800,LV_PART_MAIN);
        // lv_roller_set_selected(roller[0], 3, LV_ANIM_ON);
        lv_obj_set_style_anim_time(roller[i], 1200, LV_PART_MAIN);
        lv_roller_set_selected(roller[i], (uint32_t)(_fans_numb_last/pow(10,4-i))%10, LV_ANIM_ON);
    }
    lv_anim_init(&animation[FANREPORT].anim);
    lv_anim_set_var(&animation[FANREPORT].anim, NULL);
    lv_anim_set_exec_cb(&animation[FANREPORT].anim, animation[FANREPORT].animation_exec_func); //运行函数
    lv_anim_set_completed_cb(&animation[FANREPORT].anim,animation[FANREPORT].animation_complete_func);
    lv_anim_set_time(&animation[FANREPORT].anim,animation[FANREPORT].duration);//运行时间
    lv_anim_set_repeat_count(&animation[FANREPORT].anim, animation[FANREPORT].repeat_cnt);    //动画重复次数
    lv_anim_set_repeat_delay(&animation[FANREPORT].anim,  animation[FANREPORT].repeat_delay);  //重播延时时间
    lv_anim_set_values(&animation[FANREPORT].anim, animation[FANREPORT].step_value_range[0], animation[FANREPORT].step_value_range[1]); //value范围
    lv_anim_start(&animation[FANREPORT].anim);
    now_fans_number = _fans_numb;
    // lv_obj_add_flag(GUI_modules[FANREPORT], LV_OBJ_FLAG_HIDDEN);
}
void FanReportExec(void * _obj, int v)
{
    if(v > 20 && v < 25)
    {
        for(int i = 0;i<5;++i)
        {
            // printf("bp\n");
            lv_roller_set_selected(roller[i], (uint32_t)(now_fans_number/pow(10,4-i))%10, LV_ANIM_ON);
        }
    }
}
void FanReport_complete_cb(lv_anim_t * a)
{
    // lv_obj_add_flag(GUI_modules[FANREPORT], LV_OBJ_FLAG_HIDDEN);
    // printf("end roller\n");
    GUI_module_delete(FANREPORT);
}
/*wifi status*/
void WiFiStatuReport(const char * _ssid)
{
    // GUI_module_init(WIFILOGO);
    lv_obj_t * wifi_pic = lv_image_create(lv_layer_top());
    lv_image_set_src(wifi_pic, &wifi_logo);
    lv_obj_center(wifi_pic);
    lv_obj_set_pos(wifi_pic, 50, 220);
    lv_obj_t * ssid = lv_label_create(lv_layer_top());
    lv_label_set_text(ssid, _ssid);
    lv_obj_set_pos(ssid, 270, 465);
    // lv_obj_center(ssid);
    lv_obj_set_style_transform_rotation(ssid,1800,LV_PART_MAIN);
    lv_obj_set_style_text_font(ssid,&lv_font_montserrat_14,LV_PART_MAIN);
    lv_obj_set_style_text_color(ssid, lv_color_white(), 0);
}