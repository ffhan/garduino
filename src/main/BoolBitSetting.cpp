#include "BoolBitSetting.h"
#include "Control.h"

BoolBitSetting::BoolBitSetting(char *title, Control *control, Action *event, GetBit getBit) : BitSetting<bool>(title, control, event, getBit) {
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

