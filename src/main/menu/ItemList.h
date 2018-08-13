#ifndef __ITEMLIST_INCLUDED__
#define __ITEMLIST_INCLUDED__

#include "support/ItemNode.h"

class Item;

typedef void (*Printer) (Item *item);

/*
 * a better name would be SubMenu, but I didn't want to draw a too close relationship to
 * Menu, because this holds all Item objects and does not behave like a menu; it's a helper
 * class.
 */
class ItemList{
  public:
  int len = 0;
  ItemNode *head = NULL;
  ItemNode *tail = NULL;

  Item *parent;

  ItemList(Item *parent);

  void add(Item *item);
  
  Item *get(int index);
  
  void process(int index, Printer func);
  void processAll(Printer func);
  
};

#endif
