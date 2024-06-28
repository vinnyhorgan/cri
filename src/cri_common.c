#include "cri.h"
#include "cri_internal.h"

#define SOKOL_AUDIO_IMPL
#include "sokol_audio.h"

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

bool cri_open_audio(int sample_rate, int channels, cri_audio_cb cb, void *user_data) {
    saudio_setup(&(saudio_desc){
        .sample_rate = sample_rate,
        .num_channels = channels,
        .stream_userdata_cb = cb,
        .user_data = user_data
    });

    return saudio_isvalid();
}

void cri_close_audio() {
    saudio_shutdown();
}

int cri_get_audio_sample_rate() {
    return saudio_sample_rate();
}

int cri_get_audio_channels() {
    return saudio_channels();
}
