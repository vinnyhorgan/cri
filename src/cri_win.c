#include "cri.h"
#include "cri_internal.h"

#include <windows.h>
#include <dwmapi.h>

#include <stdlib.h>

#define DARK_MODE 20

typedef struct {
    WNDCLASS wc;
    HWND hwnd;
    HDC hdc;
    BITMAPINFO *bmi;
    bool mouse_inside;
    cri_timer *timer;
} s_cri_window_data_win;

extern double g_timer_res;
extern double g_time_per_frame;
extern short int keycodes[512];

static int s_cri_translate_mod();
static cri_key s_cri_translate_key(unsigned int wparam, unsigned long lparam);
static void s_cri_destroy_window_data(s_cri_window_data *window_data);

static LRESULT CALLBACK wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    int res = 0;

    s_cri_window_data *window_data = (s_cri_window_data*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    s_cri_window_data_win *window_data_win = NULL;
    if (window_data) {
        window_data_win = (s_cri_window_data_win*)window_data->specific;
    }

    switch (message) {
    case WM_PAINT:
        if (window_data && window_data->buffer && window_data_win) {
            StretchDIBits(window_data_win->hdc,
                          window_data->dst_ox, window_data->dst_oy, window_data->dst_width, window_data->dst_height,
                          0, 0, window_data->buffer_width, window_data->buffer_height,
                          window_data->buffer, window_data_win->bmi, DIB_RGB_COLORS, SRCCOPY);

            ValidateRect(hwnd, NULL);
        }
        break;

    case WM_DESTROY:
    case WM_CLOSE:
        if (window_data) {
            window_data->close = true;
        }
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if(window_data) {
            cri_key key = s_cri_translate_key((unsigned int)wparam, (unsigned long)lparam);
            int is_pressed = !((lparam >> 31) & 1);
            window_data->mod_keys = s_cri_translate_mod();

            if (key == KB_KEY_UNKNOWN)
                return FALSE;

            cri_call(keyboard_cb, key, window_data->mod_keys, is_pressed);
        }
        break;

    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_UNICHAR:
        if(window_data) {
            if(message == WM_UNICHAR && wparam == UNICODE_NOCHAR)
                return TRUE;

            cri_call(char_input_cb, wparam);
        }
        break;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONDBLCLK:
        if(window_data) {
            cri_mouse_button button = MOUSE_BTN_0;
            window_data->mod_keys = s_cri_translate_mod();
            int is_pressed = 0;

            switch(message) {
            case WM_LBUTTONDOWN:
                is_pressed = 1;
            case WM_LBUTTONUP:
                button = MOUSE_BTN_1;
                break;

            case WM_RBUTTONDOWN:
                is_pressed = 1;
            case WM_RBUTTONUP:
                button = MOUSE_BTN_2;
                break;

            case WM_MBUTTONDOWN:
                is_pressed = 1;
            case WM_MBUTTONUP:
                button = MOUSE_BTN_3;
                break;

            default:
                button = (GET_XBUTTON_WPARAM(wparam) == XBUTTON1 ? MOUSE_BTN_5 : MOUSE_BTN_6);
                if(message == WM_XBUTTONDOWN)
                    is_pressed = 1;
            }

            cri_call(mouse_button_cb, button, window_data->mod_keys, is_pressed);
        }
        break;

    case WM_MOUSEWHEEL:
        if(window_data) {
            cri_call(mouse_scroll_cb, s_cri_translate_mod(), 0.0f, (SHORT)HIWORD(wparam) / (float)WHEEL_DELTA);
        }
        break;

    case WM_MOUSEHWHEEL:
        if(window_data) {
            cri_call(mouse_scroll_cb, s_cri_translate_mod(), -((SHORT)HIWORD(wparam) / (float)WHEEL_DELTA), 0.0f);
        }
        break;

    case WM_MOUSEMOVE:
        if(window_data) {
            if(window_data_win->mouse_inside == false) {
                window_data_win->mouse_inside = true;
                TRACKMOUSEEVENT tme;
                ZeroMemory(&tme, sizeof(tme));
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
            }

            cri_call(mouse_move_cb, ((int)(short)LOWORD(lparam)), ((int)(short)HIWORD(lparam)));
        }
        break;

    case WM_MOUSELEAVE:
        if(window_data) {
            window_data_win->mouse_inside = false;
        }
        break;

    case WM_SIZE:
        if(window_data) {
            window_data->dst_ox = 0;
            window_data->dst_oy = 0;
            window_data->dst_width = LOWORD(lparam);
            window_data->dst_height = HIWORD(lparam);
            window_data->window_width = window_data->dst_width;
            window_data->window_height = window_data->dst_height;
            BitBlt(window_data_win->hdc, 0, 0, window_data->window_width, window_data->window_height, 0, 0, 0, BLACKNESS);
            cri_call(resize_cb, window_data->window_width, window_data->window_height);
        }
        break;

    case WM_SETFOCUS:
        if (window_data) {
            cri_call(active_cb, true);
        }
        break;

    case WM_KILLFOCUS:
        if (window_data) {
            cri_call(active_cb, false);
        }
        break;

    default:
        res = DefWindowProc(hwnd, message, wparam, lparam);
    }

    return res;
}

