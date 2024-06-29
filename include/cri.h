#ifndef CRI_H
#define CRI_H

#include "cri_enums.h"

#define CRI_RGB(r, g, b) (((unsigned int)r) << 16) | (((unsigned int)g) << 8) | ((unsigned int)b)

#ifdef __cplusplus
extern "C" {
#endif

cri_window *cri_open(const char *title, int width, int height, int flags);
int cri_update(cri_window *window, void *buffer);
void cri_close(cri_window *window);

void cri_set_user_data(cri_window *window, void *user_data);
void *cri_get_user_data(cri_window *window);

bool cri_set_viewport(cri_window *window, int ox, int oy, int width, int height);

void cri_set_active_cb(cri_window *window, cri_active_cb cb);
void cri_set_resize_cb(cri_window *window, cri_resize_cb cb);
void cri_set_keyboard_cb(cri_window *window, cri_keyboard_cb cb);
void cri_set_char_input_cb(cri_window *window, cri_char_input_cb cb);
void cri_set_mouse_button_cb(cri_window *window, cri_mouse_button_cb cb);
void cri_set_mouse_move_cb(cri_window *window, cri_mouse_move_cb cb);
void cri_set_mouse_scroll_cb(cri_window *window, cri_mouse_scroll_cb cb);

void cri_set_target_fps(int fps);
bool cri_wait_sync(cri_window *window);

cri_timer *cri_timer_create();
void cri_timer_destroy(cri_timer *timer);
void cri_timer_reset(cri_timer *timer);
double cri_timer_now(cri_timer *timer);
double cri_timer_dt(cri_timer *timer);

bool cri_open_audio(int sample_rate, int channels, cri_audio_cb cb, void *user_data);
void cri_close_audio();
int cri_get_audio_sample_rate();
int cri_get_audio_channels();

#ifdef __cplusplus
}
#endif

#endif
