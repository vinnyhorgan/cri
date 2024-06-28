#include "cri.h"
#include "cri_internal.h"

#include <stdlib.h>

short int keycodes[512];

void cri_close(struct cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->close = true;
    }
}

void cri_set_user_data(struct cri_window *window, void *user_data) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->user_data = user_data;
    }
}

void *cri_get_user_data(struct cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->user_data;
    }
    return NULL;
}

void cri_set_active_cb(cri_window *window, cri_active_cb cb) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->active_cb = cb;
    }
}

void cri_set_resize_cb(cri_window *window, cri_resize_cb cb) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->resize_cb = cb;
    }
}

void cri_set_keyboard_cb(cri_window *window, cri_keyboard_cb cb) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->keyboard_cb = cb;
    }
}

void cri_set_char_input_cb(cri_window *window, cri_char_input_cb cb) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->char_input_cb = cb;
    }
}

void cri_set_mouse_button_cb(cri_window *window, cri_mouse_button_cb cb) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->mouse_button_cb = cb;
    }
}

void cri_set_mouse_move_cb(cri_window *window, cri_mouse_move_cb cb) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->mouse_move_cb = cb;
    }
}

void cri_set_mouse_scroll_cb(cri_window *window, cri_mouse_scroll_cb cb) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->mouse_scroll_cb = cb;
    }
}
