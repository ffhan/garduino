#include "BitSetting.h"

template <class T>
BitSetting<T>::BitSetting(char *title, Control *control, Event event, GetBit getBit, int code) : Setting(title, control, event){
  this->getBit = getBit;
  this->code = code;
  this->tempValue = (T) (control->*getBit)();
  type = BITSETTING;
}

template <class T>
T BitSetting<T>::getValue(){
  return (T) (controller->*getBit)();
}

template <class T>
T BitSetting<T>::getTempValue(){
  return tempValue;
}

template <class T>
bool BitSetting<T>::isSaved(){
  return getValue() == tempValue;
}

template <class T>
Item *BitSetting<T>::enter(){
  controller->mainSwitch(code);
  return Setting::enter();
}

template class BitSetting<bool>;
template class BitSetting<int>;
