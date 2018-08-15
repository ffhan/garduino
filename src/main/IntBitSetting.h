#ifndef __INTBITSETTING_INCLUDED__
#define __INTBITSETTING_INCLUDED__

#include "BitSetting.h"

class Action;
class Control;

class IntBitSetting : public BitSetting<int> {

  private:

    int range;

  public:

    IntBitSetting(char *title, Control *control, Action *event, GetBit getBit, int range);

    void left();
    void right();

	Item *enter();

};

#endif
