#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "status.h"
#include "item.h"
#include "dialog.h"

typedef struct {
  int x, y;
  Status status;
  Inventory inventory;
  DialogSystem dialog;
} Player;

#endif
