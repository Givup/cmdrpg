#define _WIN32_WINNT 0x0502
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "perlin.h"
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

  load_permutation("perlin_seed");

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

  int changed;
  while(1) {
    if(window_handle != GetForegroundWindow()) continue;

    _getch(); // To capture the input to console

    changed = 0;

    if(GetKeyState(VK_SPACE) & 0x8000) {
      break;
    }

    if(GetKeyState(0x57) & 0x8000) {
      changed |= print_string(&screen, "W", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
    }
    else if(GetKeyState(0x41) & 0x8000) {
      changed |= print_string(&screen, "A", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
    }
    else if(GetKeyState(0x53) & 0x8000) {
      changed |= print_string(&screen, "S", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
    }
    else if(GetKeyState(0x44) & 0x8000) {
      changed |= print_string(&screen, "D", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
    }

    if(changed) {
      print_console(&screen);
    }

    Sleep(10);
  }

  free_map(&map);
  free_screen(&screen);

  return 0;
};
