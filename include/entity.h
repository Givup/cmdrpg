#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "map.h"
#include "item.h"

extern int interact_with_entity(Map*, int, int, Inventory*, ItemList*, Status*);

#endif
