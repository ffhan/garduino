#include "IntBitSetting.h"

IntBitSetting::IntBitSetting(char *title, Control *control, Event event, GetBit getBit, SetBit setBit, int range) : BitSetting<int>(title, control, event, getBit, setBit){
  this->range = range;
  type = INTBITSETTING;
}


void IntBitSetting::left(){
  if(tempValue - 1 >= 0) tempValue--;
}
void IntBitSetting::right(){
  if(tempValue + 1 <= range) tempValue++;
}
