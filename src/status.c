#include "status.h"

#include "map.h"
#include "screen.h"

const char* get_temp_description(int temp) {
  if(temp < -25) {
    return "dead cold";
  } 
  else if(temp < -10) {
    return "freezing";
  } 
  else if(temp < 0) {
    return "cold";
  } 
  else if(temp < 5) {
    return "chilly";
  } 
  else if(temp < 15) {
    return "comfortable";
  } 
  else if(temp < 25) {
    return "hot";
  } 
  else return "dead hot";
};

int get_tile_temp(int tile) {
  switch(tile) {
  case TILE_MOUNTAIN: return -25;
  case TILE_ROCK: return 1;
  case TILE_WATER: return 8;
  case TILE_GROUND: return 10;
  case TILE_GRASS: return 15;
  case TILE_FOREST: return 15;
  case TILE_SAND: return 30;
  default: return 10;
  }
};

WORD get_temp_attributes(int temp) {
  if(temp < 0) {
    return FG_CYAN;
  }
  else {
    return FG_LIGHT_RED;
  }
};

