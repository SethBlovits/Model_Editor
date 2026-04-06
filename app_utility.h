#ifndef APP_UTILITY_H
#define APP_UTILITY_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#ifdef _WIN32
#include <Windows.h>
#include <commdlg.h>
#endif

#define INVALID 0

struct {
    LARGE_INTEGER frequency;
    LARGE_INTEGER previous_time;
    LARGE_INTEGER current_time;
    float delta_time;
    float current_time_float;
} app_time;


typedef enum APP_EVENT_CODE{
    APP_EVENT_MOUSE_MOVE = 1,
    APP_EVENT_MOUSE_LEFT_BUTTON_DOWN = 2,
    APP_EVENT_MOUSE_LEFT_BUTTON_UP = 3,
    APP_EVENT_MOUSE_RIGHT_BUTTON_DOWN = 4,
    APP_EVENT_MOUSE_RIGHT_BUTTON_UP = 5,
    APP_EVENT_KEY_UP = 6,
    APP_EVENT_KEY_DOWN = 7,
    APP_EVENT_SCROLL_WHEEL = 8,
    APP_EVENT_CHAR = 9,
    APP_EVENT_MOUSE_LEAVE = 10
}APP_EVENT_CODE;

typedef enum APP_KEY_CODE{
    KEYCODE_A = 1,
    KEYCODE_B = 2,
    KEYCODE_C = 3,
    KEYCODE_D = 4,
    KEYCODE_E = 5,
    KEYCODE_F = 6,
    KEYCODE_G = 7,
    KEYCODE_H = 8,
    KEYCODE_I = 9,
    KEYCODE_J = 10,
    KEYCODE_K = 11,
    KEYCODE_L = 12,
    KEYCODE_M = 13,
    KEYCODE_N = 14,
    KEYCODE_O = 15,
    KEYCODE_P = 16,
    KEYCODE_Q = 17,
    KEYCODE_R = 18,
    KEYCODE_S = 19,
    KEYCODE_T = 20,
    KEYCODE_U = 21,
    KEYCODE_V = 22,
    KEYCODE_W = 23,
    KEYCODE_X = 24,
    KEYCODE_Y = 25,
    KEYCODE_Z = 26,

    KEYCODE_0 = 27,
    KEYCODE_1 = 28,
    KEYCODE_2 = 29,
    KEYCODE_3 = 30,
    KEYCODE_4 = 31,
    KEYCODE_5 = 32,
    KEYCODE_6 = 33,
    KEYCODE_7 = 34,
    KEYCODE_8 = 35,
    KEYCODE_9 = 36,

    /* Special keys */
    KEYCODE_ESCAPE      = 37,
    KEYCODE_ENTER       = 38,
    KEYCODE_TAB         = 39,
    KEYCODE_BACKSPACE   = 40,
    KEYCODE_SPACE       = 41,

    KEYCODE_LEFT_SHIFT  = 42,
    KEYCODE_RIGHT_SHIFT = 43,
    KEYCODE_LEFT_CTRL   = 44,
    KEYCODE_RIGHT_CTRL  = 45,
    KEYCODE_LEFT_ALT    = 46,
    KEYCODE_RIGHT_ALT   = 47,
    KEYCODE_CAPS_LOCK   = 48,

    KEYCODE_INSERT      = 49,
    KEYCODE_DELETE      = 50,
    KEYCODE_HOME        = 51,
    KEYCODE_END         = 52,
    KEYCODE_PAGE_UP     = 53,
    KEYCODE_PAGE_DOWN   = 54,

    KEYCODE_ARROW_UP    = 55,
    KEYCODE_ARROW_DOWN  = 56,
    KEYCODE_ARROW_LEFT  = 57,
    KEYCODE_ARROW_RIGHT = 58
}APP_KEY_CODE;


typedef struct{
    int event_code;
    int keycode;
    char event_char;
    short mouse_wheel_delta;
}app_event_t;

typedef void (*UserFunc)();
typedef void (*EventFunc)(app_event_t* event);

typedef struct{
    UserFunc init_func;
    UserFunc frame_func;
    UserFunc cleanup_func;
    EventFunc event_func;
    int width;
    int height;
}app_desc_t;

#ifdef _WIN32
struct app_data_t{
    HWND hwnd;
    UINT app_dpi;
    char app_working_directory[MAX_PATH];
}app_data_d3d12;
#endif

UserFunc app_init_func;
UserFunc app_frame_func;
UserFunc app_cleanup_func;
EventFunc app_event_func;

//public function declarations
void app_init();
void app_mainline();
void app_cleanup();
void app_get_cursor_pos(int*,int*);

//private windows only declarations

void _create_window_Win32(int,int);
void _app_frame_loop_Win32();
LRESULT CALLBACK _event_callback_Win32(HWND,UINT,WPARAM,LPARAM);
int _conv_event_Win32(int,WPARAM,app_event_t*);
int _conv_keycode_Win32(WPARAM);
void _get_cursor_pos_Win32(int*,int*);


