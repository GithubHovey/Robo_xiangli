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




/*statement static function*/
static void Expression_init();
static void BlinkExec(void * _obj, int v);
static void LoadingWaveInit();
static void LoadingWaveExec(void * _obj, int v);

/*-------------------------------------------------------------*/

static lv_point_t l_eye_points[] = {
    {LCD_H_RES/2-EYES_INTERVAL/2,EYES_Y_POS-EYES_LENTH_MAX/2},
    {LCD_H_RES/2-EYES_INTERVAL/2,EYES_Y_POS+EYES_LENTH_MAX/2}
};
static lv_point_t r_eye_points[] = {
    {LCD_H_RES/2+EYES_INTERVAL/2,EYES_Y_POS-EYES_LENTH_MAX/2},
    {LCD_H_RES/2+EYES_INTERVAL/2,EYES_Y_POS+EYES_LENTH_MAX/2}
};
static lv_point_t mouth_points[] = {
    {220,350},
    {260,350}
};
static lv_obj_t *face[] = {
    NULL,
    NULL,
    NULL
};
static LoadingWave RoboLoading;
static Animation animation[] = {
    {  
        .duration = BLINK_duration,
        .repeat_delay = BLINK_repeat_delay,
        .repeat_cnt = BLINK_repeat_cnt,
        .step_value_range = {BLINK_step_value_start,BLINK_step_value_end},
        // .anim = NULL,
        .animation_exec_func = BlinkExec,
        .lock = 0
    },
    {
        .duration = LOADINGWAVE_duration,
        .repeat_delay = LOADINGWAVE_repeat_delay,
        .repeat_cnt = LOADINGWAVE_repeat_cnt,
        .step_value_range = {LOADINGWAVE_step_value_start,LOADINGWAVE_step_value_end},
        // .anim = NULL,
        .animation_exec_func = LoadingWaveExec,
        .lock = 0
    }
};

/**
* @brief  
* @param  
* @return 
*/
void GUI_init(void)
{
    // Expression_init();
    // Blink();
    LoadingWaveInit();
    WaveLoading();
    // LoadingWaveExec(&RoboLoading,1);
    // LoadingWaveExec(&RoboLoading,1);
    // LoadingWaveExec(&RoboLoading,1);
}

int Blink(void)
{   
    lv_anim_start(&animation[BLINK].anim);  //动画开始
    return 0;
}

int WaveLoading(void)
{
    lv_anim_start(&animation[LOADINGWAVE].anim);
    return 0;
}
/**
* @brief  初始化表情相关的lvgl部件
* @param  
* @return 
*/
static void Expression_init()
{
    face[LEFT_EYE]=lv_line_create(lv_scr_act());
    face[RIGHT_EYE]=lv_line_create(lv_scr_act());
    face[MOUTH] = lv_line_create(lv_scr_act());
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
        
    lv_anim_init(&animation[BLINK].anim);
    lv_anim_set_var(&animation[BLINK].anim, face);
    lv_anim_set_exec_cb(&animation[BLINK].anim, animation[BLINK].animation_exec_func); //运行函数
    lv_anim_set_time(&animation[BLINK].anim,animation[BLINK].duration);//运行时间
    lv_anim_set_repeat_count(&animation[BLINK].anim, animation[BLINK].repeat_cnt);    //动画重复次数
    lv_anim_set_repeat_delay(&animation[BLINK].anim,  animation[BLINK].repeat_delay);  //重播延时时间
    lv_anim_set_values(&animation[BLINK].anim, animation[BLINK].step_value_range[0], animation[BLINK].step_value_range[1]); //value范围
    
}
static void LoadingWaveInit()
{
    /*部件初始化*/
    float a = (float)(WAVE_MAX_HEIGHT - WAVE_MIN_HEIGHT)/((WAVES_TYPE_NUMB - 1)*(WAVES_TYPE_NUMB - 1));
    int delta_y = 0;
    for (int i = 0; i < WAVES_NUMB; i++)
    {
        RoboLoading.wave[i] = lv_line_create(lv_scr_act()); 
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
    
}
/**
* @brief  Blink的分步函数
* @param  
* @return 
*/
static void BlinkExec(void * _obj, int v)
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

/**
* @brief  
* @param  
* @return 
*/
static void LoadingWaveExec(void * _obj, int v)
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