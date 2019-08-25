#ifndef _DIALOG_H_
#define _DIALOG_H_

struct Inventory;

#define DIAG_MODE_ADVANCE  0x100
#define DIAG_MODE_PURCHASE 0x200
#define DIAG_MODE_AMOUNT   0x400

struct DialogSystem {
  int major, minor; // Current dialog major and minor
  int active; // 0 == not active, > 0 == selected option
  int n_lines;
  int n_options;
  int line_length;
  char** lines;
  char** options;
  int* options_ptr;
};

extern int
init_dialog(struct DialogSystem*);

extern int
open_dialog(struct DialogSystem*, struct Inventory*, int, int);

extern int
reload_dialog(struct DialogSystem*, struct Inventory*);

extern int
free_dialog(struct DialogSystem*);

extern char**
dialog_callback(void*, void*);

#endif
