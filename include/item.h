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

#define EQUIP_SLOT_HEAD 1
#define EQUIP_SLOT_BODY 2
#define EQUIP_SLOT_LEGS 3

typedef struct {
  int id;
  int type;
  int price;
  char* name; // Add this later
  char* desc; // Add this later
  int metadata;
} Item;

extern int is_item_equipable(int);
extern int item_equip_slot(int);

extern int use_item_for_status(Item*, Status*);

typedef struct {
  Item* items;
  int n_items;
} ItemList;

extern int load_items(ItemList*, const char*);
extern int free_items(ItemList*);

extern int get_item_by_name(ItemList*, const char*);

typedef struct {
  int* items; // Contains the amount of items by id
  int n_items; // Item count
  int equipped_items[3];
} Inventory;

extern int create_inventory(Inventory*, int);
extern int free_inventory(Inventory*);
extern int inventory_has_item(Inventory*, int);
extern int inventory_add_items(Inventory*, int, int);
extern int inventory_take_items(Inventory*, int, int);
extern int inventory_transfer_to(Inventory*, Inventory*);
extern int inventory_unique_item_count(Inventory*);
extern int inventory_unique_nth_count(Inventory*, int);

extern int inventory_get_next_item(Inventory*, int);
extern int inventory_get_previous_item(Inventory*, int);

#endif
