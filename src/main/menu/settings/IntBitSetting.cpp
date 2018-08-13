#include "IntBitSetting.h"

#include "../Action.h"
#include "../../garduino/Control.h"

IntBitSetting::IntBitSetting(char *title, Control *control, Action *event, GetBit getBit, int range) : BitSetting<int>(title, control, event, getBit) {
	this->range = range;
	event->bindToValue(&tempValue);
	type = INTBITSETTING;
}

Item *IntBitSetting::enter() {
	return BitSetting<int>::enter();
}

void IntBitSetting::left() {
	if (tempValue - 1 >= 0) tempValue--;
}
void IntBitSetting::right() {
	if (tempValue + 1 <= range) tempValue++;
}
