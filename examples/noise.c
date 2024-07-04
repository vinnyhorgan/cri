#include "cri.h"

#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480

static void on_active(cri_window *window, bool is_active) {
    printf("Active: %s\n", is_active ? "true" : "false");
}

static void on_resize(cri_window *window, int width, int height) {
    printf("Resized: %dx%d\n", width, height);
}

static void on_keyboard(cri_window *window, cri_key key, cri_mod_key mod, bool is_pressed) {
    printf("Key: %d, Mod: %d, Pressed: %s\n", key, mod, is_pressed ? "true" : "false");
}

static void on_char_input(cri_window *window, unsigned int code) {
    printf("Char: %d\n", code);
}

static void on_mouse_button(cri_window *window, cri_mouse_button button, cri_mod_key mod, bool is_pressed) {
    printf("Button: %d, Mod: %d, Pressed: %s\n", button, mod, is_pressed ? "true" : "false");
}

static void on_mouse_move(cri_window *window, int x, int y) {
    printf("Mouse: %d, %d\n", x, y);
}

static void on_mouse_scroll(cri_window *window, cri_mod_key mod, float dx, float dy) {
    printf("Scroll: %f, %f\n", dx, dy);
}

static void on_audio(float *buffer, int frames, int channels, void *user_data) {
    static unsigned int count = 0;
    for (int i = 0; i < frames; i++) {
        buffer[i] = (count++ & (1<<3)) ? 0.5f : -0.5f;
    }
}

int main() {
    unsigned int buffer[WIDTH * HEIGHT];
    int noise, carry, seed = 0xbeef;

    cri_window *window = cri_open("Noise", WIDTH, HEIGHT, FLAG_RESIZABLE);
    if (!window)
        return 1;

    cri_open_audio(44100, 1, on_audio, NULL);

    cri_set_active_cb(window, on_active);
    cri_set_resize_cb(window, on_resize);
    cri_set_keyboard_cb(window, on_keyboard);
    cri_set_char_input_cb(window, on_char_input);
    cri_set_mouse_button_cb(window, on_mouse_button);
    cri_set_mouse_move_cb(window, on_mouse_move);
    cri_set_mouse_scroll_cb(window, on_mouse_scroll);

    do {
        int i;

        for (i = 0; i < WIDTH * HEIGHT; ++i) {
            noise = seed;
            noise >>= 3;
            noise ^= seed;
            carry = noise & 1;
            noise >>= 1;
            seed >>= 1;
            seed |= (carry << 30);
            noise &= 0xFF;
            buffer[i] = cri_rgb(noise, noise, noise);
        }

        int state = cri_update(window, buffer);
        if (state != 0) { break; }
    } while (cri_wait_sync(window));

    cri_close_audio();

    return 0;
}
