#ifndef CRI_INTERNAL_H
#define CRI_INTERNAL_H

#include "cri.h"

#define cri_call(cb, ...) if (window_data && window_data->cb) window_data->cb((cri_window*)window_data, __VA_ARGS__);

typedef struct {
    void *specific;
    void *user_data;

    cri_active_cb active_cb;
    cri_resize_cb resize_cb;
    cri_keyboard_cb keyboard_cb;
    cri_char_input_cb char_input_cb;
    cri_mouse_button_cb mouse_button_cb;
    cri_mouse_move_cb mouse_move_cb;
    cri_mouse_scroll_cb mouse_scroll_cb;

    int window_width;
    int window_height;

    int dst_ox;
    int dst_oy;
    int dst_width;
    int dst_height;

    void *buffer;
    int buffer_width;
    int buffer_height;

    bool is_active;
    int mouse_x;
    int mouse_y;
    float mouse_scroll_x;
    float mouse_scroll_y;
    uint8_t keyboard_state[512];
    uint8_t mouse_state[8];

    int mod_keys;
    bool close;
} s_cri_window_data;

struct cri_timer {
    int64_t start_time;
    int64_t dt_counter;
    uint64_t time;
};

extern short int g_keycodes[512];

#endif
