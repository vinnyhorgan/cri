#ifndef CRI_H
#define CRI_H

#include "cri_enums.h"

#define cri_rgb(r, g, b) (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | ((uint32_t)b)
#define cri_rgba(r, g, b, a) (((uint32_t)a) << 24) | (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | ((uint32_t)b)

#ifdef __cplusplus
extern "C" {
#endif

cri_window *cri_open(const char *title, int width, int height, int flags);
int cri_update(cri_window *window, void *buffer, int width, int height);
int cri_update_events(cri_window *window);
void cri_close(cri_window *window);

void cri_set_user_data(cri_window *window, void *user_data);
void *cri_get_user_data(cri_window *window);

bool cri_set_viewport(cri_window *window, int ox, int oy, int width, int height);
void cri_set_clipboard_text(const char *text);
const char *cri_get_clipboard_text();

void *cri_read_file(const char *filename, int *size);
bool cri_write_file(const char *filename, const void *data, int size);
bool cri_file_exists(const char *filename);
bool cri_dir_exists(const char *dirname);
int cri_get_file_size(const char *filename);
long cri_get_file_mod_time(const char *filename);
char **cri_get_dir_files(const char *dirname, int *count);
void cri_free_dir_files(char **files, int count);
bool cri_chdir(const char *dirname);

void cri_set_active_cb(cri_window *window, cri_active_cb cb);
void cri_set_resize_cb(cri_window *window, cri_resize_cb cb);
void cri_set_close_cb(cri_window *window, cri_close_cb cb);
void cri_set_keyboard_cb(cri_window *window, cri_keyboard_cb cb);
void cri_set_char_input_cb(cri_window *window, cri_char_input_cb cb);
void cri_set_mouse_button_cb(cri_window *window, cri_mouse_button_cb cb);
void cri_set_mouse_move_cb(cri_window *window, cri_mouse_move_cb cb);
void cri_set_mouse_scroll_cb(cri_window *window, cri_mouse_scroll_cb cb);
void cri_set_drop_cb(cri_window *window, cri_drop_cb cb);

bool cri_is_window_active(cri_window *window);
int cri_get_window_width(cri_window *window);
int cri_get_window_height(cri_window *window);
int cri_get_mouse_x(cri_window *window);
int cri_get_mouse_y(cri_window *window);
float cri_get_mouse_scroll_x(cri_window *window);
float cri_get_mouse_scroll_y(cri_window *window);
const uint8_t *cri_get_keyboard_state(cri_window *window);
const uint8_t *cri_get_mouse_state(cri_window *window);

void cri_set_target_fps(int fps);
int cri_get_target_fps();
bool cri_wait_sync(cri_window *window);

cri_timer *cri_timer_create();
void cri_timer_destroy(cri_timer *timer);
void cri_timer_reset(cri_timer *timer);
double cri_timer_now(cri_timer *timer);
double cri_timer_dt(cri_timer *timer);

bool cri_open_audio(int sample_rate, int channels, int samples, cri_audio_cb cb, void *user_data);
void cri_close_audio();
int cri_get_audio_sample_rate();
int cri_get_audio_channels();

#ifdef __cplusplus
}
#endif

#endif
