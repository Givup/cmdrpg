#include "item.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Private functions

int
get_item_type_from_str(const char* type_str)
{
  // Maybe make a 'pair' of str <-> int, for easier checking
  if(strcmp("weapon", type_str) == 0)
  {
    return ITEM_TYPE_WEAPON;
  }
  if(strcmp("armor_head", type_str) == 0)
  {
    return ITEM_TYPE_ARMOR_HEAD;
  }
  if(strcmp("armor_body", type_str) == 0)
  {
    return ITEM_TYPE_ARMOR_BODY;
  }
  if(strcmp("armor_legs", type_str) == 0)
  {
    return ITEM_TYPE_ARMOR_LEGS;
  }
  if(strcmp("food", type_str) == 0)
  {
    return ITEM_TYPE_FOOD;
  }
  if(strcmp("heal", type_str) == 0)
  {
    return ITEM_TYPE_HEAL;
  }
  if(strcmp("drink", type_str) == 0)
  {
    return ITEM_TYPE_DRINK;
  }
  if(strcmp("misc", type_str) == 0)
  {
    return ITEM_TYPE_MISC;
  }
  return ITEM_TYPE_UNDEF;
};

void
strreplace(char* str, char find, char replacor)
{
  while(*str != '\0') // While string hasn't ended
  { 
    if(*str == find)
    { // If current character is what we are replacing
      *str = replacor; // Replace it
    }
    str++; // Advance string
  }
};

/*
  ITEM
 */

// TODO
// Should this be filled in runtime, instead of hardcoding it?
int
is_item_equipable(int item_type)
{
  switch(item_type) {
    case ITEM_TYPE_WEAPON:
    case ITEM_TYPE_ARMOR_HEAD:
    case ITEM_TYPE_ARMOR_BODY:
    case ITEM_TYPE_ARMOR_LEGS: return 1;
    default: return 0;
  }
};

// TODO
// Should this be filled in runtime, instead of hardcoding it?
int
item_equip_slot(int item_type)
{
  switch(item_type) {
    case ITEM_TYPE_ARMOR_HEAD: return EQUIP_SLOT_HEAD;
    case ITEM_TYPE_ARMOR_BODY: return EQUIP_SLOT_BODY;
    case ITEM_TYPE_ARMOR_LEGS: return EQUIP_SLOT_LEGS;
    default: return 0;
  }
};


/*
  ITEM LIST
 */

int
load_items(struct ItemList* list, const char* path)
{
  FILE* f = fopen(path, "r");
  if(f == NULL)
  {
    return 1;
  }

  int item_count = 0;
  if(!fscanf(f, "%d", &item_count))
  {
    printf("Failed to read item count.\n");
    return 1;
  }

  printf("There are %d items.\n", item_count);

  list->n_items = item_count;
  list->items = (struct Item*)malloc(sizeof(struct Item) * item_count);

  char buffer[128]; // String buffer

  // Instead of reading the amount of items from the file itself, maybe there
  //   should be some dynamic way of doing it?
  for(int i = 0;i < item_count;i++)
  { 
    // Read item data and create item out of it
    struct Item item;
    item.id = i;

    //printf("\nstruct Item [%d]:\n", i);

    for(int j = 0; j < 7; j++)
    {
      fscanf(f, "%s", buffer);

      // Skip lines that start with #
      if(buffer[0] == '#')
      {
	fscanf(f, "%[^\n]", buffer);
	/*
	  while(fread(buffer, 1, 1, f))
	  {
	  if(buffer[0] == '\n')
	  {
	  break;
	  }
	  }
	*/
	j--;
	continue;
      }

      if(strcmp(buffer, "type") == 0)
      { // Type defined
	fscanf(f, "%s", buffer);
	item.type = get_item_type_from_str(buffer);
	//printf("\tType: %d\n", item.type);
      }
      else if(strcmp(buffer, "name") == 0)
      {
	fscanf(f, "%s", buffer);
	int len = strlen(buffer);
	if(len <= 0) return 1;
	item.name = (char*)malloc(len + 1);
	strreplace(buffer, '_', ' ');
	strcpy(item.name, buffer);
	//printf("\tName: %s\n", item.name);
      }
      else if(strcmp(buffer, "desc") == 0)
      {
	fscanf(f, "%s", buffer);
	int len = strlen(buffer);
	if(len <= 0) return 1;
	item.desc = (char*)malloc(len + 1);
	strreplace(buffer, '_', ' ');
	strcpy(item.desc, buffer);
	//printf("\tDescription: %s\n", item.desc);
      }
      else if(strcmp(buffer, "price") == 0)
      {
	int price = 0;
	if(fscanf(f, "%u", &price) == 0) {
	  printf("Invalid price input! '%s'\n", buffer);
	  return 1;
	}
	item.price = price;
	//printf("\tPrice: %d\n", item.price);
      }
      else if(strcmp(buffer, "metadata") == 0)
      {
	int metadata = 0;
	if(fscanf(f, "%d", &metadata) == 0) {
	  printf("Invalid metadata input! '%s'\n", buffer);
	  return 1;
	}
	item.metadata = metadata;
	//printf("\tMetadata: %d\n", item.metadata);
      }
      else if(strcmp(buffer, "weight") == 0)
      {
	int weight = 0;
	if(fscanf(f, "%d", &weight) == 0) {
	  printf("Invalid metadata input! '%s'\n", buffer);
	  return 1;
	}
	item.weight = weight;
	//printf("\tWeight: %d\n", item.weight);
      } 
      else if(strcmp(buffer, "short_name") == 0)
      {
	fscanf(f, "%s", buffer);
	int len = strlen(buffer);
	if(len <= 0) return 1;
	item.short_name = (char*)malloc(len + 1);
	strreplace(buffer, '_', ' ');
	strcpy(item.short_name, buffer);
	//printf("\tShort name: %s\n", item.short_name);
      }
      else
      {
	printf("Invalid item parameter: '%s\n", buffer);
	return 1;
      }
    }
    list->items[i] = item;
  }
  return 0;
};

