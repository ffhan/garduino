#include "Menu.h"

Menu::Menu(char *title) : Item(title){
    type = MENU;
}

void Menu::addItem(Item *item){
	childItems.add(item);
}

void Menu::addItems(){}

Item *Menu::enter(){
	Item *temp = childItems.get(0);
	if(temp) return temp;
	Item::enter();
}
void Menu::processChildItem(int index, Printer func){
	childItems.process(index, func);
}
void Menu::processChildItems(Printer func){
	childItems.processAll(func);
}
Item *Menu::getSubMenuItem(int index){
	return childItems.get(index);
}

byte *Menu::getSelector(){
	return &selector;
}
//not really needed since I exposed selector directly, but it's less verbose.
byte Menu::getSelectorValue(){ 
	return selector;
}
