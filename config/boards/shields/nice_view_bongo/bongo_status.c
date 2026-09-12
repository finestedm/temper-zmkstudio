#include "bongo_status.h"

#include <stdio.h>
#include <string.h>

#include <zephyr/logging/log.h>

#include <zmk/battery.h>
#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/wpm.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define BONGO_BACKGROUND lv_color_white()
#define BONGO_FOREGROUND lv_color_black()

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct bongo_key_state {
    bool pressed;
};

struct bongo_battery_state {
    uint8_t level;
};

struct bongo_connection_state {
    enum bongo_connection_status status;
    uint8_t profile_index;
};

struct bongo_layer_state {
    uint8_t index;
    const char *label;
};

struct bongo_wpm_state {
    uint8_t wpm;
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

static void init_label(lv_draw_label_dsc_t *dsc, lv_text_align_t align) {
    lv_draw_label_dsc_init(dsc);
    dsc->color = BONGO_FOREGROUND;
    dsc->font = &lv_font_unscii_8;
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
    lv_area_t coords = {x, y, x + width - 1, BONGO_LOGICAL_HEIGHT - 1};
    lv_draw_label(&layer, dsc, &coords);
    lv_canvas_finish_layer(canvas, &layer);
}

static void draw_battery(lv_obj_t *canvas, uint8_t level) {
    lv_draw_rect_dsc_t black;
    lv_draw_rect_dsc_t white;

    init_rect(&black, BONGO_FOREGROUND, 0, 0);
    init_rect(&white, BONGO_BACKGROUND, 0, 0);

    draw_rect(canvas, 2, 3, 18, 9, &black);
    draw_rect(canvas, 4, 5, 14, 5, &white);
    draw_rect(canvas, 20, 5, 2, 5, &black);

    int fill_width = (level * 12 + 99) / 100;
    if (fill_width > 0) {
        draw_rect(canvas, 5, 6, fill_width, 3, &black);
    }
}

static void draw_indicators(lv_obj_t *canvas, const struct zmk_widget_bongo_status *widget) {
    lv_draw_label_dsc_t left;
    lv_draw_label_dsc_t right;
    char battery_text[6];
    char connection_text[6];
    char layer_text[8];
    char wpm_text[8];

    init_label(&left, LV_TEXT_ALIGN_LEFT);
    init_label(&right, LV_TEXT_ALIGN_RIGHT);

    snprintf(battery_text, sizeof(battery_text), "%u%%", widget->battery);
    switch (widget->connection) {
    case BONGO_CONNECTION_USB:
        strcpy(connection_text, "USB");
        break;
    case BONGO_CONNECTION_BLE_CONNECTED:
        snprintf(connection_text, sizeof(connection_text), "B%u+", widget->profile_index);
        break;
    case BONGO_CONNECTION_BLE_DISCONNECTED:
        snprintf(connection_text, sizeof(connection_text), "B%u-", widget->profile_index);
        break;
    default:
        snprintf(connection_text, sizeof(connection_text), "B%u?", widget->profile_index);
        break;
    }

    if (widget->layer_label != NULL && strlen(widget->layer_label) > 0) {
        snprintf(layer_text, sizeof(layer_text), "L %.4s", widget->layer_label);
    } else {
        snprintf(layer_text, sizeof(layer_text), "L%u", widget->layer_index);
    }
    snprintf(wpm_text, sizeof(wpm_text), "%u WPM", widget->wpm);

    draw_battery(canvas, widget->battery);
    draw_text(canvas, 25, 1, 22, &left, battery_text);
    draw_text(canvas, 48, 1, 18, &right, connection_text);
    draw_text(canvas, 2, 15, 30, &left, layer_text);
    draw_text(canvas, 33, 15, 33, &right, wpm_text);
}

static void draw_paw(lv_obj_t *canvas, int x, bool tapping) {
    lv_draw_rect_dsc_t outline;
    lv_draw_rect_dsc_t black;
    lv_draw_line_dsc_t motion_line;

    init_rect(&outline, BONGO_BACKGROUND, 2, 9);
    init_rect(&black, BONGO_FOREGROUND, 0, 3);
    init_line(&motion_line, 2);

    if (tapping) {
        /* The long paw is the one currently hitting the desk. */
        draw_rect(canvas, x, 98, 20, 47, &outline);

        const lv_point_t left_tap[] = {{x + 3, 150}, {x, 155}};
        const lv_point_t middle_tap[] = {{x + 10, 151}, {x + 10, 157}};
        const lv_point_t right_tap[] = {{x + 17, 150}, {x + 20, 155}};
        draw_line(canvas, left_tap, 2, &motion_line);
        draw_line(canvas, middle_tap, 2, &motion_line);
        draw_line(canvas, right_tap, 2, &motion_line);
    } else {
        /* The raised paw shows the simple monochrome paw pads from Bongo Cat. */
        draw_rect(canvas, x, 99, 20, 32, &outline);
        draw_rect(canvas, x + 7, 114, 6, 8, &black);
        draw_rect(canvas, x + 3, 108, 4, 4, &black);
        draw_rect(canvas, x + 8, 106, 4, 4, &black);
        draw_rect(canvas, x + 13, 108, 4, 4, &black);
    }
}

static void draw_bongo_cat(lv_obj_t *canvas, bool alternate_paw) {
    lv_draw_rect_dsc_t black;
    lv_draw_line_dsc_t outline;
    lv_draw_line_dsc_t face;

    init_rect(&black, BONGO_FOREGROUND, 0, 2);
    init_line(&outline, 3);
    init_line(&face, 2);

    lv_canvas_fill_bg(canvas, BONGO_BACKGROUND, LV_OPA_COVER);

    /* The characteristic Bongo Cat blob: peaked ears, arched back, and tail. */
    const lv_point_t body[] = {
        {4, 103}, {2, 94},  {3, 77},  {8, 62},  {16, 52}, {21, 33},
        {29, 45}, {40, 47}, {51, 55}, {63, 49}, {64, 70}, {61, 82},
        {66, 94}, {62, 103}, {55, 109}, {13, 109}, {4, 103},
    };
    draw_line(canvas, body, ARRAY_SIZE(body), &outline);

    /* Dot eyes and the small W-shaped mouth from the original animation. */
    draw_rect(canvas, 21, 70, 4, 6, &black);
    draw_rect(canvas, 45, 70, 4, 6, &black);
    const lv_point_t mouth[] = {{28, 80}, {31, 84}, {34, 80}, {37, 84}, {41, 80}};
    draw_line(canvas, mouth, ARRAY_SIZE(mouth), &face);

    draw_paw(canvas, 8, !alternate_paw);
    draw_paw(canvas, 40, alternate_paw);
}

static void rotate_clockwise(struct zmk_widget_bongo_status *widget) {
    const uint32_t source_stride =
        lv_draw_buf_width_to_stride(BONGO_LOGICAL_WIDTH, BONGO_COLOR_FORMAT);
    const uint32_t display_stride =
        lv_draw_buf_width_to_stride(BONGO_DISPLAY_WIDTH, BONGO_COLOR_FORMAT);

    lv_draw_sw_rotate(widget->drawing_buf, widget->display_buf, BONGO_LOGICAL_WIDTH,
                      BONGO_LOGICAL_HEIGHT, source_stride, display_stride,
                      LV_DISPLAY_ROTATION_90, BONGO_COLOR_FORMAT);
    lv_obj_invalidate(widget->display_canvas);
}

static void draw_frame(struct zmk_widget_bongo_status *widget) {
    draw_bongo_cat(widget->drawing_canvas, widget->alternate_paw);
    draw_indicators(widget->drawing_canvas, widget);
    rotate_clockwise(widget);
}

static void battery_update_cb(struct bongo_battery_state state) {
    struct zmk_widget_bongo_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        widget->battery = state.level;
        draw_frame(widget);
    }
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

static void connection_update_cb(struct bongo_connection_state state) {
    struct zmk_widget_bongo_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        widget->connection = state.status;
        widget->profile_index = state.profile_index;
        draw_frame(widget);
    }
}

