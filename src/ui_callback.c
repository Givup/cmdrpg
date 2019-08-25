#include <stdio.h>

#include "equipment.h"
#include "item.h"
#include "status.h"
#include "ui.h"

// Array of strings print helper macro
#define CALLBACK_PRINT(buffer_a, y, format, ...) snprintf(buffer_a[y], panel->w, format, __VA_ARGS__)

// Callback function for the character sheet struct UIPanel
char**
status_screen_callback(void* system, void* p_panel)
{
  struct UIPanel* panel = (struct UIPanel*)p_panel;

  char** lines = malloc(sizeof(char*) * panel->h);
  for(int i = 0;i < panel->h;i++)
  {
    *(lines + i) = malloc(sizeof(char) * panel->w + 1);
    memset(*(lines + i), 0, panel->w);
  }

  void* data = panel->user_data;
  struct UICallbackData *cbdata = (struct UICallbackData*)data;

  struct Status *status = cbdata->status;
  struct Inventory *inventory = cbdata->inventory;
  struct ItemList *item_list = inventory->list;

  int line = 2;

  CALLBACK_PRINT(lines, 0, "STATUS", 0);

  CALLBACK_PRINT(lines, line++, "Health: %d / %d", status->hp, status->max_hp);
  CALLBACK_PRINT(lines, line++, "Hunger: %d / %d", status->hunger / 25, status->max_hunger / 25);
  CALLBACK_PRINT(lines, line++, "Thirst: %d / %d", status->thirst / 25, status->max_thirst / 25);

  int weight = inventory_get_weight(inventory, item_list);

  CALLBACK_PRINT(lines, line++, "Weight: %d.%d kg", weight / 1000, (weight % 1000) / 10);

  line++;

  int wpn = inventory->equipped_items[EQUIP_SLOT_WEAPON];
  int head = inventory->equipped_items[EQUIP_SLOT_HEAD];
  int body = inventory->equipped_items[EQUIP_SLOT_BODY];
  int legs = inventory->equipped_items[EQUIP_SLOT_LEGS];

  CALLBACK_PRINT(lines, line++, "WPN: %s", wpn != -1 ? item_list->items[wpn].name : "Fists");
  CALLBACK_PRINT(lines, line++, "HEAD: %s", head != -1 ? item_list->items[head].name : "None");
  CALLBACK_PRINT(lines, line++, "BODY: %s", body != -1 ? item_list->items[body].name : "None");
  CALLBACK_PRINT(lines, line++, "LEGS: %s", legs != -1 ? item_list->items[legs].name : "None");

  if(wpn != -1)
  {
    int w_meta = item_list->items[wpn].metadata;

    int dmg_type_physical = w_meta & 0x0F;
    int dmg_type_magical = w_meta & 0xF0;

    CALLBACK_PRINT(lines, 11, "DMG: %d", get_value_from_metadata(w_meta));
    CALLBACK_PRINT(lines, 12, "%s %s",
	       dmg_type_magical ? get_damage_type_str(dmg_type_magical) : "", 
	       dmg_type_physical ? get_damage_type_str(dmg_type_physical) :"");
  }
  else
  {
    CALLBACK_PRINT(lines, 11, "Offensive", 0);
    CALLBACK_PRINT(lines, 12, "DMG: 1", 0);
    CALLBACK_PRINT(lines, 13, "%s", get_damage_type_str(DAMAGE_TYPE_BLUNT));
  }

  {
    int h_meta = head != -1 ? item_list->items[head].metadata : 0;
    int b_meta = body != -1 ? item_list->items[body].metadata : 0;
    int l_meta = legs != -1 ? item_list->items[legs].metadata : 0;

    int slash_armor = 0;
    int thrust_armor = 0;
    int blunt_armor = 0;

    int shock_armor = 0;
    int fire_armor = 0;
    int ice_armor = 0;

    slash_armor += get_type_value_from_metadata(DAMAGE_TYPE_SLASH, h_meta);
    slash_armor += get_type_value_from_metadata(DAMAGE_TYPE_SLASH, b_meta);
    slash_armor += get_type_value_from_metadata(DAMAGE_TYPE_SLASH, l_meta);
	  
    thrust_armor += get_type_value_from_metadata(DAMAGE_TYPE_THRUST, h_meta);
    thrust_armor += get_type_value_from_metadata(DAMAGE_TYPE_THRUST, b_meta);
    thrust_armor += get_type_value_from_metadata(DAMAGE_TYPE_THRUST, l_meta);
	  
    blunt_armor += get_type_value_from_metadata(DAMAGE_TYPE_BLUNT, h_meta);
    blunt_armor += get_type_value_from_metadata(DAMAGE_TYPE_BLUNT, b_meta);
    blunt_armor += get_type_value_from_metadata(DAMAGE_TYPE_BLUNT, l_meta);

    shock_armor += get_type_value_from_metadata(DAMAGE_TYPE_SHOCK, h_meta);
    shock_armor += get_type_value_from_metadata(DAMAGE_TYPE_SHOCK, b_meta);
    shock_armor += get_type_value_from_metadata(DAMAGE_TYPE_SHOCK, l_meta);

    fire_armor += get_type_value_from_metadata(DAMAGE_TYPE_FIRE, h_meta);
    fire_armor += get_type_value_from_metadata(DAMAGE_TYPE_FIRE, b_meta);
    fire_armor += get_type_value_from_metadata(DAMAGE_TYPE_FIRE, l_meta);

    ice_armor += get_type_value_from_metadata(DAMAGE_TYPE_ICE, h_meta);
    ice_armor += get_type_value_from_metadata(DAMAGE_TYPE_ICE, b_meta);
    ice_armor += get_type_value_from_metadata(DAMAGE_TYPE_ICE, l_meta);

    CALLBACK_PRINT(lines, 15, "Defensive", 0);
    CALLBACK_PRINT(lines, 16, "Slash: %d", slash_armor);
    CALLBACK_PRINT(lines, 17, "Thrust: %d", thrust_armor);
    CALLBACK_PRINT(lines, 18, "Blunt: %d", blunt_armor);
    CALLBACK_PRINT(lines, 19, "Shock: %d", shock_armor);
    CALLBACK_PRINT(lines, 20, "Fire: %d", fire_armor);
    CALLBACK_PRINT(lines, 21, "Ice: %d", ice_armor);
  }

  return lines;
};