cri_window *cri_open(const char *title, int width, int height, int flags) {
    init_keycodes();

    s_cri_window_data *window_data = (s_cri_window_data*)calloc(1, sizeof(s_cri_window_data));
    s_cri_window_data_win *window_data_win = (s_cri_window_data_win*)calloc(1, sizeof(s_cri_window_data_win));
    window_data->specific = window_data_win;

    window_data->buffer_width = width;
    window_data->buffer_height = height;

    int window_style = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;

    if (flags & FLAG_RESIZABLE)
        window_style |= WS_MAXIMIZEBOX | WS_SIZEBOX;

    RECT rect = {0};
    int x, y;

    rect.right = width;
    rect.bottom = height;
    AdjustWindowRect(&rect, window_style, 0);
    rect.right -= rect.left;
    rect.bottom -= rect.top;

    x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
    y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom + rect.top) / 2;

    window_data_win->wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    window_data_win->wc.lpfnWndProc = wndproc;
    window_data_win->wc.hCursor = LoadCursor(0, IDC_ARROW);
    window_data_win->wc.lpszClassName = title;
    RegisterClass(&window_data_win->wc);

    if (window_data->dst_width == 0)
        window_data->dst_width = width;

    if (window_data->dst_height == 0)
        window_data->dst_height = height;

    window_data->window_width  = rect.right;
    window_data->window_height = rect.bottom;

    window_data_win->hwnd = CreateWindowEx(0, title, title, window_style, x, y, window_data->window_width, window_data->window_height, 0, 0, 0, 0);
    if (!window_data_win->hwnd)
        return NULL;

    BOOL dark = TRUE;
    DwmSetWindowAttribute(window_data_win->hwnd, DARK_MODE, &dark, sizeof(dark));

    SetWindowLongPtr(window_data_win->hwnd, GWLP_USERDATA, (LONG_PTR)window_data);

    ShowWindow(window_data_win->hwnd, SW_NORMAL);

    window_data_win->bmi = (BITMAPINFO*)calloc(1, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 3);
    window_data_win->bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    window_data_win->bmi->bmiHeader.biPlanes = 1;
    window_data_win->bmi->bmiHeader.biBitCount = 32;
    window_data_win->bmi->bmiHeader.biCompression = BI_BITFIELDS;
    window_data_win->bmi->bmiHeader.biWidth = window_data->buffer_width;
    window_data_win->bmi->bmiHeader.biHeight = -(LONG)window_data->buffer_height;
    window_data_win->bmi->bmiColors[0].rgbRed = 0xff;
    window_data_win->bmi->bmiColors[1].rgbGreen = 0xff;
    window_data_win->bmi->bmiColors[2].rgbBlue = 0xff;

    window_data_win->hdc = GetDC(window_data_win->hwnd);
    window_data_win->timer = cri_timer_create();

    return (cri_window*)window_data;
}

