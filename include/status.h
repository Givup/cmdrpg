#ifndef _STATUS_H_
#define _STATUS_H_

#include <windows.h>

typedef struct {
  int hp, max_hp;
  int temp;
  int wet;
  int bleeding;
  int infected;
  int hypothermia;
} Status;

extern const char* get_temp_description(int);
extern int get_tile_temp(int);
extern WORD get_temp_attributes(int);
#endif
