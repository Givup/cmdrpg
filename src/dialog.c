#include "item.h"
#include "dialog.h"
#include "ui.h"

#include "log.h"

#include <string.h>
#include <stdio.h>

int
init_dialog(struct DialogSystem* dialog)
{
  dialog->major = -1;
  dialog->minor = -1;
  dialog->active = 0;
  dialog->n_lines = 0;
  dialog->n_options = 0;
  dialog->line_length = 0;
  dialog->lines = NULL;
  dialog->options = NULL;
  dialog->options_ptr = NULL;
};

int
open_dialog(struct DialogSystem *dialog, struct Inventory *inventory, int diag_major, int diag_minor)
{
  struct ItemList *item_list = inventory->list;

  char src_path[256];

  dialog->major = diag_major;
  dialog->minor = diag_minor;

  strcpy(src_path, "dialogue/");
  sprintf(src_path, "dialogue/%d/%d.dlg", diag_major, diag_minor);

  PRINT_LOG("\nOpening dialogue: %s\n", src_path);

  FILE* dialogue_file = fopen(src_path, "r");
  int lines, options;
  fscanf(dialogue_file, "%d %d\n", &lines, &options);

  PRINT_LOG("There are %d lines and %d options.\n", lines, options);

  dialog->n_lines = lines;
  dialog->n_options = options;

  dialog->lines = malloc(sizeof(char*) * lines);
  dialog->options = malloc(sizeof(char*) * options);
  dialog->options_ptr = malloc(sizeof(int) * options);

  int longest = 0;

  PRINT_LOG("Lines: %c", '\n');

  char line_buffer[256];

  for(int i = 0; i < lines; i++)
  {
    dialog->lines[i] = malloc(sizeof(char) * 256);
    memset(dialog->lines[i], 0, 256);
    if(fgets(line_buffer, 256, dialogue_file))
    {
      PRINT_LOG("\t%s", line_buffer);
      longest = max(longest, strlen(line_buffer) - 1);
      memcpy(dialog->lines[i], line_buffer, strlen(line_buffer) - 1);
    }
  }

  PRINT_LOG("Options: %c", '\n');
  for(int i = 0; i < options; i++)
  {
    dialog->options[i] = malloc(sizeof(char) * 256);
    memset(dialog->options[i], 0, 256);

    int temp = 0;
    int mode = 0;
    char modechar;
    fread(&modechar, 1, 1, dialogue_file);

    if(modechar == '.')
    {
      fscanf(dialogue_file, "%d ", &temp);
      mode = DIAG_MODE_ADVANCE;
      fgets(line_buffer, 256, dialogue_file);
      line_buffer[strlen(line_buffer) - 1] = 0; // "clip" newlines
    }
    else if(modechar == '$')
    {
      fscanf(dialogue_file, "%[^\n]\n", &line_buffer[0]);
      temp = get_item_by_name(item_list, line_buffer);

      memset(line_buffer, 0, 256);

      char price_buffer[32];
      sprintf(price_buffer, "%d", item_list->items[temp].price);

      strcat(line_buffer, item_list->items[temp].name);
      strcat(line_buffer, " (");
      strcat(line_buffer, price_buffer);
      strcat(line_buffer, ") "); // End with ' ' to render the whole thing
      // @Bug longest line goes offpanel by horizontal margin

      mode = DIAG_MODE_PURCHASE;
    }
    else if(modechar == 'A')
    {
      fscanf(dialogue_file, "%[^\n]\n", &line_buffer[0]);
      temp = get_item_by_name(item_list, line_buffer);
      memset(line_buffer, 0, 256);

      if(temp == -1)
      {
	sprintf(line_buffer, "INVALID_ID");
      }
      else
      {
	sprintf(line_buffer, "%d x %s", inventory->items[temp], item_list->items[temp].name);
      }
      mode = DIAG_MODE_AMOUNT;
    }
    else
    {
      PRINT_LOG("Unknown mode character: %c\n", modechar);
    }

    dialog->options_ptr[i] = temp | mode;

    PRINT_LOG("\t%s", line_buffer);
    longest = max(longest, strlen(line_buffer));
    memcpy(dialog->options[i], line_buffer, strlen(line_buffer));
  }

  PRINT_LOG("Longest line: %d\n", longest);

  dialog->line_length = longest;
  dialog->active = 1;
  return 0;
};

int
reload_dialog(struct DialogSystem *dialog, struct Inventory *inventory)
{
  int dactive, dmajor, dminor;
  dactive = dialog->active;
  dmajor = dialog->major;
  dminor = dialog->minor;

  free_dialog(dialog);
  open_dialog(dialog, inventory, dmajor, dminor);
  dialog->active = dactive;  
};

int
free_dialog(struct DialogSystem* dialog)
{
  if(dialog->n_lines)
  {
    for(int i = 0;i< dialog->n_lines; i++)
    {
      free(dialog->lines[i]);
    }
    free(dialog->lines); dialog->n_lines = 0;
  }
  if(dialog->n_options)
  {
    for(int i = 0;i< dialog->n_options; i++)
    {
      free(dialog->options[i]);
    }
    free(dialog->options_ptr);
    free(dialog->options); dialog->n_options = 0;
  }
  return 0;
};

char**
dialog_callback(void* p_system, void* p_panel)
{
  struct UISystem* system = (struct UISystem*)p_system;
  struct UIPanel* panel = (struct UIPanel*)p_panel;

  struct UICallbackData* data = (struct UICallbackData*)panel->user_data;
  struct DialogSystem *dialog = data->dialog;

  char** lines = NULL;
  int index = 0;

  int line_count = dialog->n_lines + dialog->n_options + 2;

  PRINT_LOG("\nDialog callback, lines: %d\n", line_count);

  lines = (char**)malloc(sizeof(char*) * line_count);

  for(int i = 0;i < line_count;i++)
  {
    lines[i] = (char*)malloc(sizeof(char) * (dialog->line_length + 2));
  }

  for(int i = 0;i < dialog->n_lines;i++)
  {
    sprintf(lines[index], "%s", dialog->lines[i]);
    index++;
  }
  sprintf(lines[index++], "-OPTIONS-");
  for(int i = 0;i < dialog->n_options;i++)
  {
    sprintf(lines[index], "%c%s", (dialog->active - 1 == i ? '>' : ' '), dialog->options[i]);
    index++;
  }
  lines[line_count - 1] = 0;
  
  return lines;
};
