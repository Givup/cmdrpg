#include "ui.h"
#include <stdio.h>

int create_ui(UISystem* system, Screen* screen) {
  system->screen = screen;
  return 0;
};

int create_ui_panel(UIPanel* panel, int x, int y, int w, int h, WORD clear, WORD render) {
  panel->x = x;
  panel->y = y;
  panel->w = w;
  panel->h = h;
  panel->margin_horizontal = 0;
  panel->margin_vertical = 0;
  panel->attributes_clear = clear;
  panel->attributes_render = render;
  panel->text_callback = NULL;
  return 0;
};

int set_margin_ui_panel(UIPanel* panel, int margin_h, int margin_v) {
  panel->margin_horizontal = margin_h;
  panel->margin_vertical = margin_v;
  return 0;
};

int set_ui_panel_callback(UIPanel* panel, void* data, const char**(*callback)(void*, void*)) {
  panel->user_data = data;
  panel->text_callback = callback;
  return 0;
};

int render_ui_panel(UISystem* system, UIPanel* panel, const char** text) {
  if(panel->text_callback != NULL) {
    text = panel->text_callback(system, panel->user_data);
  }

  int text_index = -panel->margin_vertical;
  char* buffer = (char*)malloc(panel->w + 1);

  for(int y = panel->y;y < panel->y + panel->h; y++) {
    for(int x = panel->x; x < panel->x + panel->w; x++) {
      print_string(system->screen, " ", panel->attributes_clear, x, y, ALIGN_LEFT);
    }

    if(text_index < 0) {
      text_index++;
    } else if(*(text + text_index) != 0 && text_index < panel->h - panel->margin_vertical * 2) {

      int length = min(strlen(*(text + text_index)), panel->w - panel->margin_horizontal * 2);

      memcpy(buffer, *(text + text_index), length);
      if(length < panel->w) buffer[length] = 0;

      print_string(system->screen, buffer, panel->attributes_render, panel->x + panel->margin_horizontal, y, ALIGN_LEFT);

      text_index++;
    }
  }

  free(buffer);
  return 0;
};
