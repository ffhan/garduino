#ifndef __ITEM_INCLUDED__
#define __ITEM_INCLUDED__

#include "Arduino.h"

typedef enum {
    ITEM = 0, SETTING = 1, BOOLSETTING = 2, INTSETTING = 3, BITSETTING = 4, INTBITSETTING = 5, BOOLBITSETTING = 6, MENU = 7
} itemType;

//typedef void (*Printer) (Item *item);

class Item{

  protected : 
  itemType type;
  
  private :

  byte index;
  char *title;

  Item *parent;
  bool hasParent = false;
  
  Item *before;
  bool top = false;
  
  Item *after;
  bool bottom = false;
  
  public : 
  
  Item(char *title);
  char *getTitle();
  itemType getType();
  
  void setBefore(Item *before);
  Item *getBefore();
  
  void setAfter(Item *after);
  Item *getAfter();
  
  void setParent(Item *parent);
  Item *getParent();
  
  Item *up();
  Item *down();
  Item *back();
  virtual Item *enter();
  
  virtual void left();
  virtual void right();

  bool isMenu();
  bool isSetting();

  char *getStringType();
  
  public : 
  
  byte getIndex();
  byte setIndex(byte index);
};

#endif
