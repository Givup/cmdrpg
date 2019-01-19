#ifndef _TILE_MAP_H_
#define _TILE_MAP_H_

#include "screen.h"

typedef struct {
  int pool_size;
  int* pool;
} BiomeEntityPool;

typedef struct {
  int width, height;
  int* tiles;
  int* entities;

  int pool_count;
  BiomeEntityPool* biome_pools;
} Map;

extern const char* get_tile_str(int tile);
extern WORD get_tile_attributes(int tile);
extern void print_map(Map*, Screen*);

extern void create_map(Map*, int, int);
extern void free_map(Map*);

extern int get_background_of_map_at(Map*, int, int);
extern int get_foreground_of_map_at(Map*, int, int);

extern void generate_map(Map*, int, int, int, int);
extern int  generate_biome_at(Map*, int, int);

extern void clear_entities(Map*);

#endif
