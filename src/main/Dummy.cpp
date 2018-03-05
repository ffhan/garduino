#include "Dummy.h"

Dummy::Dummy(bool t, bool t1, int value){
	test = t;
	test2 = t1;
	val = value;
}

void Dummy::printStates(){
	Serial.print("Bool 1 "); if(test) Serial.println("true"); else Serial.println("false");
	Serial.print("Bool 2 "); if(test2) Serial.println("true"); else Serial.println("false");
	Serial.print("Integer 1 "); Serial.println(val);
}
void Dummy::printEnter(){
	Serial.println("HEY YOU PRESSED ME");
}