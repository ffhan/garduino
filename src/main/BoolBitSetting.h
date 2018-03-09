#ifndef __BOOLBITSETTING_INCLUDED__
#define __BOOLBITSETTING_INCLUDED__

#include "BitSetting.h"

class Control;

class BoolBitSetting : public BitSetting<bool> {

  public:

    BoolBitSetting(char *title, Control *control, Action *event, GetBit getBit);

    void left();
    void right();

    Item *enter();

};

#endif
