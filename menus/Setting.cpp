#include "Setting.h"

Setting::Setting(char *title, Dummy *control, Event event) : Item(title){
	controller = control;
	this->event = event;
	type = SETTING;
}

Item *Setting::enter(){
	(controller->*event)();
	Item::enter();
}
