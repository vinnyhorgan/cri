#include "cri.h"
#include "cri_internal.h"

#define SOKOL_AUDIO_IMPL
#include "sokol_audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

short int g_keycodes[512];
double g_timer_res;
double g_time_per_frame = 1.0 / 60.0;

extern void s_cri_timer_init();
extern uint64_t s_cri_timer_tick();

void cri_close(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->close = true;
    }
}

void cri_set_user_data(cri_window *window, void *user_data) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        window_data->user_data = user_data;
    }
}

void *cri_get_user_data(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->user_data;
    }
    return NULL;
}

void *cri_read_file(const char *filename, int *size) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) { return NULL; }
    fseek(fp, 0, SEEK_END);
    int n = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buf = (char*)calloc(n + 1, 1);
    if (!buf) { return NULL; }
    fread(buf, 1, n, fp);
    fclose(fp);
    if (size) { *size = n; }
    return buf;
}

void cri_write_file(const char *filename, const void *data, int size) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) { return; }
    fwrite(data, 1, size, fp);
    fclose(fp);
}

bool cri_file_exists(const char *filename) {
    struct stat st;
    return stat(filename, &st) == 0;
}

bool cri_dir_exists(const char *dirname) {
    DIR *dir = opendir(dirname);
    if (!dir) { return false; }
    closedir(dir);
    return true;
}

int cri_get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) { return st.st_size; }
    return -1;
}

long cri_get_file_mod_time(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) { return st.st_mtime; }
    return -1;
}

char **cri_get_dir_files(const char *dirname, int *count) {
    DIR *dir = opendir(dirname);
    if (!dir) { return NULL; }

    struct dirent *entry;
    int n = 0;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            n++;
        }
    }
    closedir(dir);

    char **files = (char**)malloc(n * sizeof(char*));
    if (!files) { return NULL; }

    dir = opendir(dirname);
    if (!dir) {
        free(files);
        return NULL;
    }

    int i = 0;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            files[i] = strdup(entry->d_name);
            if (!files[i]) {
                free(files);
                closedir(dir);
                return NULL;
            }
            i++;
        }
    }
    closedir(dir);

    *count = n;

    return files;
}

void cri_free_dir_files(char **files, int count) {
    for (int i = 0; i < count; i++) {
        free(files[i]);
    }
    free(files);
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

bool cri_is_window_active(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->is_active;
    }
    return false;
}

int cri_get_window_width(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->window_width;
    }
    return 0;
}

int cri_get_window_height(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->window_height;
    }
    return 0;
}

int cri_get_mouse_x(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->mouse_x;
    }
    return 0;
}

int cri_get_mouse_y(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->mouse_y;
    }
    return 0;
}

float cri_get_mouse_scroll_x(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->mouse_scroll_x;
    }
    return 0;
}

float cri_get_mouse_scroll_y(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->mouse_scroll_y;
    }
    return 0;
}

const uint8_t *cri_get_keyboard_state(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->keyboard_state;
    }
    return NULL;
}

const uint8_t *cri_get_mouse_state(cri_window *window) {
    if (window) {
        s_cri_window_data *window_data = (s_cri_window_data*)window;
        return window_data->mouse_state;
    }
    return NULL;
}

void cri_set_target_fps(int fps) {
    if (fps == 0) {
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

    cri_timer *timer = (cri_timer*)malloc(sizeof(cri_timer));
    cri_timer_reset(timer);

    return timer;
}

void cri_timer_destroy(cri_timer *timer) {
    if (timer) {
        free(timer);
    }
}

void cri_timer_reset(cri_timer *timer) {
    if (!timer)
        return;

    timer->start_time = s_cri_timer_tick();
    timer->dt_counter = timer->start_time;
    timer->time = 0;
}

double cri_timer_now(cri_timer *timer) {
    if (!timer)
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
