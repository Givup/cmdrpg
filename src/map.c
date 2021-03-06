#include "map.h"

#include <stdio.h>
#include <stdlib.h>

#define METADATA_HOUSE_LEFT_RIGHT   1
#define METADATA_HOUSE_TOP_BOTTOM   2
#define METADATA_HOUSE_LEFT_TOP     3
#define METADATA_HOUSE_LEFT_BOTTOM  4
#define METADATA_HOUSE_RIGHT_TOP    5
#define METADATA_HOUSE_RIGHT_BOTTOM 6

extern float
randomf();

extern float
randombif();

extern unsigned int
randomi(int);

extern unsigned int
randomi_range(int, int);

const char*
get_tile_str(int tile)
{
  switch(tile) 
  {
    case TILE_GROUND:
    case TILE_ROCK:
    case TILE_SAND: return "\260";

    case TILE_FLOOR:
    case TILE_FOREST:
    case TILE_GRASS:  return "\261";

    case TILE_WATER:  return "\262";

    case TILE_MOUNTAIN: return "\333";

    default: return ".";
  }
};

WORD
get_tile_attributes(int tile)
{
  switch(tile)
  {
    case TILE_GRASS: return BG_LIGHT_GREEN | FG_GREEN;
    case TILE_GROUND: return FG_LIGHT_RED | BG_GRAY;
    case TILE_WATER: return FG_LIGHT_BLUE | BG_BLUE;
    case TILE_SAND: return FG_YELLOW | BG_LIGHT_YELLOW;
    case TILE_FOREST: return FG_LIGHT_GREEN | BG_GREEN;
    case TILE_ROCK: return FG_LIGHT_GRAY | BG_GRAY;
    case TILE_MOUNTAIN: return FG_WHITE | BG_LIGHT_GRAY;
    case TILE_FLOOR: return BG_LIGHT_GRAY;
    default: return FG_MAGENTA;
  }
};

const char*
get_entity_str(int entity, int metadata)
{
  switch(entity)
  {
    case ENTITY_CACTUS: return "f";
    case ENTITY_SCRUB: return "*";
    case ENTITY_WALKED_SNOW: return "\260";
    case ENTITY_COW: return "M";

    case ENTITY_NPC: return "\1";

    case ENTITY_HOUSE:
    {
      // House metadata contains the connection points
      switch(metadata)
      {
	case METADATA_HOUSE_LEFT_RIGHT: return "\315";
	case METADATA_HOUSE_TOP_BOTTOM: return "\272";
	case METADATA_HOUSE_LEFT_TOP: return "\274";
	case METADATA_HOUSE_LEFT_BOTTOM: return "\273";
	case METADATA_HOUSE_RIGHT_TOP: return "\310";
	case METADATA_HOUSE_RIGHT_BOTTOM: return "\311";
	default: return "\316";
      }
    }
    case ENTITY_DOOR: return "\333";
    case ENTITY_MONEY: return "\353";

    default: return " ";
  };
};

WORD
get_entity_attributes(int entity)
{
  switch(entity)
  {
    case ENTITY_CACTUS: return FG_LIGHT_GREEN;
    case ENTITY_SCRUB: return FG_RED;
    case ENTITY_WALKED_SNOW: return FG_WHITE;
    case ENTITY_HOUSE:
    case ENTITY_FLOOR: return FG_BLACK;
    case ENTITY_DOOR: return FG_RED;
    case ENTITY_MONEY: return FG_LIGHT_GREEN;
    case ENTITY_COW: return FG_BLACK;
    case ENTITY_NPC: return FG_BLACK;
    default: return FG_MAGENTA | BG_MAGENTA;
  }
};

void
print_map(struct Map* map, struct Screen* screen)
{
  int tile;
  struct MapEntity entity;
  for(int y = 0; y < map->height; y++)
  {
    for(int x = 0; x < map->width; x++)
    {
      tile = map->tiles[x + y * map->width];
      const char* tile_str = get_tile_str(tile);
      WORD attr = get_tile_attributes(tile);
      
      entity = map->entities[x + y * map->width];
      const char* entity_str = get_entity_str(entity.tile, entity.metadata);
      WORD entity_attr = get_entity_attributes(entity.tile) | (attr & 0xF0);

      print_string(screen, tile_str, attr, x, y, ALIGN_LEFT);
      if(entity.tile != ENTITY_UNDEF)
      {
	print_string(screen, entity_str, entity_attr, x, y, ALIGN_LEFT);
      }
    }
  };
};

float
f_abs(float v)
{
  return v > 0.0f ? v : -v;
}

