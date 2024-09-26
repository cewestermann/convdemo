#include <windows.h>

LRESULT main_window_callback(HWND window,
                             UINT msg,
                             WPARAM wparam,
                             LPARAM lparam) 
{
  LRESULT result = 0;

  switch (msg) {
    case WM_SIZE:
      {
        OutputDebugStringA("WM_SIZE\n");
      } break;

    case WM_DESTROY:
      {
        OutputDebugStringA("WM_DESTROY\n");
      } break;

    case WM_CLOSE:
      {
        OutputDebugStringA("WM_CLOSE\n");
      } break;

    case WM_ACTIVATEAPP:
      {
        OutputDebugStringA("WM_ACTIVATEAPP\n");
      } break;

    default:
      {
        result = DefWindowProc(window, msg, wparam, lparam);

      } break;
  } 

  return result;
} 




int CALLBACK WinMain(HINSTANCE instance,
                     HINSTANCE prev_instance,
                     LPSTR cmd_line,
                     int show_code) {
  WNDCLASSA window_class = {0}; 
  window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = main_window_callback;
  window_class.hInstance = instance;
  window_class.lpszClassName = "ConvDemoWindowClass";

  if (RegisterClassA(&window_class)) {

    HWND window = CreateWindowExA(0, window_class.lpszClassName, "Conv Demo",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  0, 0, instance, 0);
    if (window) {
      // Success
      for (;;) {
        MSG message;
        BOOL message_result = GetMessage(&message, 0, 0, 0);
        if (message_result > 0) {
          // Process message and go get another message
          TranslateMessage(&message);
          DispatchMessageA(&message);

        } else {
          break;
        } 
      }

    } else {
      // Failed, TODO: logging
    } 
      
  } else {

    // TODO: logging
  }

  return 0;
} 

