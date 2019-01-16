#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <windows.h>

#define FG_BLUE         (FOREGROUND_BLUE)
#define FG_LIGHT_BLUE   (FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define FG_GREEN        (FOREGROUND_GREEN)
#define FG_LIGHT_GREEN  (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define FG_RED          (FOREGROUND_RED)
#define FG_LIGHT_RED    (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define FG_YELLOW       (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define FG_MAGENTA      (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define FG_CYAN         (FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define FG_TURQUOISE    (FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FG_GRAY         (FOREGROUND_INTENSITY)
#define FG_LIGHT_GRAY   (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FG_WHITE        (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define FG_BLACK        (0)

#define BG_BLUE         (BACKGROUND_BLUE)
#define BG_LIGHT_BLUE   (BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define BG_GREEN        (BACKGROUND_GREEN)
#define BG_LIGHT_GREEN  (BACKGROUND_GREEN | BACKGROUND_INTENSITY)
#define BG_RED          (BACKGROUND_RED)
#define BG_LIGHT_RED    (BACKGROUND_RED | BACKGROUND_INTENSITY)
#define BG_YELLOW       (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY)
#define BG_MAGENTA      (BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define BG_CYAN         (BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define BG_TURQUOISE    (BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BG_GRAY         (BACKGROUND_INTENSITY)
#define BG_LIGHT_GRAY   (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BG_WHITE        (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define BG_BLACK        (0)

#define ALIGN_LEFT   0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT  2

typedef struct {
  int width, height;
  CHAR_INFO* buffer;
  HANDLE console_handle;
} Screen;

extern int create_screen(Screen*, int, int);
extern int free_screen(Screen*);

extern void show_cursor(Screen*, BOOL);

// Screen pointer, string, Colors, x, y, alignment
extern void print_string(Screen*, const char*, WORD, int, int, int);

extern void print_console(Screen*);

#endif
