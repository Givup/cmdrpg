#ifndef _DIALOG_H_
#define _DIALOG_H_

typedef struct {
  int active; // 0 == not active, > 0 == selected option
  int n_lines;
  int n_options;
  int line_length;
  char** lines;
  char** options;
} DialogSystem;

extern int init_dialog(DialogSystem*);
extern int open_dialog(DialogSystem*, int, int);
extern int free_dialog(DialogSystem*);

extern char** dialog_callback(void*, void*);

#endif
