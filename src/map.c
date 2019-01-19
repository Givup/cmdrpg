#include "map.h"
#include "perlin.h"

#include <stdlib.h>

extern float randomf();
extern float randombif();
extern unsigned int randomi(int);
extern unsigned int randomi_range(int, int);

const char* get_tile_str(int tile) {
  switch(tile) {

  case TILE_GROUND:
  case TILE_ROCK:
  case TILE_SAND: return "\260";

  case TILE_FOREST:
  case TILE_GRASS:  return "\261";

  case TILE_WATER:  return "\262";

  case TILE_MOUNTAIN: return "\333";

  default: return ".";
  }
};

WORD get_tile_attributes(int tile) {
  switch(tile) {
  case TILE_GRASS: return BG_LIGHT_GREEN | FG_GREEN;
  case TILE_GROUND: return FG_LIGHT_RED | BG_GRAY;
  case TILE_WATER: return FG_LIGHT_BLUE | BG_BLUE;
  case TILE_SAND: return FG_YELLOW | BG_LIGHT_YELLOW;
  case TILE_FOREST: return FG_LIGHT_GREEN | BG_GREEN;
  case TILE_ROCK: return FG_LIGHT_GRAY | BG_GRAY;
  case TILE_MOUNTAIN: return FG_WHITE | BG_LIGHT_GRAY;
  default: return FG_MAGENTA;
  }
};

const char* get_entity_str(int entity) {
  switch(entity) {
  case ENTITY_CACTUS: return "f";
  case ENTITY_SCRUB: return "*";
  case ENTITY_WALKED_SNOW: return "\260";
  default: return " ";
  };
};

WORD get_entity_attributes(int entity) {
  switch(entity) {
  case ENTITY_CACTUS: return FG_LIGHT_GREEN;
  case ENTITY_SCRUB: return FG_RED;
  case ENTITY_WALKED_SNOW: return FG_WHITE;
  default: return FG_MAGENTA | BG_MAGENTA;
  }
};

void print_map(Map* map, Screen* screen) {
  int tile, entity;
  for(int y = 0; y < map->height; y++) {
    for(int x = 0; x < map->width; x++) {
      tile = map->tiles[x + y * map->width];
      const char* tile_str = get_tile_str(tile);
      WORD attr = get_tile_attributes(tile);
      
      entity = map->entities[x + y * map->width];
      const char* entity_str = get_entity_str(entity);
      WORD entity_attr = get_entity_attributes(entity) | (attr & 0xF0);

      print_string(screen, tile_str, attr, x, y, ALIGN_LEFT);
      if(entity != ENTITY_UNDEF) {
	print_string(screen, entity_str, entity_attr, x, y, ALIGN_LEFT);
      }
    }
  };
};

float f_abs(float v) {
  return v > 0.0f ? v : -v;
}

void create_biome_pool(Map* map, int tile, int* pool, int pool_count) {
  BiomeEntityPool* bpool = &map->biome_pools[tile - 1];
  bpool->pool_size = pool_count;
  bpool->pool = (int*)malloc(sizeof(int) * pool_count * 2);
  memcpy(bpool->pool, pool, sizeof(int) * pool_count *  2);
};

void create_map(Map* map, int w, int h) {
  map->width = w;
  map->height = h;
  map->tiles = (int*)malloc(sizeof(int) * w * h);
  map->entities = (int*)malloc(sizeof(int) * w * h);

  for(int i = 0;i < w * h; i++) {
    map->tiles[i] = TILE_UNDEF;
    map->entities[i] = ENTITY_UNDEF;
  }

  map->biome_pools = (BiomeEntityPool*)malloc(sizeof(BiomeEntityPool) * TILE_COUNT);
  for(int bp = 0;bp < TILE_COUNT;bp++) {
    map->biome_pools[bp].pool_size = 0;
  }
  map->pool_count = TILE_COUNT;
  
  int sand_pool[] = { ENTITY_CACTUS, 100, ENTITY_SCRUB, 100 };
  create_biome_pool(map, TILE_SAND, sand_pool, sizeof(sand_pool) / sizeof(int) / 2);
};

void free_biome_pool(BiomeEntityPool* pool) {
  free(pool->pool);
};

void free_map(Map* map) {
  free(map->tiles);
  free(map->entities);

  for(int bp = 0;bp < map->pool_count; bp += 2) {
    free_biome_pool(&map->biome_pools[bp]);
  }
  free(map->biome_pools);
};

