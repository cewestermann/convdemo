#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool running;
global_variable BITMAPINFO bitmap_info;
global_variable void* bitmap_memory;

global_variable int bitmap_width;
global_variable int bitmap_height;

// A device context is a Windows term when working with the GDI, the Windows Graphics Device Interface
// It's a place where you draw, so if you have several different device contexts, you're drawing multiple places,
// such as multiple windows. 

internal void win32_resize_DIB_section(int width, int height) 
{
  bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
  bitmap_info.bmiHeader.biWidth = bitmap_width;
  bitmap_info.bmiHeader.biHeight = bitmap_height;
  bitmap_info.bmiHeader.biPlanes = 1;
  bitmap_info.bmiHeader.biBitCount = 32;
  bitmap_info.bmiHeader.biCompression = BI_RGB;

  if (bitmap_memory) {
    if (VirtualFree(bitmap_memory, 0, MEM_RELEASE) != 0) {
      fprintf(stderr, "Error in freeing memory\n");
    };
  } 

  bitmap_width = width;
  bitmap_height = height;

  int bytes_per_pixel = 4;
  int bitmap_memory_size = bytes_per_pixel * bitmap_width * bitmap_height;
  // Returns memory in pages. Since we pass in 0 for the starting address of the memory (as we don't really care)
  // Windows automatically rounds up to the next page boundary, so that we don't cross more pages than necessary.
  // The bitmap_memory size we need is specifically on this laptop: 3518976 bytes or 3436 KiB. Windows uses
  // 4 KiB pages as default, which means that we've allocated 859 pages and we can step through them in the
  // memory view of a debugger and we can see that they are all zeroed out.
  bitmap_memory = VirtualAlloc(0, bitmap_memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

}

internal void win32_update_window(HDC device_context, RECT* client_rect)
{
  int window_width = client_rect->right - client_rect->left;
  int window_height = client_rect->bottom - client_rect->top;
  
  StretchDIBits(device_context,
                0, 0, window_width, window_height,
                0, 0, bitmap_width, bitmap_height,
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

        RECT client_rect;
        GetClientRect(window, &client_rect);

        win32_update_window(device_context, &client_rect);

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

