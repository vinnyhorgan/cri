#include "cri.h"

#include <stdlib.h>

static int g_width = 640;
static int g_height = 480;
static uint32_t *g_buffer;

static void on_resize(cri_window *window, int width, int height) {
    g_width = width;
    g_height = height;
    g_buffer = (uint32_t*)realloc(g_buffer, g_width * g_height * 4);
}

int main() {
    int noise, carry, seed = 0xbeef;

    cri_window *window = cri_open("Noise", g_width, g_height, FLAG_RESIZABLE);
    if (!window)
        return 1;

    g_buffer = (uint32_t*)malloc(g_width * g_height * 4);
    cri_set_resize_cb(window, on_resize);

    do {
        for (int i = 0; i < g_width * g_height; ++i) {
            noise = seed;
            noise >>= 3;
            noise ^= seed;
            carry = noise & 1;
            noise >>= 1;
            seed >>= 1;
            seed |= (carry << 30);
            noise &= 0xFF;
            g_buffer[i] = cri_rgb(noise, noise, noise);
        }

        int state = cri_update(window, g_buffer, g_width, g_height);
        if (state != 0) { break; }
    } while (cri_wait_sync(window));

    return 0;
}