int cri_update(cri_window *window, void *buffer) {
    if (!window)
        return 1;

    s_cri_window_data *window_data = (s_cri_window_data*)window;
    s_cri_window_data_win *window_data_win = (s_cri_window_data_win*)window_data->specific;

    if (window_data->close) {
        s_cri_destroy_window_data(window_data);
        return 1;
    }

    if (!buffer)
        return 1;

    window_data->buffer = buffer;

    InvalidateRect(window_data_win->hwnd, NULL, TRUE);
    SendMessage(window_data_win->hwnd, WM_PAINT, 0, 0);

    MSG msg;
    while (!window_data->close && PeekMessage(&msg, window_data_win->hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

bool cri_set_viewport(cri_window *window, int ox, int oy, int width, int height) {
    s_cri_window_data *window_data = (s_cri_window_data*)window;

    if (ox + width > window_data->window_width)
        return false;

    if (oy + height > window_data->window_height)
        return false;

    window_data->dst_ox = ox;
    window_data->dst_oy = oy;
    window_data->dst_width = width;
    window_data->dst_height = height;

    return true;
}

bool cri_wait_sync(cri_window *window) {
    if (!window)
        return false;

    s_cri_window_data *window_data = (s_cri_window_data*)window;
    s_cri_window_data_win *window_data_win = (s_cri_window_data_win*)window_data->specific;

    if (window_data->close) {
        s_cri_destroy_window_data(window_data);
        return false;
    }

    double current;
    unsigned int milliseconds = 1;
    MSG msg;
    while (true) {
        if (PeekMessage(&msg, window_data_win->hwnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (window_data->close) {
            s_cri_destroy_window_data(window_data);
            return false;
        }

        current = cri_timer_now(window_data_win->timer);;
        if (current >= g_time_per_frame) {
            cri_timer_reset(window_data_win->timer);
            return true;
        } else if (current >= g_time_per_frame * 0.8) {
            milliseconds = 0;
        }

        Sleep(milliseconds);
    }

    return true;
}

static int s_cri_translate_mod() {
    int mods = 0;

    if (GetKeyState(VK_SHIFT) & 0x8000)
        mods |= KB_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000)
        mods |= KB_MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000)
        mods |= KB_MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
        mods |= KB_MOD_SUPER;
    if (GetKeyState(VK_CAPITAL) & 1)
        mods |= KB_MOD_CAPS_LOCK;
    if (GetKeyState(VK_NUMLOCK) & 1)
        mods |= KB_MOD_NUM_LOCK;

    return mods;
}

void init_keycodes() {
    for (int i = 0; i < sizeof(keycodes) / sizeof(keycodes[0]); i++)
        keycodes[i] = 0;

    keycodes[0x00B] = KB_KEY_0;
    keycodes[0x002] = KB_KEY_1;
    keycodes[0x003] = KB_KEY_2;
    keycodes[0x004] = KB_KEY_3;
    keycodes[0x005] = KB_KEY_4;
    keycodes[0x006] = KB_KEY_5;
    keycodes[0x007] = KB_KEY_6;
    keycodes[0x008] = KB_KEY_7;
    keycodes[0x009] = KB_KEY_8;
    keycodes[0x00A] = KB_KEY_9;
    keycodes[0x01E] = KB_KEY_A;
    keycodes[0x030] = KB_KEY_B;
    keycodes[0x02E] = KB_KEY_C;
    keycodes[0x020] = KB_KEY_D;
    keycodes[0x012] = KB_KEY_E;
    keycodes[0x021] = KB_KEY_F;
    keycodes[0x022] = KB_KEY_G;
    keycodes[0x023] = KB_KEY_H;
    keycodes[0x017] = KB_KEY_I;
    keycodes[0x024] = KB_KEY_J;
    keycodes[0x025] = KB_KEY_K;
    keycodes[0x026] = KB_KEY_L;
    keycodes[0x032] = KB_KEY_M;
    keycodes[0x031] = KB_KEY_N;
    keycodes[0x018] = KB_KEY_O;
    keycodes[0x019] = KB_KEY_P;
    keycodes[0x010] = KB_KEY_Q;
    keycodes[0x013] = KB_KEY_R;
    keycodes[0x01F] = KB_KEY_S;
    keycodes[0x014] = KB_KEY_T;
    keycodes[0x016] = KB_KEY_U;
    keycodes[0x02F] = KB_KEY_V;
    keycodes[0x011] = KB_KEY_W;
    keycodes[0x02D] = KB_KEY_X;
    keycodes[0x015] = KB_KEY_Y;
    keycodes[0x02C] = KB_KEY_Z;

    keycodes[0x028] = KB_KEY_APOSTROPHE;
    keycodes[0x02B] = KB_KEY_BACKSLASH;
    keycodes[0x033] = KB_KEY_COMMA;
    keycodes[0x00D] = KB_KEY_EQUAL;
    keycodes[0x029] = KB_KEY_GRAVE_ACCENT;
    keycodes[0x01A] = KB_KEY_LEFT_BRACKET;
    keycodes[0x00C] = KB_KEY_MINUS;
    keycodes[0x034] = KB_KEY_PERIOD;
    keycodes[0x01B] = KB_KEY_RIGHT_BRACKET;
    keycodes[0x027] = KB_KEY_SEMICOLON;
    keycodes[0x035] = KB_KEY_SLASH;
    keycodes[0x056] = KB_KEY_WORLD_2;

    keycodes[0x00E] = KB_KEY_BACKSPACE;
    keycodes[0x153] = KB_KEY_DELETE;
    keycodes[0x14F] = KB_KEY_END;
    keycodes[0x01C] = KB_KEY_ENTER;
    keycodes[0x001] = KB_KEY_ESCAPE;
    keycodes[0x147] = KB_KEY_HOME;
    keycodes[0x152] = KB_KEY_INSERT;
    keycodes[0x15D] = KB_KEY_MENU;
    keycodes[0x151] = KB_KEY_PAGE_DOWN;
    keycodes[0x149] = KB_KEY_PAGE_UP;
    keycodes[0x045] = KB_KEY_PAUSE;
    keycodes[0x146] = KB_KEY_PAUSE;
    keycodes[0x039] = KB_KEY_SPACE;
    keycodes[0x00F] = KB_KEY_TAB;
    keycodes[0x03A] = KB_KEY_CAPS_LOCK;
    keycodes[0x145] = KB_KEY_NUM_LOCK;
    keycodes[0x046] = KB_KEY_SCROLL_LOCK;
    keycodes[0x03B] = KB_KEY_F1;
    keycodes[0x03C] = KB_KEY_F2;
    keycodes[0x03D] = KB_KEY_F3;
    keycodes[0x03E] = KB_KEY_F4;
    keycodes[0x03F] = KB_KEY_F5;
    keycodes[0x040] = KB_KEY_F6;
    keycodes[0x041] = KB_KEY_F7;
    keycodes[0x042] = KB_KEY_F8;
    keycodes[0x043] = KB_KEY_F9;
    keycodes[0x044] = KB_KEY_F10;
    keycodes[0x057] = KB_KEY_F11;
    keycodes[0x058] = KB_KEY_F12;
    keycodes[0x064] = KB_KEY_F13;
    keycodes[0x065] = KB_KEY_F14;
    keycodes[0x066] = KB_KEY_F15;
    keycodes[0x067] = KB_KEY_F16;
    keycodes[0x068] = KB_KEY_F17;
    keycodes[0x069] = KB_KEY_F18;
    keycodes[0x06A] = KB_KEY_F19;
    keycodes[0x06B] = KB_KEY_F20;
    keycodes[0x06C] = KB_KEY_F21;
    keycodes[0x06D] = KB_KEY_F22;
    keycodes[0x06E] = KB_KEY_F23;
    keycodes[0x076] = KB_KEY_F24;
    keycodes[0x038] = KB_KEY_LEFT_ALT;
    keycodes[0x01D] = KB_KEY_LEFT_CONTROL;
    keycodes[0x02A] = KB_KEY_LEFT_SHIFT;
    keycodes[0x15B] = KB_KEY_LEFT_SUPER;
    keycodes[0x137] = KB_KEY_PRINT_SCREEN;
    keycodes[0x138] = KB_KEY_RIGHT_ALT;
    keycodes[0x11D] = KB_KEY_RIGHT_CONTROL;
    keycodes[0x036] = KB_KEY_RIGHT_SHIFT;
    keycodes[0x15C] = KB_KEY_RIGHT_SUPER;
    keycodes[0x150] = KB_KEY_DOWN;
    keycodes[0x14B] = KB_KEY_LEFT;
    keycodes[0x14D] = KB_KEY_RIGHT;
    keycodes[0x148] = KB_KEY_UP;

    keycodes[0x052] = KB_KEY_KP_0;
    keycodes[0x04F] = KB_KEY_KP_1;
    keycodes[0x050] = KB_KEY_KP_2;
    keycodes[0x051] = KB_KEY_KP_3;
    keycodes[0x04B] = KB_KEY_KP_4;
    keycodes[0x04C] = KB_KEY_KP_5;
    keycodes[0x04D] = KB_KEY_KP_6;
    keycodes[0x047] = KB_KEY_KP_7;
    keycodes[0x048] = KB_KEY_KP_8;
    keycodes[0x049] = KB_KEY_KP_9;
    keycodes[0x04E] = KB_KEY_KP_ADD;
    keycodes[0x053] = KB_KEY_KP_DECIMAL;
    keycodes[0x135] = KB_KEY_KP_DIVIDE;
    keycodes[0x11C] = KB_KEY_KP_ENTER;
    keycodes[0x037] = KB_KEY_KP_MULTIPLY;
    keycodes[0x04A] = KB_KEY_KP_SUBTRACT;
}

static cri_key s_cri_translate_key(unsigned int wparam, unsigned long lparam) {
    if (wparam == VK_CONTROL) {
        MSG next;
        DWORD time;

        if (lparam & 0x01000000)
            return KB_KEY_RIGHT_CONTROL;

        time = GetMessageTime();
        if (PeekMessageW(&next, 0x0, 0, 0, PM_NOREMOVE))
            if (next.message == WM_KEYDOWN || next.message == WM_SYSKEYDOWN || next.message == WM_KEYUP || next.message == WM_SYSKEYUP)
                if (next.wParam == VK_MENU && (next.lParam & 0x01000000) && next.time == time)
                    return KB_KEY_UNKNOWN;

        return KB_KEY_LEFT_CONTROL;
    }

    if (wparam == VK_PROCESSKEY)
        return KB_KEY_UNKNOWN;

    return (cri_key)keycodes[HIWORD(lparam) & 0x1FF];
}

static void s_cri_destroy_window_data(s_cri_window_data *window_data) {
    if(window_data == NULL)
        return;

    s_cri_window_data_win *window_data_win = (s_cri_window_data_win*)window_data->specific;

    window_data->buffer = NULL;
    if(window_data_win->bmi)
        free(window_data_win->bmi);

    if(window_data_win->hwnd && window_data_win->hdc) {
        ReleaseDC(window_data_win->hwnd, window_data_win->hdc);
        DestroyWindow(window_data_win->hwnd);
    }

    cri_timer_destroy(window_data_win->timer);
    window_data_win->timer = NULL;

    window_data_win->hwnd = NULL;
    window_data_win->hdc = NULL;
    window_data_win->bmi = NULL;
    window_data->close = true;
}

uint64_t s_cri_timer_tick() {
    int64_t counter;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    return counter;
}

void s_cri_timer_init() {
    uint64_t frequency;
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    g_timer_res = 1.0 / (double)((int64_t)frequency);
}
