#ifndef CRI_ENUMS_H
#define CRI_ENUMS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    MOUSE_BTN_0,
    MOUSE_BTN_1,
    MOUSE_BTN_2,
    MOUSE_BTN_3,
    MOUSE_BTN_4,
    MOUSE_BTN_5,
    MOUSE_BTN_6,
    MOUSE_BTN_7
} cri_mouse_button;

#define MOUSE_LEFT MOUSE_BTN_1
#define MOUSE_RIGHT MOUSE_BTN_2
#define MOUSE_MIDDLE MOUSE_BTN_3

typedef enum {
    KB_KEY_UNKNOWN       = -1,

    KB_KEY_SPACE         = 32,
    KB_KEY_APOSTROPHE    = 39,
    KB_KEY_COMMA         = 44,
    KB_KEY_MINUS         = 45,
    KB_KEY_PERIOD        = 46,
    KB_KEY_SLASH         = 47,
    KB_KEY_0             = 48,
    KB_KEY_1             = 49,
    KB_KEY_2             = 50,
    KB_KEY_3             = 51,
    KB_KEY_4             = 52,
    KB_KEY_5             = 53,
    KB_KEY_6             = 54,
    KB_KEY_7             = 55,
    KB_KEY_8             = 56,
    KB_KEY_9             = 57,
    KB_KEY_SEMICOLON     = 59,
    KB_KEY_EQUAL         = 61,
    KB_KEY_A             = 65,
    KB_KEY_B             = 66,
    KB_KEY_C             = 67,
    KB_KEY_D             = 68,
    KB_KEY_E             = 69,
    KB_KEY_F             = 70,
    KB_KEY_G             = 71,
    KB_KEY_H             = 72,
    KB_KEY_I             = 73,
    KB_KEY_J             = 74,
    KB_KEY_K             = 75,
    KB_KEY_L             = 76,
    KB_KEY_M             = 77,
    KB_KEY_N             = 78,
    KB_KEY_O             = 79,
    KB_KEY_P             = 80,
    KB_KEY_Q             = 81,
    KB_KEY_R             = 82,
    KB_KEY_S             = 83,
    KB_KEY_T             = 84,
    KB_KEY_U             = 85,
    KB_KEY_V             = 86,
    KB_KEY_W             = 87,
    KB_KEY_X             = 88,
    KB_KEY_Y             = 89,
    KB_KEY_Z             = 90,
    KB_KEY_LEFT_BRACKET  = 91,
    KB_KEY_BACKSLASH     = 92,
    KB_KEY_RIGHT_BRACKET = 93,
    KB_KEY_GRAVE_ACCENT  = 96,
    KB_KEY_WORLD_1       = 161,
    KB_KEY_WORLD_2       = 162,

    KB_KEY_ESCAPE        = 256,
    KB_KEY_ENTER         = 257,
    KB_KEY_TAB           = 258,
    KB_KEY_BACKSPACE     = 259,
    KB_KEY_INSERT        = 260,
    KB_KEY_DELETE        = 261,
    KB_KEY_RIGHT         = 262,
    KB_KEY_LEFT          = 263,
    KB_KEY_DOWN          = 264,
    KB_KEY_UP            = 265,
    KB_KEY_PAGE_UP       = 266,
    KB_KEY_PAGE_DOWN     = 267,
    KB_KEY_HOME          = 268,
    KB_KEY_END           = 269,
    KB_KEY_CAPS_LOCK     = 280,
    KB_KEY_SCROLL_LOCK   = 281,
    KB_KEY_NUM_LOCK      = 282,
    KB_KEY_PRINT_SCREEN  = 283,
    KB_KEY_PAUSE         = 284,
    KB_KEY_F1            = 290,
    KB_KEY_F2            = 291,
    KB_KEY_F3            = 292,
    KB_KEY_F4            = 293,
    KB_KEY_F5            = 294,
    KB_KEY_F6            = 295,
    KB_KEY_F7            = 296,
    KB_KEY_F8            = 297,
    KB_KEY_F9            = 298,
    KB_KEY_F10           = 299,
    KB_KEY_F11           = 300,
    KB_KEY_F12           = 301,
    KB_KEY_F13           = 302,
    KB_KEY_F14           = 303,
    KB_KEY_F15           = 304,
    KB_KEY_F16           = 305,
    KB_KEY_F17           = 306,
    KB_KEY_F18           = 307,
    KB_KEY_F19           = 308,
    KB_KEY_F20           = 309,
    KB_KEY_F21           = 310,
    KB_KEY_F22           = 311,
    KB_KEY_F23           = 312,
    KB_KEY_F24           = 313,
    KB_KEY_F25           = 314,
    KB_KEY_KP_0          = 320,
    KB_KEY_KP_1          = 321,
    KB_KEY_KP_2          = 322,
    KB_KEY_KP_3          = 323,
    KB_KEY_KP_4          = 324,
    KB_KEY_KP_5          = 325,
    KB_KEY_KP_6          = 326,
    KB_KEY_KP_7          = 327,
    KB_KEY_KP_8          = 328,
    KB_KEY_KP_9          = 329,
    KB_KEY_KP_DECIMAL    = 330,
    KB_KEY_KP_DIVIDE     = 331,
    KB_KEY_KP_MULTIPLY   = 332,
    KB_KEY_KP_SUBTRACT   = 333,
    KB_KEY_KP_ADD        = 334,
    KB_KEY_KP_ENTER      = 335,
    KB_KEY_KP_EQUAL      = 336,
    KB_KEY_LEFT_SHIFT    = 340,
    KB_KEY_LEFT_CONTROL  = 341,
    KB_KEY_LEFT_ALT      = 342,
    KB_KEY_LEFT_SUPER    = 343,
    KB_KEY_RIGHT_SHIFT   = 344,
    KB_KEY_RIGHT_CONTROL = 345,
    KB_KEY_RIGHT_ALT     = 346,
    KB_KEY_RIGHT_SUPER   = 347,
    KB_KEY_MENU          = 348
} cri_key;

typedef enum {
    KB_MOD_SHIFT     = 0x0001,
    KB_MOD_CONTROL   = 0x0002,
    KB_MOD_ALT       = 0x0004,
    KB_MOD_SUPER     = 0x0008,
    KB_MOD_CAPS_LOCK = 0x0010,
    KB_MOD_NUM_LOCK  = 0x0020
} cri_mod_key;

typedef enum {
    FLAG_RESIZABLE = 0x01,
    FLAG_HIDECURSOR = 0x02
} cri_flags;

struct cri_window;
typedef struct cri_window cri_window;

struct cri_timer;
typedef struct cri_timer cri_timer;

typedef void(*cri_active_cb)(cri_window *window, bool is_active);
typedef void(*cri_resize_cb)(cri_window *window, int width, int height);
typedef void(*cri_keyboard_cb)(cri_window *window, cri_key key, cri_mod_key mod, bool is_pressed);
typedef void(*cri_char_input_cb)(cri_window *window, unsigned int code);
typedef void(*cri_mouse_button_cb)(cri_window *window, cri_mouse_button button, cri_mod_key mod, bool is_pressed);
typedef void(*cri_mouse_move_cb)(cri_window *window, int x, int y);
typedef void(*cri_mouse_scroll_cb)(cri_window *window, cri_mod_key mod, float dx, float dy);
typedef void(*cri_drop_cb)(cri_window *window, int count, const char **paths);

typedef void(*cri_audio_cb)(float *buffer, int frames, int channels, void *user_data);

#endif
