#ifndef _ENTITY_H_
#define _ENTITY_H_

struct Map;
struct DialogSystem;
struct Status;
struct Inventory;

struct MapInteractionData {
  struct Map *map;
  struct DialogSystem *dialog;
  struct Status *status;
  struct Inventory *inventory;
};

extern int
interact_with_entity(struct MapInteractionData*, int, int);

#endif