int
free_items(struct ItemList* list)
{
  for(int i = 0; i < list->n_items;i++)
  {
    free(list->items[i].name);
    free(list->items[i].desc);
    free(list->items[i].short_name);
  }
  if(list->n_items > 0) free(list->items);
  list->n_items = 0;
};

int
get_item_by_name(struct ItemList* list, const char* name) 
{
  for(int i = 0;i < list->n_items;i++) 
  {
    if(strcmp(list->items[i].name, name) == 0) 
    {
      return list->items[i].id;
    }
  }
  return -1;
};

/*
  INVENTORY
 */

int
create_inventory(struct Inventory* inventory, struct ItemList* list)
{
  inventory->items = (int*)malloc(sizeof(int) * list->n_items);
  inventory->list = list;
  if(inventory->items == NULL)
  {
    return 1;
  }
  memset(inventory->items, 0, sizeof(int) * list->n_items);

  // Clear equipped items
  for(int e = 0; e < sizeof(inventory->equipped_items) / sizeof(int); e++)
  {
    inventory->equipped_items[e] = -1;
  }
  return 0;
};

int
free_inventory(struct Inventory* inventory)
{
  if(inventory->list->n_items > 0) free(inventory->items);
  return 0;
}

int
inventory_has_item(struct Inventory* inventory, int item_id)
{
  if(item_id < 0) return -1; // Invalid item_id
  return inventory->items[item_id]; // Return amount of items
};

int
inventory_add_items(struct Inventory* inventory, int item_id, int amount)
{
  if(item_id < 0 || amount <= 0) return -1; // Invalid item_id or invalid amount
  inventory->items[item_id] += amount; // Add items
  return 0;
};

int
inventory_take_items(struct Inventory* inventory, int item_id, int amount)
{
  if(item_id < 0) return -1; // Invalid item_id
  if(inventory->items[item_id] < amount) // If there aren't enough items
  { 
    printf("There wasn't enough items.\n");
    return -1;
  }
  else // Remove items
  { 
    inventory->items[item_id] -= amount;
  }
  return inventory->items[item_id]; // Return the amount of items left
};

int
inventory_transfer_to(struct Inventory* from, struct Inventory* to)
{
  for(int i = 0;i < from->list->n_items; i++)
  {
    to->items[i] += from->items[i]; // Add items
    from->items[i] = 0; // Clean 'from' inventory of items
  }
  return 0;
};

int
inventory_unique_item_count(struct Inventory* inventory)
{
  int uniques = 0;
  for(int i= 0; i < inventory->list->n_items; i++)
  {
    if(inventory->items[i] > 0)
    {
      uniques++;
    }
  }
  return uniques;
};

int
inventory_unique_nth_count(struct Inventory* inventory, int item_id)
{
  int n = 0;
  for(int i = 0;i < inventory->list->n_items;i++)
  {
    if(i == item_id) break;
    if(inventory->items[i] > 0)
    {
      n++;
    }
  }
  return n;
};

int
inventory_get_next_item(struct Inventory* inventory, int current_id)
{
  if(inventory_unique_item_count(inventory) == 0) return -1; // If there aren't any items in the inventory
  int i = current_id;
  while(1)
  {
    i++;
    if(i >= inventory->list->n_items) i = 0;
    if(inventory->items[i] > 0) return i;
  }
};

int
inventory_get_previous_item(struct Inventory* inventory, int current_id)
{
  if(inventory_unique_item_count(inventory) == 0) return -1;
  int i = current_id;
  while(1)
  {
    i--;
    if(i < 0) i = inventory->list->n_items - 1;
    if(inventory->items[i] > 0) return i;
  }
};

int
inventory_get_weight(struct Inventory* inventory, struct ItemList* item_list)
{
  int w = 0;
  for(int i = 0;i < item_list->n_items;i++)
  {
    w += item_list->items[i].weight * inventory->items[i];
  }
  return w;
};

int
use_item_for_status(struct Item* item, struct Status* status)
{
  switch(item->type) {
    case ITEM_TYPE_FOOD:
      status->hunger = min(status->hunger + item->metadata, status->max_hunger);
      break;
    case ITEM_TYPE_DRINK:
      status->thirst = min(status->thirst + item->metadata, status->max_thirst);
      break;
    case ITEM_TYPE_HEAL:
      status->hp = min(status->hp + item->metadata, status->max_hp);
      if(status->bleeding)
      {
	status->bleeding = 0;
      }
      if(status->infected)
      {
	status->infected = 0;
      }
      break;
    default: return 0;
  };
  return 1;
};

int
use_item_for_equipment(struct Item* item, struct Inventory* inventory)
{
  int slot = -1;
  
  switch(item->type) {
    case ITEM_TYPE_WEAPON:
      slot = EQUIP_SLOT_WEAPON;
      break;
    case ITEM_TYPE_ARMOR_HEAD:
      slot = EQUIP_SLOT_HEAD;
      break;
    case ITEM_TYPE_ARMOR_BODY:
      slot = EQUIP_SLOT_BODY;
      break;
    case ITEM_TYPE_ARMOR_LEGS:
      slot = EQUIP_SLOT_LEGS;
      break;
    default: return 0;
  }

  if(slot != -1)
  {
    if(inventory->equipped_items[slot] == item->id)
    {
      inventory->equipped_items[slot] = -1;
    }
    else
    {
      inventory->equipped_items[slot] = item->id;
    }
  }
  return 1;
};
