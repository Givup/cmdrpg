#include "dialog.h"
#include "player.h"
#include "ui.h"

#include "log.h"

#include <string.h>
#include <stdio.h>

int init_dialog(DialogSystem* dialog) {
  dialog->active = 0;
  dialog->n_lines = 0;
  dialog->n_options = 0;
  dialog->line_length = 0;
  dialog->lines = NULL;
  dialog->options = NULL;
};

int open_dialog(DialogSystem* dialog, int diag_major, int diag_minor) {
  char src_path[256];
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

  int longest = 0;

  PRINT_LOG("Lines: %c", '\n');

  char line_buffer[256];

  for(int i = 0; i < lines; i++) {
    dialog->lines[i] = malloc(sizeof(char) * 256);
    memset(dialog->lines[i], 0, 256);
    if(fgets(line_buffer, 256, dialogue_file)) {
      PRINT_LOG("\t%s", line_buffer);
      longest = max(longest, strlen(line_buffer) - 1);
      memcpy(dialog->lines[i], line_buffer, strlen(line_buffer) - 1);
    }
  }

  int temp;

  PRINT_LOG("Options: %c", '\n');
  for(int i = 0; i < options; i++) {
    dialog->options[i] = malloc(sizeof(char) * 256);
    memset(dialog->options[i], 0, 256);
    fscanf(dialogue_file, ".%d ", &temp);
    if(fgets(line_buffer, 256, dialogue_file)) {
      PRINT_LOG("\t%s", line_buffer);
      longest = max(longest, strlen(line_buffer) - 1);
      memcpy(dialog->options[i], line_buffer, strlen(line_buffer) - 1);
    }
  }

  PRINT_LOG("Longest line: %d\n", longest);

  dialog->line_length = longest;
  dialog->active = 1;
  return 0;
};

int free_dialog(DialogSystem* dialog) {
  if(dialog->n_lines) {
    for(int i = 0;i< dialog->n_lines; i++) {
      free(dialog->lines[i]);
    }
    free(dialog->lines); dialog->n_lines = 0;
  }
  if(dialog->n_options) {
    for(int i = 0;i< dialog->n_options; i++) {
      free(dialog->options[i]);
    }
    free(dialog->options); dialog->n_options = 0;
  }
  return 0;
};

char** dialog_callback(void* p_system, void* p_panel) {
  UISystem* system = (UISystem*)p_system;
  UIPanel* panel = (UIPanel*)p_panel;
  DialogSystem* dialog = (DialogSystem*)panel->user_data;
  char** lines = NULL;
  int index = 0;

  int line_count = dialog->n_lines + dialog->n_options + 2;

  PRINT_LOG("\nDialog callback, lines: %d\n", line_count);

  lines = (char**)malloc(sizeof(char*) * line_count);

  for(int i = 0;i < line_count;i++) {
    lines[i] = (char*)malloc(sizeof(char) * (dialog->line_length + 2));
  }

  for(int i = 0;i < dialog->n_lines;i++) {
    sprintf(lines[index], "%s", dialog->lines[i]);
    index++;
  }
  sprintf(lines[index++], "-OPTIONS-");
  for(int i = 0;i < dialog->n_options;i++) {
    sprintf(lines[index], "%c%s", (dialog->active - 1 == i ? '>' : ' '), dialog->options[i]);
    index++;
  }
  lines[line_count - 1] = 0;
  
  return lines;
};