#ifdef APP_UTILITY_IMPLEMENTATION

#ifdef _WIN32

int _open_file_dialog_Win32(char* out_path, DWORD out_size){    
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn,sizeof(ofn));
    ZeroMemory(out_path,out_size);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "All Files\0*.*\0";
    ofn.lpstrFile = out_path;
    ofn.nMaxFile = out_size;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if(GetOpenFileName(&ofn)){
        return 1;//succeeded
    }
    return 0; //failed
}

void _get_cursor_pos_Win32(int *out_x,int *out_y){
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(app_data_d3d12.hwnd,&point);

    *out_x = (int)point.x;
    *out_y = (int)point.y;
}
int _conv_keycode_Win32(WPARAM wparam){
    switch(wparam){
         /* Letters A–Z */
        case 'A': return KEYCODE_A;
        case 'B': return KEYCODE_B;
        case 'C': return KEYCODE_C;
        case 'D': return KEYCODE_D;
        case 'E': return KEYCODE_E;
        case 'F': return KEYCODE_F;
        case 'G': return KEYCODE_G;
        case 'H': return KEYCODE_H;
        case 'I': return KEYCODE_I;
        case 'J': return KEYCODE_J;
        case 'K': return KEYCODE_K;
        case 'L': return KEYCODE_L;
        case 'M': return KEYCODE_M;
        case 'N': return KEYCODE_N;
        case 'O': return KEYCODE_O;
        case 'P': return KEYCODE_P;
        case 'Q': return KEYCODE_Q;
        case 'R': return KEYCODE_R;
        case 'S': return KEYCODE_S;
        case 'T': return KEYCODE_T;
        case 'U': return KEYCODE_U;
        case 'V': return KEYCODE_V;
        case 'W': return KEYCODE_W;
        case 'X': return KEYCODE_X;
        case 'Y': return KEYCODE_Y;
        case 'Z': return KEYCODE_Z;

        /* Number row 0–9 */
        case '0': return KEYCODE_0;
        case '1': return KEYCODE_1;
        case '2': return KEYCODE_2;
        case '3': return KEYCODE_3;
        case '4': return KEYCODE_4;
        case '5': return KEYCODE_5;
        case '6': return KEYCODE_6;
        case '7': return KEYCODE_7;
        case '8': return KEYCODE_8;
        case '9': return KEYCODE_9;

        /* Special keys */
        case VK_ESCAPE:    return KEYCODE_ESCAPE;
        case VK_RETURN:    return KEYCODE_ENTER;
        case VK_TAB:       return KEYCODE_TAB;
        case VK_BACK:      return KEYCODE_BACKSPACE;
        case VK_SPACE:     return KEYCODE_SPACE;

        case VK_LSHIFT:    return KEYCODE_LEFT_SHIFT;
        case VK_RSHIFT:    return KEYCODE_RIGHT_SHIFT;
        case VK_CONTROL:   return KEYCODE_LEFT_CTRL;
        case VK_LCONTROL:  return KEYCODE_LEFT_CTRL;
        case VK_RCONTROL:  return KEYCODE_RIGHT_CTRL;
        case VK_LMENU:     return KEYCODE_LEFT_ALT;
        case VK_RMENU:     return KEYCODE_RIGHT_ALT;
        case VK_CAPITAL:   return KEYCODE_CAPS_LOCK;

        case VK_INSERT:    return KEYCODE_INSERT;
        case VK_DELETE:    return KEYCODE_DELETE;
        case VK_HOME:      return KEYCODE_HOME;
        case VK_END:       return KEYCODE_END;
        case VK_PRIOR:     return KEYCODE_PAGE_UP;
        case VK_NEXT:      return KEYCODE_PAGE_DOWN;

        case VK_UP:        return KEYCODE_ARROW_UP;
        case VK_DOWN:      return KEYCODE_ARROW_DOWN;
        case VK_LEFT:      return KEYCODE_ARROW_LEFT;
        case VK_RIGHT:     return KEYCODE_ARROW_RIGHT;
    }

    return INVALID;
}
int _conv_event_Win32(int win_event_code,WPARAM wparam,app_event_t* event){
    switch(win_event_code){
        case WM_LBUTTONUP:
        {
            ReleaseCapture();
            return APP_EVENT_MOUSE_LEFT_BUTTON_UP;
        }
        case WM_LBUTTONDOWN:
        {
            SetCapture(app_data_d3d12.hwnd);
            return APP_EVENT_MOUSE_LEFT_BUTTON_DOWN;
        } 
        case WM_MOUSELEAVE:
        {
            return APP_EVENT_MOUSE_LEAVE;
        }
        case WM_RBUTTONDOWN: return APP_EVENT_MOUSE_RIGHT_BUTTON_DOWN;
        case WM_RBUTTONUP: return APP_EVENT_MOUSE_RIGHT_BUTTON_UP;
        case WM_KEYDOWN:
        {
            event->keycode = _conv_keycode_Win32(wparam);
            return APP_EVENT_KEY_DOWN;
        }
        case WM_KEYUP:
        {
            event->keycode = _conv_keycode_Win32(wparam);
            return APP_EVENT_KEY_UP;
        }
        case WM_MOUSEWHEEL:
        {
            short z_delta = GET_WHEEL_DELTA_WPARAM(wparam);
            event->mouse_wheel_delta = z_delta;
            return APP_EVENT_SCROLL_WHEEL;
        }
        case WM_CHAR:
        {
            event->event_char = (char)wparam;
            return APP_EVENT_CHAR;
        }
    }

    return INVALID;
}
void _create_window_Win32(int width, int height){
    const char CLASS_NAME[] = "Window Class";

    QueryPerformanceFrequency(&app_time.frequency);
    QueryPerformanceCounter(&app_time.previous_time);
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEX wc;

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc   = _event_callback_Win32;
    wc.cbClsExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = CLASS_NAME;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)){
        MessageBoxEx(NULL,"Window Registration Failed","ERROR!",0,
            MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    

    // Adjust for window decorations (title bar, borders)
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_CLIENTEDGE;

    // Adjust for BOTH the style AND extended style
    RECT rect = {0, 0, width, height};
    AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle); // Use AdjustWindowRectEx!

    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    app_data_d3d12.hwnd = CreateWindowEx(
        dwExStyle,           // Use the same extended style
        CLASS_NAME,
        "APP_WINDOW",
        dwStyle,             // Use the same style
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL);

    if(app_data_d3d12.hwnd == NULL){
        MessageBoxEx(NULL,"Window Creation Failed", "ERROR!",0,
            MB_OK|MB_ICONEXCLAMATION);
            return;
    }
    app_data_d3d12.app_dpi = GetDpiForWindow(app_data_d3d12.hwnd);
    return;

    
}

