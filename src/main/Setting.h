#ifndef __SETTING_INCLUDED__
#define __SETTING_INCLUDED__

#include "Item.h"
#include "Control.h"

class Setting : public Item{
  protected:
  
  Control *controller = NULL;
  Event event = NULL;

  public:

  Setting(char *title, Control *control, Event event);

  Item *enter();
  
};

#endif
