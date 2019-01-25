#ifndef _ITEM_H_
#define _ITEM_H_

// Should these be replaced by enum???
#define ITEM_TYPE_UNDEF      0
#define ITEM_TYPE_MISC       1

#define ITEM_TYPE_ARMOR_HEAD 2
#define ITEM_TYPE_ARMOR_BODY 3
#define ITEM_TYPE_ARMOR_LEGS 4

#define ITEM_TYPE_CONSUMABLE 5
#define ITEM_TYPE_FOOD       6
#define ITEM_TYPE_HEAL       7

#define ITEM_TYPE_WEAPON     8
#define ITEM_TYPE_WEAPON_OFF 9

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
} Inventory;

extern int create_inventory(Inventory*, int);
extern int inventory_has_item(Inventory*, int);
extern int inventory_add_items(Inventory*, int, int);
extern int inventory_take_items(Inventory*, int, int);
extern int inventory_transfer_to(Inventory*, Inventory*);

#endif
