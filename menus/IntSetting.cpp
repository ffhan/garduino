#include "IntSetting.h"

IntSetting::IntSetting(char *title, int *bindValue, Dummy *control, Event event) : ValueSetting<int>(title, bindValue, control, event){
	type = INTSETTING;
}

void IntSetting::left(){
	if(tempValue - 1 >= 0) tempValue--;
}
void IntSetting::right(){
	if(tempValue + 1 <= 9) tempValue++;
}