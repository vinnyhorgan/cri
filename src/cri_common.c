#include "cri.h"
#include "cri_internal.h"

#define SOKOL_AUDIO_IMPL
#include "sokol_audio.h"

#include <stdio.h>
#include <stdlib.h>

short int keycodes[512];

double g_timer_res;
double g_time_per_frame = 1.0 / 60.0;

extern uint64_t s_cri_timer_tick();
extern void s_cri_timer_init();

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

void cri_set_target_fps(int fps) {
    if(fps == 0) {
        g_time_per_frame = 0;
    } else {
        g_time_per_frame = 1.0 / fps;
    }
}

cri_timer *cri_timer_create() {
    static bool first = true;

    if (first) {
        s_cri_timer_init();
        first = false;
    }

    cri_timer *timer = malloc(sizeof(cri_timer));
    cri_timer_reset(timer);

    return timer;
}

void cri_timer_destroy(cri_timer *timer) {
    if(timer) {
        free(timer);
    }
}

void cri_timer_reset(cri_timer *timer) {
    if(!timer)
        return;

    timer->start_time = s_cri_timer_tick();
    timer->dt_counter = timer->start_time;
    timer->time = 0;
}

double cri_timer_now(cri_timer *timer) {
    if(!timer)
        return 0.0;

    uint64_t counter = s_cri_timer_tick();
    timer->time += (counter - timer->start_time);
    timer->start_time = counter;

    return timer->time * g_timer_res;
}

double cri_timer_dt(cri_timer *timer) {
    if (!timer)
        return 0.0;

    int64_t counter = s_cri_timer_tick();
    uint64_t dt = (counter - timer->dt_counter);
    timer->dt_counter = counter;

    return dt * g_timer_res;
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

void *cri_read_file(const char *filename, int *size) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) { return NULL; }
    fseek(fp, 0, SEEK_END);
    int n = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buf = calloc(n + 1, 1);
    if (!buf) { return NULL; }
    fread(buf, 1, n, fp);
    fclose(fp);
    if (size) { *size = n; }
    return buf;
}
