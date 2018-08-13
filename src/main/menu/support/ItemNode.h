#ifndef __ITEMNODE__INCLUDED__
#define __ITEMNODE__INCLUDED__

#include "Arduino.h"
#include "../Item.h"

class ItemNode{
  public : 
  ItemNode *previous = NULL;
  ItemNode *next = NULL;
  Item *item = NULL;
};

#endif