void
create_biome_pool(struct Map* map, int tile, int* pool, int pool_count)
{
  struct BiomeEntityPool* bpool = &map->biome_pools[tile - 1];
  bpool->pool_size = pool_count;
  bpool->pool = (int*)malloc(sizeof(int) * pool_count * 2);
  memcpy(bpool->pool, pool, sizeof(int) * pool_count *  2);
};

int
generate_house(struct Map* map, int x, int y, int w, int h)
{
  int empty_space = 3; // How much empty space there needs to be surrounding the house

  // If the place we were going to generate house already has one, stop generation
  for(int x0 = x - w - empty_space; x0 <= x + w + empty_space; x0++)
  {
    for(int y0 = y - h - empty_space; y0 <= y + h + empty_space; y0++)
    {
      if(x0 < 0 || x0 >= map->width || y0 < 0 || y0 >= map->height) return 0;
      if(map->entities[x0 + y0 * map->width].tile == ENTITY_HOUSE) return 0;
      if(map->tiles[x0 + y0 * map->width] == TILE_WATER) return 0;
    }
  }

  // Fill the floor
  for(int x0 = x - w; x0 <= x + w; x0++)
  {
    for(int y0 = y - h; y0 <= y + h; y0++)
    {
      set_entity(map, x0, y0, ENTITY_UNDEF, 0);
      if(randomi(100) == 0)
      {
	set_entity(map, x0, y0, ENTITY_MONEY, randomi_range(10, 25));
      }
      set_tile_at(map, x0, y0, TILE_FLOOR);
    }
  }

  // Outlines
  // Top and bottom
  for(int x0 = x - w; x0 <= x + w; x0++)
  {
    int metadata = METADATA_HOUSE_LEFT_RIGHT;

    if(x0 == x - w) metadata = METADATA_HOUSE_RIGHT_BOTTOM;
    if(x0 == x + w) metadata = METADATA_HOUSE_LEFT_BOTTOM;

    set_entity(map, x0, y - h, ENTITY_HOUSE, metadata);

    if(x0 == x - w) metadata = METADATA_HOUSE_RIGHT_TOP;
    if(x0 == x + w) metadata = METADATA_HOUSE_LEFT_TOP;

    set_entity(map, x0, y + h, ENTITY_HOUSE, metadata);
  }
  // Left and right
  for(int y0 = y - h; y0 <= y + h; y0++)
  {
    int metadata = METADATA_HOUSE_TOP_BOTTOM;
    
    if(y0 == y - h) metadata = METADATA_HOUSE_RIGHT_BOTTOM;
    if(y0 == y + h) metadata = METADATA_HOUSE_RIGHT_TOP;
    
    set_entity(map, x - w, y0, ENTITY_HOUSE, metadata);

    if(y0 == y - h) metadata = METADATA_HOUSE_LEFT_BOTTOM;
    if(y0 == y + h) metadata = METADATA_HOUSE_LEFT_TOP;

    set_entity(map, x + w, y0, ENTITY_HOUSE, metadata);
  }

  int side = randomi(4);
  
  int dx, dy;

  if(side == 0) // Top
  {
    dx = randomi_range(x - w + 1, x + w);
    dy = y - h;
  }
  else if(side == 1) // Right
  {
    dx = x + w;
    dy = randomi_range(y - h + 1, y + h);
  }
  else if(side == 2) // Bottom
  {
    dx = randomi_range(x - w + 1, x + w);
    dy = y + h;
  }
  else // Left
  {
    dx = x - w;
    dy = randomi_range(y - h + 1, y + h);
  }

  // Make door hole
  map->entities[dx + dy * map->width].tile = ENTITY_DOOR;

  // Spawn humans inside?  
  // Loop through insides
  int humans_spawned = 0;
  for(int y0 = y - h + 1; y0 < y + h - 1; y0++)
  {
    for(int x0 = x - w + 1; x0 < x + w - 1 && humans_spawned < 3; x0++)
    {
      if(randomi(1000) > 900)
      {
	set_entity(map, x0, y0, ENTITY_NPC, 0);
	humans_spawned++;
      }
    }
  }

  return 1;
};

void
create_map(struct Map* map, int w, int h)
{
  map->width = w;
  map->height = h;
  map->tiles = (int*)malloc(sizeof(int) * w * h);
  map->entities = (struct MapEntity*)malloc(sizeof(struct MapEntity) * w * h);

  for(int i = 0;i < w * h; i++)
  {
    map->tiles[i] = TILE_UNDEF;
    map->entities[i].tile = ENTITY_UNDEF;
  }

  map->biome_pools = (struct BiomeEntityPool*)malloc(sizeof(struct BiomeEntityPool) * TILE_COUNT);
  for(int bp = 0;bp < TILE_COUNT;bp++)
  {
    map->biome_pools[bp].pool_size = 0;
  }
  map->pool_count = TILE_COUNT;
  
  int forest_pool[] = { ENTITY_COW, 5, ENTITY_NPC, 1 };
  create_biome_pool(map, TILE_GRASS, forest_pool, sizeof(forest_pool) / sizeof(int) / 2);

  int sand_pool[] = { ENTITY_CACTUS, 100, ENTITY_SCRUB, 100, ENTITY_HOUSE, 5, ENTITY_COW, 10 };
  create_biome_pool(map, TILE_SAND, sand_pool, sizeof(sand_pool) / sizeof(int) / 2);
};

