#ifndef _TILE_MAP_H_
#define _TILE_MAP_H_

#include "screen.h"

typedef struct {
  int width, height;
  int* tiles;
} Map;

extern const char* get_tile_str(int tile);
extern WORD get_tile_attributes(int tile);

extern void print_map(Map*, Screen*);
extern void create_map(Map*, int, int);

extern void free_map(Map*);

#endif
