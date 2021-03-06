#ifndef _STATUS_H_
#define _STATUS_H_

#include <windows.h>

struct Status {
  int hp, max_hp;
  int hunger, max_hunger;
  int thirst, max_thirst;
  int temp;
  int wet;
  int bleeding;
  int infected;
  int hypothermia;
  int heat_stroke; // Should this and hypothermia be combined
};

extern void 
init_status(struct Status*, int, int, int);

extern const char*
get_temp_description(int);

extern int
get_tile_temp(int);

extern WORD
get_temp_attributes(int);

extern void
apply_status(const struct Status a, struct Status* b);

extern int
tick_status(struct Status* status);

#endif

 