void
free_biome_pool(struct BiomeEntityPool* pool)
{
  free(pool->pool);
};

void
free_map(struct Map* map)
{
  free(map->tiles);
  free(map->entities);

  for(int bp = 0;bp < map->pool_count; bp += 2)
  {
    free_biome_pool(&map->biome_pools[bp]);
  }
  free(map->biome_pools);
};

int
get_background_of_map_at(struct Map* map, int x, int y)
{
  if(x < 0 || x >= map->width || y < 0 || y >= map->height)
  {
    return 0;
  };
  return get_tile_attributes(map->tiles[x + y * map->width]) & 0xF0;
};

int
get_foreground_of_map_at(struct Map* map, int x, int y)
{
  if(x < 0 || x >= map->width || y < 0 || y >= map->height)
  {
    return 0;
  };
  return get_tile_attributes(map->tiles[x + y * map->width]) & 0x0F;
};

const char*
get_biome_name(int biome)
{
  switch(biome)
  {
    case TILE_GRASS: return "Grasslands";
    case TILE_GROUND: return "Flatlands";
    case TILE_WATER: return "Water";
    case TILE_SAND: return "Beach";
    case TILE_FOREST: return "Thick forest";
    case TILE_ROCK: return "Mountain";
    case TILE_MOUNTAIN: return "Snowy Mountain";
    default: return "Void";
  };
};

int
get_tile_at(struct Map* map, int x, int y)
{
  if(x < 0 || x >= map->width || y < 0 || y >= map->height) return TILE_UNDEF;
  return map->tiles[x + y * map->width];
};

int
set_tile_at(struct Map* map, int x, int y, int tile)
{
  if(x < 0 || x >= map->width || y < 0 || y >= map->height) return -1;
  map->tiles[x + y * map->width] = tile;
  return 0;
};

