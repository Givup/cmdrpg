#include "equipment.h"

const char*
get_damage_type_str(int type)
{
  switch(type) {
    case DAMAGE_TYPE_SLASH: return "Slash";
    case DAMAGE_TYPE_THRUST: return "Thrust";
    case DAMAGE_TYPE_BLUNT: return "Blunt";

    case DAMAGE_TYPE_SHOCK: return "Shock";
    case DAMAGE_TYPE_FIRE: return "Fire";
    case DAMAGE_TYPE_ICE: return "Ice";

    default: return "Invalid-DMG-Type";
  };
};

int
get_damage_type_from_metadata(int metadata)
{
  return metadata & 0xFF;
};

int
get_value_from_metadata(int metadata)
{
  return (metadata & 0xFF00) >> 8;
};

int 
get_type_value_from_metadata(int type, int meta)
{
  if((meta & type) > 0)
  {
    return get_value_from_metadata(meta);
  }
  return 0;
};
