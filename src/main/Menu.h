#ifndef __MENU_INCLUDED__
#define __MENU_INCLUDED__

#include "ItemList.h"

class Item;

class Menu : public Item{

  private :

  //contains all items that are shown when this menu is clicked. 
  ItemList childItems = ItemList(this); 

  byte selector = 0;
  
  public: 
  Menu(char *title);
  
  void addItem(Item *item);
  void addItems();
  
  template <typename Item, typename ... Items>
  void addItems(Item item, Items ... rest){
    addItem((Item) item);
    addItems(rest...);
  }
  
  Item *enter();
  
  void processChildItem(int index, Printer func);
  void processChildItems(Printer func);
  
  Item *getSubMenuItem(int index);

  byte *getSelector();
  
  //not really needed since I exposed selector directly, but it's less verbose.
  byte getSelectorValue();
};

#endif
