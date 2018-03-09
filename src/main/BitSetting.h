#ifndef __BITSETTING_INCLUDED__
#define __BITSETTING_INCLUDED__

#include "Setting.h"
#include "Control.h"

template <class T>
class BitSetting : public Setting{

  protected:

  GetBit getBit;

  public : 

  T tempValue;

  BitSetting(char *title, Control *control, Action *event, GetBit getBit);

  T getValue();
  T getTempValue();

  bool isSaved();

  virtual void left() = 0;
  virtual void right() = 0;

  Item *enter();
  
};


#endif
