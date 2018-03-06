#ifndef __BOOLBITSETTING_INCLUDED__
#define __BOOLBITSETTING_INCLUDED__

#include "BitSetting.h"

class Control;

class BoolBitSetting : public BitSetting<bool> {

  public:

    BoolBitSetting(char *title, Control *control, Event event, GetBit getBit, SetBit setBit);

    void left();
    void right();

};

#endif
