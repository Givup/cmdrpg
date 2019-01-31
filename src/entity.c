#include "entity.h"

#include "core.h"
#include "equipment.h"

int interact_with_entity(Map* map, int x, int y,
			 Inventory* inventory, ItemList* item_list,
			 Status* status) 
{
  MapEntity* entity = get_entity(map, x, y);
  if(entity == NULL) return -1;

  switch(entity->tile) {
  case ENTITY_CACTUS:
    {
      int w_meta = 0;
      if(inventory->equipped_items[EQUIP_SLOT_WEAPON] != -1) {
	w_meta = item_list->items[inventory->equipped_items[EQUIP_SLOT_WEAPON]].metadata;
      } else {
	status->hp--;
      }

      if((w_meta & DAMAGE_TYPE_SLASH) || (w_meta & DAMAGE_TYPE_THRUST)) {
	// TODO: Add item to player inventory
	inventory_add_items(inventory, get_item_by_name(item_list, "Cactus Fruit"), randomi_range(1, 2));
      }
      set_entity(map, x, y, TILE_UNDEF, 0);
      return 1;
    }

  case ENTITY_HOUSE:
    {
      int w_meta = 0;
      if(inventory->equipped_items[EQUIP_SLOT_WEAPON] != -1) {
	w_meta = item_list->items[inventory->equipped_items[EQUIP_SLOT_WEAPON]].metadata;
      }

      if(w_meta & DAMAGE_TYPE_BLUNT) {
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
      if(inventory->equipped_items[EQUIP_SLOT_WEAPON] != -1) {
	w_meta = item_list->items[inventory->equipped_items[EQUIP_SLOT_WEAPON]].metadata;
      }

      if(w_meta & DAMAGE_TYPE_BLUNT) {
	inventory_add_items(inventory, get_item_by_name(item_list, "Stick"), randomi_range(10, 25));
	set_entity(map, x, y, TILE_UNDEF, 0);
      }
      return 1;
    }

  default: return 0;
  };
};