static struct bongo_connection_state connection_get_state(const zmk_event_t *eh) {
    const struct zmk_endpoint_instance selected = zmk_endpoint_get_selected();
    struct bongo_connection_state state = {
        .status = BONGO_CONNECTION_USB,
        .profile_index = zmk_ble_active_profile_index() + 1,
    };

    if (selected.transport == ZMK_TRANSPORT_BLE) {
        if (zmk_ble_active_profile_is_connected()) {
            state.status = BONGO_CONNECTION_BLE_CONNECTED;
        } else if (zmk_ble_active_profile_is_open()) {
            state.status = BONGO_CONNECTION_BLE_OPEN;
        } else {
            state.status = BONGO_CONNECTION_BLE_DISCONNECTED;
        }
    }

    return state;
}

ZMK_DISPLAY_WIDGET_LISTENER(bongo_connection_listener, struct bongo_connection_state,
                            connection_update_cb, connection_get_state)
ZMK_SUBSCRIPTION(bongo_connection_listener, zmk_endpoint_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(bongo_connection_listener, zmk_usb_conn_state_changed);
#endif
#if IS_ENABLED(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(bongo_connection_listener, zmk_ble_active_profile_changed);
#endif

static void layer_update_cb(struct bongo_layer_state state) {
    struct zmk_widget_bongo_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        widget->layer_index = state.index;
        widget->layer_label = state.label;
        draw_frame(widget);
    }
}

static struct bongo_layer_state layer_get_state(const zmk_event_t *eh) {
    const zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
    return (struct bongo_layer_state){
        .index = index,
        .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index)),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(bongo_layer_listener, struct bongo_layer_state, layer_update_cb,
                            layer_get_state)
ZMK_SUBSCRIPTION(bongo_layer_listener, zmk_layer_state_changed);

static void wpm_update_cb(struct bongo_wpm_state state) {
    struct zmk_widget_bongo_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        widget->wpm = state.wpm;
        draw_frame(widget);
    }
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
        draw_frame(widget);
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
    lv_obj_set_size(widget->obj, BONGO_DISPLAY_WIDTH, BONGO_DISPLAY_HEIGHT);

    widget->drawing_canvas = lv_canvas_create(widget->obj);
    lv_canvas_set_buffer(widget->drawing_canvas, widget->drawing_buf, BONGO_LOGICAL_WIDTH,
                         BONGO_LOGICAL_HEIGHT, BONGO_COLOR_FORMAT);
    lv_obj_add_flag(widget->drawing_canvas, LV_OBJ_FLAG_HIDDEN);

    widget->display_canvas = lv_canvas_create(widget->obj);
    lv_obj_align(widget->display_canvas, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_canvas_set_buffer(widget->display_canvas, widget->display_buf, BONGO_DISPLAY_WIDTH,
                         BONGO_DISPLAY_HEIGHT, BONGO_COLOR_FORMAT);

    sys_slist_append(&widgets, &widget->node);
    draw_frame(widget);
    bongo_battery_listener_init();
    bongo_connection_listener_init();
    bongo_layer_listener_init();
    bongo_wpm_listener_init();
    bongo_key_listener_init();

    return 0;
}

lv_obj_t *zmk_widget_bongo_status_obj(struct zmk_widget_bongo_status *widget) {
    return widget->obj;
}
