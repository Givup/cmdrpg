#include "map.h"
#include "perlin.h"

#include <stdlib.h>

#define TILE_UNDEF    0
#define TILE_GRASS    1
#define TILE_GROUND   2
#define TILE_WATER    3
#define TILE_SAND     4
#define TILE_FOREST   5
#define TILE_ROCK     6
#define TILE_MOUNTAIN 7

extern int randomi(int);

const char* get_tile_str(int tile) {
  switch(tile) {
  case TILE_FOREST: return "\235";

  case TILE_ROCK:
  case TILE_SAND:
  case TILE_GROUND: return "\260";

  case TILE_GRASS:  return "\261";

  case TILE_MOUNTAIN:
  case TILE_WATER:  return "\262";

  default: return ".";
  }
};

WORD get_tile_attributes(int tile) {
  switch(tile) {
  case TILE_GRASS: return FG_LIGHT_GREEN | BG_GREEN;
  case TILE_GROUND: return FG_GRAY;
  case TILE_WATER: return FG_LIGHT_BLUE | BG_BLUE;
  case TILE_SAND: return FG_YELLOW | BG_LIGHT_GRAY;
  case TILE_FOREST: return FG_GREEN | BG_LIGHT_GREEN;
  case TILE_ROCK: return FG_LIGHT_GRAY | BG_GRAY;
  case TILE_MOUNTAIN: return FG_WHITE | BG_LIGHT_GRAY;
  default: return FG_MAGENTA;
  }
};

void print_map(Map* map, Screen* screen) {
  int tile;
  for(int y = 0; y < map->height; y++) {
    for(int x = 0; x < map->width; x++) {
      tile = map->tiles[x + y * map->width];
      const char* tile_str = get_tile_str(tile);
      WORD attr = get_tile_attributes(tile);
      print_string(screen, tile_str, attr, x, y, ALIGN_LEFT);
    }
  };
};

float f_abs(float v) {
  return v > 0.0f ? v : -v;
}

void create_map(Map* map, int w, int h) {
  map->width = w;
  map->height = h;
  map->tiles = (int*)malloc(sizeof(int) * w * h);

  float step = 5.0f;
  float x_offset = 0.0f, y_offset = 0.0f;

  for(int i = 0;i < w * h;i++) {
    float x = (float)(i % w) * step / (float)w + x_offset;
    float y = (float)(i / w) * step / (float)h + y_offset;
    float n = -(noise(x, y, (float)w / (float)h)) * 2.0f; // Range [-1.0f : 1.0f]

    if(f_abs(n) < 0.2f) {
      map->tiles[i] = TILE_WATER;
    }
    else if(f_abs(n) < 0.4f) {
      map->tiles[i] = TILE_SAND;
    }
    else if(f_abs(n) < 0.5f) {
      map->tiles[i] = TILE_GROUND;
    }
    else if(n >= 0.5f && n < 0.99f) {
      map->tiles[i] = TILE_ROCK;
    }
    else if(n >= 0.99f) {
      map->tiles[i] = TILE_MOUNTAIN;
    }
    else if(n <= -0.5f && n > -0.8f) {
      map->tiles[i] = TILE_GRASS;
    }
    else if(n <= -0.8f) {
      map->tiles[i] = TILE_FOREST;
    }
    else {
      map->tiles[i] = TILE_UNDEF;
    }
  }
};

void free_map(Map* map) {
  free(map->tiles);
};
