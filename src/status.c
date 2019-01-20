#include "status.h"

#include "core.h"
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
  case TILE_SAND: return 40;
  default: return 10;
  }
};

WORD get_temp_attributes(int temp) {
  if(temp < 0) {
    return FG_CYAN;
  } else if(temp > 20) {
    return FG_LIGHT_RED;
  } else {
    return FG_WHITE;
  }
};

void init_status(Status* status, int hp, int hunger, int thirst) {
  status->hp = hp;
  status->max_hp = hp;
  status->hunger = hunger;
  status->max_hunger = hunger;
  status->thirst = thirst;
  status->max_thirst = thirst;
  
}

void apply_status(const Status a, Status* b) {
  int temp_d = a.temp - b->temp;
  b->wet += a.wet;
  b->bleeding += a.bleeding;
  b->temp += temp_d > 0 ? 1 : temp_d < 0 ? -1 : 0;
};

int tick_status(Status* status) {
  int damage_taken = 0;

  // Infection if bleeding and wet
  if(status->wet && status->bleeding && randomi(1000) > 990) {
    status->infected += 1;
  }

  // Hypothermia if cold
  if(status->temp < 0) {
    if(randomi(1000) > 1000 + status->temp * (status->wet > 0 ? 2 : 1)) {
      status->hypothermia += 1;
    }
  } // Hypothermia gets better if not cold anymore
  else {
    if(randomi(2) > 0) {
      status->hypothermia -= status->hypothermia > 0 ? 1 : 0;
    }
  }

  // Hypothermia inflicted damage
  if(status->hypothermia > 0) {
    if(randomi(1000) > 750) {
      status->hp--;
      damage_taken++;
    }
  }

  // Bleed damage / bleed gets better
  if(status->bleeding) {
    if(randomi(1000) > 900) {
      status->bleeding -= status->bleeding > 0 ? 1 : 0;
    }
    if(randomi(1000) > 990) {
      status->hp--;
      damage_taken++;
    }
  }

  // Infection hurts, needs to be treated to be healed
  if(status->infected) {
    if(randomi(1000) > 950) {
      status->hp--;
      damage_taken++;
    }
  }

  status->hunger--;
  if(status->temp > 30) {
    if(randomi(1000) > 900) {
      status->hunger++;
    }
  }

  if(status->temp > 30) {
    status->thirst--;
  }
  status->thirst--;

  // So anything doesn't go below 0
  if(status->hp < 0) status->hp = 0;
  if(status->hunger < 0) status->hunger = 0;
  if(status->thirst < 0) status->thirst = 0;

  return damage_taken;
};

