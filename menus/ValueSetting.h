#ifndef __VALUESETTING_INCLUDED__
#define __VALUESETTING_INCLUDED__

#include "Setting.h"

template <class T>
class ValueSetting : public Setting{

  private:

  T *value;

  public : 

  T tempValue;

  ValueSetting(char *title, T *bindValue, Dummy *control, Event event);

  T getValue();
  T getTempValue();

  bool isSaved();

  virtual void left() = 0;
  virtual void right() = 0;

  Item *enter();
  
};


#endif
