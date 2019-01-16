#include <stdlib.h>
#include "map.h"

#define TILE_UNDEF  0
#define TILE_GRASS  1
#define TILE_GROUND 2
#define TILE_WATER  3

extern int randomi(int);

const char* get_tile_str(int tile) {
  switch(tile) {
  case TILE_GRASS:  return "\261";
  case TILE_GROUND: return "\260";
  case TILE_WATER:  return "\262";
  default: return ".";
  }
};

WORD get_tile_attributes(int tile) {
  switch(tile) {
  case TILE_GRASS: return FG_LIGHT_GREEN | BG_YELLOW;
  case TILE_GROUND: return FG_GREEN;
  case TILE_WATER: return FG_LIGHT_BLUE | BG_BLUE;
  default: return FG_MAGENTA;
  }
};

void print_map(Map* map, Screen* screen) {
  int index;
  int tile;
  for(int y = 0; y < map->height; y++) {
    for(int x = 0; x < map->width; x++) {
      index = x + y * map->width;
      tile = map->tiles[index];
      const char* tile_str = get_tile_str(tile);
      WORD attr = get_tile_attributes(tile);
      print_string(screen, tile_str, attr, x, y, ALIGN_LEFT);
    }
  };
};

void create_map(Map* map, int w, int h) {
  map->width = w;
  map->height = h;
  map->tiles = (int*)malloc(sizeof(int) * w * h);
  for(int i = 0;i < w * h;i++) {
    map->tiles[i] = randomi(3) + 1;
  }
};

void free_map(Map* map) {
  free(map->tiles);
};
