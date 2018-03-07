#ifndef __INTBITSETTING_INCLUDED__
#define __INTBITSETTING_INCLUDED__

#include "BitSetting.h"

class IntBitSetting : public BitSetting<int> {

  private:

    int range;

  public:

    IntBitSetting(char *title, Control *control, Event event, GetBit getBit, int code, int range);

    void left();
    void right();

};

#endif
