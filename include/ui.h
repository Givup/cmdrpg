#ifndef _UI_H_
#define _UI_H_

#include "screen.h"

typedef struct {
  int x, y, w, h;
  int margin_horizontal;
  int margin_vertical;
  WORD attributes_clear;
  WORD attributes_render;
} UIPanel;

typedef struct {
  Screen* screen; // Pointer to the screen we'll be rendering to
} UISystem;

extern int create_ui(UISystem*, Screen*);

extern int create_ui_panel(UIPanel*, int, int, int, int, WORD, WORD);
extern int set_margin_ui_panel(UIPanel*, int, int);

extern int render_ui_panel(UISystem*, UIPanel*, const char**);

#endif
