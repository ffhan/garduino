#ifndef __INTSETTING_INCLUDED__
#define __INTSETTING_INCLUDED__

#include "ValueSetting.h"

class IntSetting : public ValueSetting<int>{
  public:

  IntSetting(char *title, int *bindValue, Control *control, Event event);
  
  void left();
  void right();
  
};

#endif
