#ifndef __SETTING_INCLUDED__
#define __SETTING_INCLUDED__

#include "Item.h"
#include "Control.h"

class Action;

class Setting : public Item{
  protected:
  
  Control *controller = NULL;
  Action *event = NULL;

  public:

  Setting(char *title, Control *control, Action *event);

  Item *enter();
  
};

#endif
