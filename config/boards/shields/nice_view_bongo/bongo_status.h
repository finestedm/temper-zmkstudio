#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

#define BONGO_WIDTH 160
#define BONGO_HEIGHT 68
#define BONGO_COLOR_FORMAT LV_COLOR_FORMAT_L8
#define BONGO_BUF_SIZE                                                                            \
    LV_CANVAS_BUF_SIZE(BONGO_WIDTH, BONGO_HEIGHT, LV_COLOR_FORMAT_GET_BPP(BONGO_COLOR_FORMAT),    \
                       LV_DRAW_BUF_STRIDE_ALIGN)

struct zmk_widget_bongo_status {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *canvas;
    uint8_t cbuf[BONGO_BUF_SIZE];
    uint8_t battery;
    uint8_t wpm;
    bool alternate_paw;
};

int zmk_widget_bongo_status_init(struct zmk_widget_bongo_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_bongo_status_obj(struct zmk_widget_bongo_status *widget);
