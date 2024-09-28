#include <windows.h>
#include <stdbool.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool running;
global_variable BITMAPINFO bitmap_info;
global_variable void* bitmap_memory;
global_variable HBITMAP bitmap_handle;
// A device context is a Windows term when working with the GDI, the Windows Graphics Device Interface
// It's a place where you draw, so if you have several different device contexts, you're drawing multiple places,
// such as multiple windows. 
global_variable HDC bitmap_device_context;

internal void win32_resize_DIB_section(int width, int height) 
{
  bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
  bitmap_info.bmiHeader.biWidth = width;
  bitmap_info.bmiHeader.biHeight = height;
  bitmap_info.bmiHeader.biPlanes = 1;
  bitmap_info.bmiHeader.biBitCount = 32;
  bitmap_info.bmiHeader.biCompression = BI_RGB;

  if (bitmap_handle) 
  {
    DeleteObject(bitmap_handle);
  } 

  if (!bitmap_device_context) {
    bitmap_device_context = CreateCompatibleDC(0);
  } 

  bitmap_handle = CreateDIBSection(bitmap_device_context,
                                           &bitmap_info,
                                           DIB_RGB_COLORS,
                                           &bitmap_memory,
                                           0, 0);

}

internal void win32_update_window(HDC device_context, int x, int y, int width, int height) 
{
  StretchDIBits(device_context,
                x, y, width, height,
                x, y, width, height,
                bitmap_memory,
                &bitmap_info,
                DIB_RGB_COLORS, SRCCOPY);
}

LRESULT win32_main_window_callback(HWND window,
                             UINT msg,
                             WPARAM wparam,
                             LPARAM lparam) 
{
  LRESULT result = 0;

  switch (msg) {
    case WM_SIZE:
      {
        RECT client_rect;
        GetClientRect(window, &client_rect);

        int width = client_rect.right - client_rect.left;
        int height = client_rect.bottom - client_rect.top;

        win32_resize_DIB_section(width, height);
      } break;

    case WM_DESTROY:
      {
        running = false;
      } break;

    case WM_CLOSE:
      { 
        running = false;
      } break;

    case WM_ACTIVATEAPP:
      {
        OutputDebugStringA("WM_ACTIVATEAPP\n");
      } break;

    case WM_PAINT:
      {
        PAINTSTRUCT paint;
        HDC device_context = BeginPaint(window, &paint);
        // Perform painting here
        int x = paint.rcPaint.left;
        int y = paint.rcPaint.top;
        int width = paint.rcPaint.right - paint.rcPaint.left;
        int height = paint.rcPaint.bottom - paint.rcPaint.top;
        win32_update_window(device_context, x, y, width, height);

        EndPaint(window, &paint);
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
  window_class.lpfnWndProc = win32_main_window_callback;
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
      running = true;
      while (running) {
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

