#include "BoolBitSetting.h"
#include "Control.h"

BoolBitSetting::BoolBitSetting(char *title, Control *control, Event event, GetBit getBit, SetBit setBit) : BitSetting<bool>(title, control, event, getBit, setBit) {
  type = BOOLBITSETTING;
}

void BoolBitSetting::left() {
  tempValue = !tempValue;
}
void BoolBitSetting::right() {
  left();
}

