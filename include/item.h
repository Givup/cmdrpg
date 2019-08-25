#ifndef _ITEM_H_
#define _ITEM_H_

#include "status.h"

// Should these be replaced by enum???
#define ITEM_TYPE_UNDEF      0
#define ITEM_TYPE_MISC       1

#define ITEM_TYPE_ARMOR_HEAD 10
#define ITEM_TYPE_ARMOR_BODY 11
#define ITEM_TYPE_ARMOR_LEGS 12

#define ITEM_TYPE_FOOD       20
#define ITEM_TYPE_DRINK      21
#define ITEM_TYPE_HEAL       22

#define ITEM_TYPE_WEAPON     30
#define ITEM_TYPE_WEAPON_OFF 31

#define EQUIP_SLOT_WEAPON 0
#define EQUIP_SLOT_HEAD   1
#define EQUIP_SLOT_BODY   2
#define EQUIP_SLOT_LEGS   3

struct Item {
  int id;
  int type;
  int price;
  int weight;
  int metadata;
  char* name;
  char* short_name;
  char* desc;
};

extern int
is_item_equipable(int);

extern int
item_equip_slot(int);

struct ItemList {
  struct Item* items;
  int n_items;
};

extern int
load_items(struct ItemList*, const char*);

extern int
free_items(struct ItemList*);

extern int
get_item_by_name(struct ItemList*, const char*);

struct Inventory {
  int* items; // Contains the amount of items by id
  struct ItemList* list;
  int equipped_items[4]; // Weapon, helm, torso, legs
};

extern int
create_inventory(struct Inventory*, struct ItemList*);

extern int
free_inventory(struct Inventory*);

extern int
inventory_has_item(struct Inventory*, int);

extern int
inventory_add_items(struct Inventory*, int, int);

extern int
inventory_take_items(struct Inventory*, int, int);

extern int
inventory_transfer_to(struct Inventory*, struct Inventory*);

extern int
inventory_unique_item_count(struct Inventory*);

extern int
inventory_unique_nth_count(struct Inventory*, int);


extern int
inventory_get_next_item(struct Inventory*, int);

extern int
inventory_get_previous_item(struct Inventory*, int);

extern int
inventory_get_weight(struct Inventory*, struct ItemList*);

// struct Item usage
extern int
use_item_for_status(struct Item*, struct Status*);

extern int
use_item_for_equipment(struct Item*, struct Inventory*);

#endif
