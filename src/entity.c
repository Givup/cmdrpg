#include "entity.h"

#include "item.h"
#include "map.h"
#include "core.h"
#include "equipment.h"
#include "dialog.h"

int
interact_with_entity(struct MapInteractionData *bundle, int x, int y)
{
  struct Map *map = bundle->map;
  struct MapEntity* entity = get_entity(map, x, y);
  if(entity == NULL) return -1;

  struct DialogSystem *dialog = bundle->dialog;
  struct Status* status = bundle->status;
  struct Inventory* inventory = bundle->inventory;
  struct ItemList* item_list = inventory->list;

  switch(entity->tile) {
  case ENTITY_CACTUS:
  {
    int w_meta = 0;
    if(inventory->equipped_items[EQUIP_SLOT_WEAPON] != -1)
    {
      w_meta = item_list->items[inventory->equipped_items[EQUIP_SLOT_WEAPON]].metadata;
    }
    else
    {
      status->hp--;
    }

    if((w_meta & DAMAGE_TYPE_SLASH) || (w_meta & DAMAGE_TYPE_THRUST))
    {
      inventory_add_items(inventory, get_item_by_name(item_list, "Cactus Fruit"), randomi_range(1, 2));
    }
    set_entity(map, x, y, TILE_UNDEF, 0);
    return 1;
  }

    case ENTITY_HOUSE:
    {
      int w_meta = 0;
      if(inventory->equipped_items[EQUIP_SLOT_WEAPON] != -1)
      {
	w_meta = item_list->items[inventory->equipped_items[EQUIP_SLOT_WEAPON]].metadata;
      }

      if(w_meta & DAMAGE_TYPE_BLUNT)
      {
	set_entity(map, x, y, TILE_UNDEF, 0);
      }
      return 1;
    };

    case ENTITY_SCRUB:
    {
      inventory_add_items(inventory, get_item_by_name(item_list, "Stick"), randomi_range(1, 3));
      set_entity(map, x, y, TILE_UNDEF, 0);
      return 1;
    };

    case ENTITY_DOOR:
    {
      int w_meta = 0;
      if(inventory->equipped_items[EQUIP_SLOT_WEAPON] != -1)
      {
	w_meta = item_list->items[inventory->equipped_items[EQUIP_SLOT_WEAPON]].metadata;
      }

      if(w_meta & DAMAGE_TYPE_BLUNT)
      {
	inventory_add_items(inventory, get_item_by_name(item_list, "Stick"), randomi_range(10, 25));
	set_entity(map, x, y, TILE_UNDEF, 0);
      }
      return 1;
    };

    case ENTITY_COW:
    {
      int w_meta = 0;
      if(inventory->equipped_items[EQUIP_SLOT_WEAPON] != -1)
      {
	w_meta = item_list->items[inventory->equipped_items[EQUIP_SLOT_WEAPON]].metadata;
      }

      if((w_meta & DAMAGE_TYPE_SLASH) || (w_meta & DAMAGE_TYPE_THRUST))
      {
	const char* item_name;
	if(w_meta & DAMAGE_TYPE_FIRE) 
	{
	  item_name = "Cooked Meat";
	}
	else
	{
	  item_name = "Raw Meat";
	}
	inventory_add_items(inventory, get_item_by_name(item_list, item_name), randomi_range(1, 3));
	set_entity(map, x, y, TILE_UNDEF, 0);
      }
      return 1;
    };

    case ENTITY_NPC:
    {
      open_dialog(dialog, inventory, 0, 0);
      return 1;
    };

    default: return 0;
  };
};
