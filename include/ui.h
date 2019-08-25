#ifndef _UI_H_
#define _UI_H_

#include "screen.h"

struct Status;
struct Inventory;

struct UICallbackData {
  struct Status *status;
  struct Inventory *inventory;
  struct DialogSystem *dialog;
};

struct UIPanel {
  int x, y, w, h;
  int margin_horizontal;
  int margin_vertical;
  WORD attributes_clear;
  WORD attributes_margin;
  WORD attributes_render;
  int text_alignment;
  void* user_data;
  char** (*text_callback)(void*, void*); // UISystem*, struct UIPanel*
};

struct UISystem {
  struct Screen* screen; // Pointer to the screen we'll be rendering to
};

extern int
create_ui(struct UISystem*, struct Screen*);

extern int
create_ui_panel(struct UIPanel*, int, int, int, int, WORD, WORD, WORD);

extern int
resize_ui_panel(struct UIPanel*, int, int, int, int);

extern int
set_margin_ui_panel(struct UIPanel*, int, int);

extern int
set_callback_ui_panel(struct UIPanel*, void*, char**(*callbackNULL)(void*, void*));

extern int
render_ui_panel(struct UISystem*, struct UIPanel*, char**);

#endif
