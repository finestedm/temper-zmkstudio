#include "bongo_status.h"

#include <stdio.h>
#include <string.h>

#include <zephyr/logging/log.h>

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define BONGO_BACKGROUND lv_color_white()
#define BONGO_FOREGROUND lv_color_black()

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct bongo_battery_state {
    uint8_t level;
};

struct bongo_wpm_state {
    uint8_t wpm;
};

struct bongo_key_state {
    bool pressed;
};

static void init_rect(lv_draw_rect_dsc_t *dsc, lv_color_t fill, int border_width,
                      int radius) {
    lv_draw_rect_dsc_init(dsc);
    dsc->bg_color = fill;
    dsc->bg_opa = LV_OPA_COVER;
    dsc->border_color = BONGO_FOREGROUND;
    dsc->border_width = border_width;
    dsc->radius = radius;
}

static void init_line(lv_draw_line_dsc_t *dsc, int width) {
    lv_draw_line_dsc_init(dsc);
    dsc->color = BONGO_FOREGROUND;
    dsc->width = width;
    dsc->round_start = true;
    dsc->round_end = true;
}

static void init_label(lv_draw_label_dsc_t *dsc, const lv_font_t *font,
                       lv_text_align_t align) {
    lv_draw_label_dsc_init(dsc);
    dsc->color = BONGO_FOREGROUND;
    dsc->font = font;
    dsc->align = align;
}

static void draw_rect(lv_obj_t *canvas, int x, int y, int width, int height,
                      lv_draw_rect_dsc_t *dsc) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_area_t coords = {x, y, x + width - 1, y + height - 1};
    lv_draw_rect(&layer, dsc, &coords);
    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_line(lv_obj_t *canvas, const lv_point_t points[], uint32_t point_count,
                      lv_draw_line_dsc_t *dsc) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    for (uint32_t i = 1; i < point_count; i++) {
        dsc->p1.x = points[i - 1].x;
        dsc->p1.y = points[i - 1].y;
        dsc->p2.x = points[i].x;
        dsc->p2.y = points[i].y;
        lv_draw_line(&layer, dsc);
    }

    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_text(lv_obj_t *canvas, int x, int y, int width, lv_draw_label_dsc_t *dsc,
                      const char *text) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    dsc->text = text;
    lv_area_t coords = {x, y, x + width - 1, BONGO_HEIGHT - 1};
    lv_draw_label(&layer, dsc, &coords);
    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_battery(lv_obj_t *canvas, uint8_t level) {
    lv_draw_rect_dsc_t outline;
    lv_draw_rect_dsc_t empty;
    lv_draw_rect_dsc_t fill;

    init_rect(&outline, BONGO_FOREGROUND, 0, 0);
    init_rect(&empty, BONGO_BACKGROUND, 0, 0);
    init_rect(&fill, BONGO_FOREGROUND, 0, 0);

    draw_rect(canvas, 3, 4, 27, 11, &outline);
    draw_rect(canvas, 5, 6, 23, 7, &empty);
    draw_rect(canvas, 30, 7, 3, 5, &outline);

    int fill_width = (level * 21 + 99) / 100;
    if (fill_width > 0) {
        draw_rect(canvas, 6, 7, fill_width, 5, &fill);
    }
}

static void draw_cat(lv_obj_t *canvas, bool alternate_paw) {
    lv_draw_rect_dsc_t outline;
    lv_draw_rect_dsc_t black;
    lv_draw_line_dsc_t thick_line;
    lv_draw_line_dsc_t thin_line;

    init_rect(&outline, BONGO_BACKGROUND, 2, 8);
    init_rect(&black, BONGO_FOREGROUND, 0, 1);
    init_line(&thick_line, 3);
    init_line(&thin_line, 2);

    const lv_point_t left_ear[] = {{51, 18}, {56, 7}, {68, 16}};
    const lv_point_t right_ear[] = {{96, 16}, {106, 7}, {111, 19}};
    draw_line(canvas, left_ear, 3, &thick_line);
    draw_line(canvas, right_ear, 3, &thick_line);

    draw_rect(canvas, 50, 14, 63, 39, &outline);

    draw_rect(canvas, 65, 27, 4, 6, &black);
    draw_rect(canvas, 94, 27, 4, 6, &black);

    const lv_point_t mouth_left[] = {{76, 37}, {81, 41}, {86, 37}};
    const lv_point_t mouth_right[] = {{86, 37}, {91, 41}, {96, 37}};
    draw_line(canvas, mouth_left, 3, &thin_line);
    draw_line(canvas, mouth_right, 3, &thin_line);

    const lv_point_t whisker_left_top[] = {{56, 36}, {43, 33}};
    const lv_point_t whisker_left_bottom[] = {{56, 41}, {42, 44}};
    const lv_point_t whisker_right_top[] = {{107, 36}, {120, 33}};
    const lv_point_t whisker_right_bottom[] = {{107, 41}, {121, 44}};
    draw_line(canvas, whisker_left_top, 2, &thin_line);
    draw_line(canvas, whisker_left_bottom, 2, &thin_line);
    draw_line(canvas, whisker_right_top, 2, &thin_line);
    draw_line(canvas, whisker_right_bottom, 2, &thin_line);

    draw_rect(canvas, 43, 54, 82, 12, &outline);
    for (int x = 49; x < 120; x += 9) {
        draw_rect(canvas, x, 59, 5, 2, &black);
    }

    int left_paw_y = alternate_paw ? 48 : 52;
    int right_paw_y = alternate_paw ? 52 : 48;
    draw_rect(canvas, 59, left_paw_y, 19, 10, &outline);
    draw_rect(canvas, 88, right_paw_y, 19, 10, &outline);
}

