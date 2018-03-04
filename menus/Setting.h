#ifndef __SETTING_INCLUDED__
#define __SETTING_INCLUDED__

#include "Item.h"
#include "Dummy.h"

class Setting : public Item{
  protected:
  
  Dummy *controller = NULL;
  Event event = NULL;

  public:

  Setting(char *title, Dummy *control, Event event);

  Item *enter();
  
};

#endif
