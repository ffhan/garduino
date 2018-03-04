#include "BoolSetting.h"

BoolSetting::BoolSetting(char *title, bool *bindValue, Dummy *control, Event event) : ValueSetting<bool>(title, bindValue, control, event){
	type = BOOLSETTING;
}

void BoolSetting::left(){
	tempValue = !tempValue;
}
void BoolSetting::right(){
	left();
}