void _app_frame_loop_Win32(){

    ShowWindow(app_data_d3d12.hwnd,SW_SHOWNORMAL);
    UpdateWindow(app_data_d3d12.hwnd);
    while(true){
        QueryPerformanceCounter(&app_time.current_time);
        app_time.delta_time = (float)(app_time.current_time.QuadPart-app_time.previous_time.QuadPart)/app_time.frequency.QuadPart;
        app_time.current_time_float = (float)app_time.current_time.QuadPart;
        app_time.previous_time = app_time.current_time;
        MSG msg;
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            if(msg.message == WM_QUIT){
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg); 
        }
        app_frame_func();
    
    }
    
}

LRESULT CALLBACK _event_callback_Win32(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    app_event_t event;
    event.event_code = _conv_event_Win32(msg,wParam,&event);

    if(event.event_code == INVALID){
        switch(msg){
            case WM_CREATE:
            {
                LPCREATESTRUCT pCreateStruct = (LPCREATESTRUCT)lParam;
                SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)pCreateStruct->lpCreateParams);
            }
            break;
            case WM_SIZE:
            {
                
            }
            break;
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                BeginPaint(hwnd,&ps);
                EndPaint(hwnd,&ps);
                return 0;
            }
            break;
            case WM_CLOSE:
            {
                DestroyWindow(hwnd);
            }
            break;
            case WM_DESTROY:
            {
                app_cleanup();
                PostQuitMessage(0);
            }
            break;
            default:
                return DefWindowProc(hwnd,msg,wParam,lParam);
        }
    }
    else if(app_event_func != NULL){
        app_event_func(&event);
    }
    return 0;
}

void _app_init_working_directory_Win32(){
    GetCurrentDirectory(MAX_PATH,app_data_d3d12.app_working_directory);
}

void _app_reset_working_directory_Win32(){
    SetCurrentDirectory(app_data_d3d12.app_working_directory);
}
//public functions
void app_open_file_dialog(char* out_path,long out_size){
    #ifdef _WIN32
    _open_file_dialog_Win32(out_path,(DWORD)out_size);
    #endif
}


HWND app_get_window_handle(){
    #ifdef _WIN32 
    return app_data_d3d12.hwnd;
    #endif
}
void app_reset_working_directory(){
    #ifdef _WIN32
    _app_reset_working_directory_Win32();
    #endif
}

#endif//win32
void app_get_cursor_pos(int* x,int* y){
    #ifdef _WIN32
    _get_cursor_pos_Win32(x,y);
    #endif
}

inline float app_get_delta_time(){
    return app_time.delta_time;
}
inline float app_get_current_time(){
    return app_time.current_time_float;
}
void app_init(app_desc_t app_desc){
    app_init_func = app_desc.init_func;
    app_frame_func = app_desc.frame_func;
    app_cleanup_func = app_desc.cleanup_func;
    app_event_func = app_desc.event_func;
    
    #ifdef _WIN32
    _create_window_Win32(app_desc.width,app_desc.height);
    _app_init_working_directory_Win32();
    #endif
    app_init_func();
}
void app_mainline(){
    #ifdef _WIN32
    _app_frame_loop_Win32();
    #endif
}
void app_cleanup(){
    app_cleanup_func();
}

#endif//utility implementation
#endif //total head endif


