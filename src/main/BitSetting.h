#ifndef __BITSETTING_INCLUDED__
#define __BITSETTING_INCLUDED__

#include "Setting.h"
#include "Control.h"

template <class T>
class BitSetting : public Setting{

  protected:

  GetBit getBit;
  int code;

  public : 

  T tempValue;

  BitSetting(char *title, Control *control, Event event, GetBit getBit, int code);

  T getValue();
  T getTempValue();

  bool isSaved();

  virtual void left() = 0;
  virtual void right() = 0;

  Item *enter();
  
};


#endif