static void draw_status(struct zmk_widget_bongo_status *widget) {
    lv_draw_label_dsc_t small_left;
    lv_draw_label_dsc_t small_right;
    lv_draw_label_dsc_t title;
    char battery_text[8];
    char wpm_text[12];

    init_label(&small_left, &lv_font_unscii_8, LV_TEXT_ALIGN_LEFT);
    init_label(&small_right, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT);
    init_label(&title, &lv_font_montserrat_14, LV_TEXT_ALIGN_CENTER);

    lv_canvas_fill_bg(widget->canvas, BONGO_BACKGROUND, LV_OPA_COVER);
    draw_battery(widget->canvas, widget->battery);
    draw_cat(widget->canvas, widget->alternate_paw);

    snprintf(battery_text, sizeof(battery_text), "%u%%", widget->battery);
    snprintf(wpm_text, sizeof(wpm_text), "%u WPM", widget->wpm);
    draw_text(widget->canvas, 2, 17, 34, &small_left, battery_text);
    draw_text(widget->canvas, 126, 5, 32, &small_right, wpm_text);
    draw_text(widget->canvas, 124, 24, 35, &title, "BONGO");
}

static void set_battery_status(struct zmk_widget_bongo_status *widget,
                               struct bongo_battery_state state) {
    widget->battery = state.level;
    draw_status(widget);
}

static void battery_update_cb(struct bongo_battery_state state) {
    struct zmk_widget_bongo_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_status(widget, state); }
}

static struct bongo_battery_state battery_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *event = as_zmk_battery_state_changed(eh);
    return (struct bongo_battery_state){
        .level = event != NULL ? event->state_of_charge : zmk_battery_state_of_charge(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(bongo_battery_listener, struct bongo_battery_state,
                            battery_update_cb, battery_get_state)
ZMK_SUBSCRIPTION(bongo_battery_listener, zmk_battery_state_changed);

static void set_wpm_status(struct zmk_widget_bongo_status *widget, struct bongo_wpm_state state) {
    widget->wpm = state.wpm;
    draw_status(widget);
}

static void wpm_update_cb(struct bongo_wpm_state state) {
    struct zmk_widget_bongo_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_wpm_status(widget, state); }
}

static struct bongo_wpm_state wpm_get_state(const zmk_event_t *eh) {
    return (struct bongo_wpm_state){.wpm = zmk_wpm_get_state()};
}

ZMK_DISPLAY_WIDGET_LISTENER(bongo_wpm_listener, struct bongo_wpm_state, wpm_update_cb,
                            wpm_get_state)
ZMK_SUBSCRIPTION(bongo_wpm_listener, zmk_wpm_state_changed);

static void key_update_cb(struct bongo_key_state state) {
    if (!state.pressed) {
        return;
    }

    struct zmk_widget_bongo_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        widget->alternate_paw = !widget->alternate_paw;
        draw_status(widget);
    }
}

static struct bongo_key_state key_get_state(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *event = as_zmk_keycode_state_changed(eh);
    return (struct bongo_key_state){.pressed = event != NULL && event->state};
}

ZMK_DISPLAY_WIDGET_LISTENER(bongo_key_listener, struct bongo_key_state, key_update_cb,
                            key_get_state)
ZMK_SUBSCRIPTION(bongo_key_listener, zmk_keycode_state_changed);

int zmk_widget_bongo_status_init(struct zmk_widget_bongo_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, BONGO_WIDTH, BONGO_HEIGHT);

    widget->canvas = lv_canvas_create(widget->obj);
    lv_obj_align(widget->canvas, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_canvas_set_buffer(widget->canvas, widget->cbuf, BONGO_WIDTH, BONGO_HEIGHT,
                         BONGO_COLOR_FORMAT);

    sys_slist_append(&widgets, &widget->node);
    bongo_battery_listener_init();
    bongo_wpm_listener_init();
    bongo_key_listener_init();

    return 0;
}

lv_obj_t *zmk_widget_bongo_status_obj(struct zmk_widget_bongo_status *widget) {
    return widget->obj;
}
