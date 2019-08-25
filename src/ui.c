#include "ui.h"
#include <stdio.h>

#include "log.h"

int
create_ui(struct UISystem* system, struct Screen* screen)
{
  system->screen = screen;
  return 0;
};

int
create_ui_panel(struct UIPanel* panel, int x, int y, int w, int h, WORD clear, WORD margin, WORD render)
{
  panel->x = x;
  panel->y = y;
  panel->w = w;
  panel->h = h;
  panel->margin_horizontal = 0;
  panel->margin_vertical = 0;
  panel->attributes_clear = clear;
  panel->attributes_margin = margin;
  panel->attributes_render = render;
  panel->text_alignment = 0;
  panel->user_data = NULL;
  panel->text_callback = NULL;
  return 0;
};

int
resize_ui_panel(struct UIPanel* panel, int x, int y, int w, int h)
{
  panel->x = x;
  panel->y = y;
  panel->w = w;
  panel->h = h;
  return 0;
};

int
set_margin_ui_panel(struct UIPanel* panel, int margin_h, int margin_v)
{
  panel->margin_horizontal = margin_h;
  panel->margin_vertical = margin_v;
  return 0;
};

int
set_callback_ui_panel(struct UIPanel* panel, void* data, char**(*callback)(void*, void*))
{
  panel->user_data = data;
  panel->text_callback = callback;
  return 0;
};

int
render_ui_panel(struct UISystem* system, struct UIPanel* panel, char** text)
{
  if(panel->text_callback != NULL)
  {
    text = panel->text_callback(system, panel);
  }
  if(text == NULL)
  {
    return 1;
  }

  int text_index = -panel->margin_vertical;
  char* buffer = (char*)malloc(panel->w + 1);

  int x0 = panel->x - panel->margin_horizontal;
  int x1 = panel->x + panel->w + panel->margin_horizontal;
  int y0 = panel->y - panel->margin_vertical;
  int y1 = panel->y + panel->h + panel->margin_vertical;

  for(int y = y0; y < y1; y++)
  {
    for(int x = x0; x < x1; x++)
    {
      WORD attribs = panel->attributes_clear;
      char *pstr = " ";
      if(panel->margin_vertical > 0 && panel->margin_horizontal > 0)
      {
	attribs = panel->attributes_margin;
	if(y == y0 && x == x0)      { pstr = "\xC9"; }
	else if(y == y0 && x == x1 - 1) { pstr = "\xBB"; }
	else if(y == y1 - 1 && x == x0) { pstr = "\xC8"; }
	else if(y == y1 - 1 && x == x1 - 1) { pstr = "\xBC"; }
	else if((x == x0 || x == x1 - 1) && (y > y0 && y < y1)) { pstr = "\xBA"; }
	else if((y == y0 || y == y1 - 1) && (x > x0 && x < x1)) { pstr = "\xCD"; }
	else { attribs = panel->attributes_clear; }
      }
      print_string(system->screen, pstr, attribs, x, y, ALIGN_LEFT);
    }

    if(text_index < 0)
    {
      text_index++;
    }
    else if(*(text + text_index) != 0 && text_index < panel->h + panel->margin_vertical * 2)
    {

      int length = min(strlen(*(text + text_index)), panel->w - panel->margin_horizontal * 2);

      memcpy(buffer, *(text + text_index), length);
      if(length < panel->w) buffer[length] = 0;

      int x_start = panel->x + panel->margin_horizontal;

      if(panel->text_alignment == ALIGN_RIGHT)
      {
	x_start = panel->x + panel->w - panel->margin_horizontal;
      }
      else if(panel->text_alignment == ALIGN_CENTER)
      {
	x_start = panel->x + panel->w / 2;
      }

      print_string(system->screen, buffer, panel->attributes_render, x_start, y, panel->text_alignment);
      
text_index++;
    }
  }
  
  if(panel->text_callback != NULL)
  {
    /*
      for(int i = 0;i < text_index;i++) {
      free(text[i]);
      }
      free(text);
    */
  }

  free(buffer);
  return 0;
};