int
get_tile_traverse_penalty(struct Map* map, int tile)
{
  switch(tile)
  {
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

void
set_entity(struct Map* map, int x, int y, int entity_id, int metadata)
{
  map->entities[x + y * map->width].tile = entity_id;
  map->entities[x + y * map->width].metadata = metadata;
};

struct MapEntity*
get_entity(struct Map* map, int x, int y)
{
  if(x < 0 || x >= map->width || y < 0 || y >= map->height) return NULL;
  return &map->entities[x + y * map->width];
};

void
clear_entities(struct Map* map)
{
  memset(map->entities, 0, sizeof(struct MapEntity) * map->width * map->height);
};

void
update_entity(struct MapEntity* entity, struct Map* map, int cx, int cy)
{
  switch(entity->tile)
  {
    case ENTITY_COW:
    {
      if(randomi(1000) > 750 && entity->metadata == 0)
      {
	int x_off = (int)(randomi_range(1, 4)) - 2;
	int y_off = (int)(randomi_range(1, 4)) - 2;

	if(!can_move_to(map, (cx + x_off + map->width) % map->width, cy % map->height))
	{
	  x_off = 0;
	}
	if(!can_move_to(map, cx % map->width, (cy + y_off + map->height) % map->height))
	{
	  y_off = 0;
	}

	int nx = (cx + x_off + map->width) % map->width;
	int ny = (cy + y_off + map->height) % map->height;
	
	if(x_off != 0 || y_off != 0)
	{
	  set_entity(map, nx, ny, entity->tile, entity->metadata++);
	  set_entity(map, cx, cy, ENTITY_UNDEF, 0);
	}
      }
    }
    break;
  }
};

void
update_entities(struct Map* map)
{
  for(int e = 0; e < map->width * map->height;e++)
  {
    if(map->entities[e].tile != ENTITY_UNDEF)
    {
      update_entity(&map->entities[e], map, e % map->width, e / map->width);
    }
  }
};

void
reset_entity(struct MapEntity* entity, struct Map* map, int cx, int cy)
{
  switch(entity->tile)
  {
    case ENTITY_COW:
    {
      entity->metadata = 0;
      break;
    }
  }
};

void
reset_entities(struct Map* map)
{
  for(int e = 0; e < map->width * map->height;e++)
  {
    if(map->entities[e].tile != ENTITY_UNDEF)
    {
      reset_entity(&map->entities[e], map, e % map->width, e / map->width);
    }
  }
};

int
get_entity_pool_for_biome(struct Map* map, int biome_tile, int** pool)
{
  struct BiomeEntityPool* bpool = &map->biome_pools[biome_tile - 1];
  *pool = bpool->pool;
  return bpool->pool_size;
};

void
populate_biome(struct Map* map, int biome_tile)
{
  int n_entity_pool = 0;
  int* entity_pool = NULL;

  n_entity_pool = get_entity_pool_for_biome(map, biome_tile, &entity_pool);

  if(n_entity_pool == 0)
  {
    return;
  }

  for(int y = 0;y < map->height;y++)
  {
    for(int x = 0;x < map->width;x++)
    {
      if(x == map->width / 2 && y == map->height / 2) continue;
      if(map->entities[x + y * map->width].tile != ENTITY_UNDEF) continue;
      if(map->tiles[x + y * map->width] == TILE_FLOOR) continue;
      int r = randomi(n_entity_pool) * 2;
      int entity = entity_pool[r];
      int chance = entity_pool[r + 1];
      if(randomi(1000) >= (1000 - chance))
      {
	if(entity == ENTITY_HOUSE)
	{
	  generate_house(map, x, y, randomi_range(3, 5), randomi_range(3, 5));
	}
	else
	{
	  map->entities[x + y * map->width].tile = entity;
	}
      }
    }
  }
};

// Load map file into map
void
load_map(struct Map *map, const char *map_file)
{
  clear_entities(map);

  char tilemap_file[256];
  strcpy(tilemap_file, map_file);
  strcat(tilemap_file, ".tmap");

  // File pointer
  FILE *fp = fopen(tilemap_file, "rb");

  // Find file size
  fseek(fp, 0L, SEEK_END);
  size_t fsize = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  // Allocate a buffer for the whole buffer
  char *buffer = malloc(fsize);

  fread(buffer, 1, fsize, fp);

  fclose(fp);

  int width = 0, height = 0;
  int spawn_x, spawn_y;
  int buffer_index = 0;

  sscanf(buffer, "%d %d %d %d\n%n", &width, &height, 
	 &spawn_x, &spawn_y, &buffer_index);

  map->width = width;
  map->height = height;
  map->spawn_x = spawn_x;
  map->spawn_y = spawn_y;
  
  // Go through all the rows
  for(int y = 0;y < height;y++)
  {
    // Go through all the columns
    for(int x = 0;x < width; x++)
    {
      int tile = TILE_UNDEF;
      switch(buffer[buffer_index])
      {
	case '#': tile = TILE_GRASS; break;
	case 'w': tile = TILE_WATER; break;
      }
      if(buffer[buffer_index] == '\n' || buffer[buffer_index] == '\r')
      {
	buffer_index++; // Advance char buffer
	x--; // Go back one since we continue here
	continue;
      }
      map->tiles[x + y * map->width] = tile;
      buffer_index++;
    }
  }

  // Free allocated buffer
  free(buffer);

  int generated = 0;
  // Generate houses?
  for(int i = 0; i < 50 && generated < 6; i++)
  {
    int x = randomi(map->width), y = randomi(map->height);
    int w = 2 + randomi(6), h = 2 + randomi(4);
    generated += generate_house(map, x, y, w, h);
  }
};

int
generate_biome_at(struct Map* map, int _x, int _y)
{
  int biome_tile = map->tiles[_x + _y * map->width];

  if(biome_tile != TILE_UNDEF)
  {
    for(int i = 0;i < map->width * map->height;++i)
    {
      map->tiles[i] = biome_tile;
    }
    populate_biome(map, biome_tile);
    return 1;
  }
  return 0;
};

int
can_move_to(struct Map* map, int x, int y)
{
  int entity = map->entities[x + y * map->width].tile;
  switch(entity)
  {
    case ENTITY_COW:
    case ENTITY_SCRUB:
    case ENTITY_CACTUS:
    case ENTITY_HOUSE: return 0;
  }

  int tile = map->tiles[x + y * map->width];
  
  switch(tile)
  {
    case TILE_WATER: return 0;
  }

  return 1;
};

void
try_move_to(struct Map* map, int x, int y, struct Status* status)
{
  int tile = map->tiles[x + y * map->width];
  int entity = map->entities[x + y * map->width].tile;

  if(entity == ENTITY_CACTUS)
  {
    status->bleeding |= 1;
  }
  if(tile == TILE_WATER)
  {
    status->wet |= 1;
  }
};
