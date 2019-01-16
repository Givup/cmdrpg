#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include "screen.h"

int create_screen(Screen* screen, int w, int h) {
  HANDLE default_console = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFO b_info;
  GetConsoleScreenBufferInfo(default_console, &b_info);

  if(b_info.dwSize.X != w || b_info.dwSize.Y != h) {
    printf("Console output size is wrong.\n");
    printf("Would you like the program to set it automatically? [Y/N]");

    int answer = 0;
    do {
      answer = _getch();
    } while(answer != 'Y' && answer != 'N' && answer != 'y' && answer != 'n');
    if(answer == 'Y' || answer == 'y') {
      char buffer[512];
      sprintf(buffer, "mode con:cols=%d lines=%d", w, h);
      system(buffer);
    } else {
      printf("\nPlease set the console window size to %dx%d\n", w, h);
      return 1;
    }
  }

  HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

  if(console == INVALID_HANDLE_VALUE) {
    printf("Failed to create console screen buffer.\n");
    return 1;
  }

  if(SetConsoleActiveScreenBuffer(console) == 0) {
    printf("Failed to set active screen buffer.\n");
    return 1;
  }

  screen->width = w;
  screen->height = h;
  screen->buffer = (CHAR_INFO*)malloc(sizeof(CHAR_INFO) * w * h);
  screen->console_handle = console;

  for(int i = 0;i < w * h;i++) {
    screen->buffer[i].Char.AsciiChar = '.';
    screen->buffer[i].Attributes = FOREGROUND_INTENSITY;
  }
  return 0;
};

int free_screen(Screen* screen) {
  free(screen->buffer);
  CloseHandle(screen->console_handle);
};
 
void show_cursor(Screen* screen, BOOL state) {
  CONSOLE_CURSOR_INFO cursor_info = { 1, state };
  SetConsoleCursorInfo(screen->console_handle, &cursor_info);
};

int get_char_offset(const char* str, int align) {
  if(align == ALIGN_LEFT) {
    return 0;
  }
  int len = strlen(str);

  if(align == ALIGN_CENTER) {
    return -len / 2;
  }
  else {
    return -len;
  }
};

void print_string(Screen* screen, const char* str, WORD attr, int x, int y, int align) {
  int char_offset = get_char_offset(str, align);
  int base_index = y * screen->width + x + char_offset;
  int offset = 0;
  int index;
  while(*str != 0) {
    index = base_index + offset;
    if(index < 0 || index > (screen->width * screen->height)) {
      printf("Out of bounds write @[%d,%d] against [%d, %d].\n", x, y, screen->width, screen->height);
      break;
    }
    if(*str <= 255) {
      screen->buffer[base_index + offset].Char.AsciiChar = *str;
    } else {
      screen->buffer[base_index + offset].Char.UnicodeChar = *str;
    }
    screen->buffer[base_index + offset].Attributes = attr;
    offset++;
    str++;
  }
};

void print_console(Screen* screen) {
  COORD c_pos = { 0, 0 };
  COORD c_size = { screen->width, screen->height };
  SMALL_RECT write_region = { 0, 0, screen->width, screen->height };
  WriteConsoleOutput(screen->console_handle, screen->buffer, c_size, c_pos, &write_region);
};
