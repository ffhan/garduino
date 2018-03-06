#ifndef __BOOLSETTING_INCLUDED__
#define __BOOLSETTING_INCLUDED__

#include "ValueSetting.h"

class Control;

class BoolSetting : public ValueSetting<bool>{

  public:
  
  BoolSetting(char *title, bool *bindValue, Control *control, Event event);
  
  void left();
  void right();
  
};

#endif
