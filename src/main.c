#define _WIN32_WINNT 0x0502
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "screen.h"
#include "map.h"

#define SW 120
#define SH 40

float randomf() {
  return (float)rand() / (float)RAND_MAX;
};

int randomi(int limit) {
  return rand() % limit;
};

int main(int argc, char** argv) {

  HWND window_handle = GetForegroundWindow();
  
  system("title DNG_CMD");

  Screen screen;
  if(create_screen(&screen, SW, SH)) {
    printf("Failed to create screen.\n");
    return 1;
  }

  show_cursor(&screen, FALSE);

  Map map;
  create_map(&map, SW, SH);

  print_map(&map, &screen);
  print_console(&screen);
  while(1) {
    if(GetKeyState(VK_SPACE) & 0x8000) {
      if(window_handle == GetForegroundWindow()) {
	break;
      }
    }
    Sleep(10);
  }

  free_map(&map);
  free_screen(&screen);

  return 0;
};
