#ifndef _TILE_MAP_H_
#define _TILE_MAP_H_

#include "screen.h"
#include "status.h"

#define TILE_UNDEF    0
#define TILE_GRASS    1
#define TILE_GROUND   2
#define TILE_WATER    3
#define TILE_SAND     4
#define TILE_FOREST   5
#define TILE_ROCK     6
#define TILE_MOUNTAIN 7
#define TILE_FLOOR    8
#define TILE_COUNT    9

#define ENTITY_UNDEF       0
#define ENTITY_CACTUS      1
#define ENTITY_SCRUB       2
#define ENTITY_WALKED_SNOW 3
#define ENTITY_HOUSE       4
#define ENTITY_DOOR        5
#define ENTITY_FLOOR       6
#define ENTITY_MONEY       7

typedef struct {
  int pool_size;
  int* pool;
} BiomeEntityPool;

typedef struct {
  int tile;
  int metadata;
} MapEntity;

typedef struct {
  int width, height;
  int* tiles;
  MapEntity* entities;

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

extern const char* get_biome_name(int);
extern int get_tile_at(Map*, int, int);
extern int set_tile_at(Map*, int, int, int);

extern int get_tile_traverse_penalty(Map*, int);

extern void set_entity(Map*, int, int, int, int);
extern MapEntity* get_entity(Map*, int, int);

extern void generate_map(Map*, int, int, int, int);
extern int  generate_biome_at(Map*, int, int);

extern int can_move_to(Map*, int, int);
extern void try_move_to(Map*, int, int, Status*);

extern void clear_entities(Map*);

#endif
