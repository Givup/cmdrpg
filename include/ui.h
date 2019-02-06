#ifndef _UI_H_
#define _UI_H_

#include "screen.h"

typedef struct {
  int x, y, w, h;
  int margin_horizontal;
  int margin_vertical;
  WORD attributes_clear;
  WORD attributes_render;
  int text_alignment;
  void* user_data;
  char** (*text_callback)(void*, void*); // UISystem*, UIPanel*
} UIPanel;

typedef struct {
  Screen* screen; // Pointer to the screen we'll be rendering to
} UISystem;

extern int create_ui(UISystem*, Screen*);

extern int create_ui_panel(UIPanel*, int, int, int, int, WORD, WORD);
extern int resize_ui_panel(UIPanel*, int, int, int, int);
extern int set_margin_ui_panel(UIPanel*, int, int);
extern int set_ui_panel_callback(UIPanel*, void*, char**(*callbackNULL)(void*, void*));

extern int render_ui_panel(UISystem*, UIPanel*, char**);

#endif
