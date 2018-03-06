#ifndef __BITSETTING_INCLUDED__
#define __BITSETTING_INCLUDED__

#include "Setting.h"
#include "Control.h"

template <class T>
class BitSetting : public Setting{

  protected:

  GetBit getBit;
  SetBit setBit;

  public : 

  T tempValue;

  BitSetting(char *title, Control *control, Event event, GetBit getBit, SetBit setBit);

  T getValue();
  T getTempValue();

  bool isSaved();

  virtual void left() = 0;
  virtual void right() = 0;

  Item *enter();
  
};


#endif