int get_background_of_map_at(Map* map, int x, int y) {
  if(x < 0 || x >= map->width || y < 0 || y >= map->height) {
    return 0;
  };
  return get_tile_attributes(map->tiles[x + y * map->width]) & 0xF0;
};

int get_foreground_of_map_at(Map* map, int x, int y) {
  if(x < 0 || x >= map->width || y < 0 || y >= map->height) {
    return 0;
  };
  return get_tile_attributes(map->tiles[x + y * map->width]) & 0x0F;
};

const char* get_biome_name(int biome) {
  switch(biome) {
  case TILE_GRASS: return "Grasslands";
  case TILE_GROUND: return "Flatlands";
  case TILE_WATER: return "Water . . .";
  case TILE_SAND: return "Beach";
  case TILE_FOREST: return "Thick forest";
  case TILE_ROCK: return "Mountain";
  case TILE_MOUNTAIN: return "Snowy Mountain";
  default: return "Void";
  };
};

int get_tile_at(Map* map, int x, int y) {
  if(x < 0 || x >= map->width || y < 0 || y >= map->height) return TILE_UNDEF;
  return map->tiles[x + y * map->width];
};

int get_tile_traverse_penalty(Map* map, int tile) {
  switch(tile) {
  case TILE_ROCK:
  case TILE_FOREST:
    return 2;
  case TILE_WATER:
    return 3;
  case TILE_MOUNTAIN:
    return 4;
  default: return 1;
  };
};

void set_entity(Map* map, int x, int y, int entity_id) {
  map->entities[x + y * map->width] = entity_id;
};

void generate_map(Map* map, int x, int y, int w, int h) {
  float step = 3.0f;
  float x_offset = (float)x * step;
  float y_offset = (float)y * step;

  for(int i = 0;i < w * h;i++) {
    float x = (float)(i % w) * step / (float)w;
    float y = (float)(i / w) * step / (float)h;

    float n = -(noise(x + x_offset / (float)w, y + y_offset / (float)h, (float)w / (float)h)) * 2.0f; // Range [-1.0f : 1.0f]

    if(f_abs(n) < 0.075f) {
      map->tiles[i] = TILE_WATER;
    }
    else if(f_abs(n) < 0.15f) {
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
    else if(n <= -0.5f && n > -0.9f) {
      map->tiles[i] = TILE_GRASS;
    }
    else if(n <= -0.9f) {
      map->tiles[i] = TILE_FOREST;
    }
    else {
      map->tiles[i] = TILE_UNDEF;
    }
  }
};

int get_entity_pool_for_biome(Map* map, int biome_tile, int** pool) {
  BiomeEntityPool* bpool = &map->biome_pools[biome_tile - 1];
  *pool = bpool->pool;
  return bpool->pool_size;
};

void populate_biome(Map* map, int biome_tile) {
  int n_entity_pool = 0;
  int* entity_pool = NULL;

  n_entity_pool = get_entity_pool_for_biome(map, biome_tile, &entity_pool);

  if(n_entity_pool == 0) {
    return;
  }

  for(int y = 0;y < map->height;y++) {
    for(int x = 0;x < map->width;x++) {
      if(x == map->width / 2 && y == map->height / 2) continue;
      int r = randomi(n_entity_pool) * 2;
      int entity = entity_pool[r];
      int chance = entity_pool[r + 1];
      if(randomi(1000) >= (1000 - chance)) {
	map->entities[x + y * map->width] = entity;
      }
    }
  }
};

int generate_biome_at(Map* map, int _x, int _y) {
  int biome_tile = map->tiles[_x + _y * map->width];

  if(biome_tile != TILE_UNDEF) {
    for(int i = 0;i < map->width * map->height;++i) {
      map->tiles[i] = biome_tile;
    }
    populate_biome(map, biome_tile);
    return 1;
  }
  return 0;
};

int can_move_to(Map* map, int x, int y) {
  int entity = map->entities[x + y * map->width];
  switch(entity) {

  case ENTITY_SCRUB:
  case ENTITY_CACTUS: return 0;

  default: return 1;
  }
};

void try_move_to(Map* map, int x, int y, Status* status) {
  int tile = map->tiles[x + y * map->width];
  int entity = map->entities[x + y * map->width];

  if(entity == ENTITY_CACTUS) {
    status->bleeding |= 1;
  }
  if(tile == TILE_WATER) {
    status->wet |= 1;
  }
};

void clear_entities(Map* map) {
  memset(map->entities, 0, sizeof(int) * map->width * map->height);
};
