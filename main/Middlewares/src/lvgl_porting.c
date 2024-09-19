#include "../include/lvgl_porting.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "../../Drivers/include/ST7701S.h"
#include "lvgl.h"
#define LVGL_TICK_PERIOD_MS     20
static const char* TAG_lvgl_porting = "[Middleware-lvgl_porting]";
#ifdef LVGL_V8
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void increase_lvgl_tick(void *arg);
static lv_disp_drv_t disp_drv;      // contains callback functions
static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)


void Enable_lvgl_for_screen(void * src_obj)
{
    ESP_LOGI(TAG_lvgl_porting, "Initialize LVGL library");
    lv_init();
    void *buf1 = NULL;
    void *buf2 = NULL;
#if CONFIG_DOUBLE_FB
    ESP_LOGI(TAG_lvgl_porting, "Use frame buffers as LVGL draw buffers");
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * LCD_V_RES);
#else
    ESP_LOGI(TAG_lvgl_porting, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = heap_caps_malloc(LCD_H_RES * LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(LCD_H_RES * LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * LCD_V_RES);
#endif // CONFIG_DOUBLE_FB

    ESP_LOGI(TAG_lvgl_porting, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = src_obj;
#if CONFIG_DOUBLE_FB
    disp_drv.full_refresh = true; // the full_refresh mode can maintain the synchronization between the two frame buffers
#endif
    //lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
    lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG_lvgl_porting, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
        esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));
}



static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    flush_color((ST7701S_handle)drv->user_data, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);
    lv_disp_flush_ready(drv);
}
#endif
static void lvgl_flush_cb(lv_display_t * display, const lv_area_t * area, void * px_map);
static void increase_lvgl_tick(void *arg);
// lv_display_t * display = NULL;
static ST7701S_handle  driver = NULL;
void Enable_lvgl_for_screen(void * src_obj)
{
    void *buf1 = NULL;
    void *buf2 = NULL;
    driver = (ST7701S_handle)src_obj;
    ESP_LOGI(TAG_lvgl_porting, "Initialize LVGL library");
    lv_init();
    lv_display_t * display = lv_display_create(LCD_H_RES, LCD_V_RES);
    ESP_LOGI(TAG_lvgl_porting, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = heap_caps_malloc(LCD_H_RES * LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(LCD_H_RES * LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    lv_display_set_buffers(display, buf1, buf2, LCD_H_RES * LCD_V_RES* sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(display, lvgl_flush_cb);

    ESP_LOGI(TAG_lvgl_porting, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
        esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

}
static void lvgl_flush_cb(lv_display_t * display, const lv_area_t * area, void * px_map)
{
    flush_color(driver, area->x1, area->y1, area->x2 + 1, area->y2 + 1, px_map);
    lv_disp_flush_ready(display);
}
static void increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}