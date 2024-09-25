#include <windows.h>

int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCMDLine,
                     int nShowCmd) {
  MessageBox(0, "This is my first message box","This is the title of the message box", MB_ICONINFORMATION | MB_OK);

  return 0;
} 

