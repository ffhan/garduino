#include "Setting.h"

#include "Action.h"

Setting::Setting(char *title, Control *control, Action *event) : Item(title) {
	controller = control;
	this->event = event;
	type = SETTING;
}

Item *Setting::enter() {
	Serial.println("entering");
	event->execute();
	Serial.println("exited.");
	return Item::enter();
}
