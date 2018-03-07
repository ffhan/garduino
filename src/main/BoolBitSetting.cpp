#include "BoolBitSetting.h"
#include "Control.h"

BoolBitSetting::BoolBitSetting(char *title, Control *control, Event event, GetBit getBit, int code) : BitSetting<bool>(title, control, event, getBit, code) {
  type = BOOLBITSETTING;
}

void BoolBitSetting::left() {
  //tempValue = !tempValue;
}
void BoolBitSetting::right() {
  //left();
}
Item *BoolBitSetting::enter(){
  BitSetting::enter();
  tempValue = getValue();
}

