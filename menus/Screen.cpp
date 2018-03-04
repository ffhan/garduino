#include "Screen.h"

Screen::Screen(byte cols, byte rows, Menu *mainMenu){
	this->rows = rows;
	this->cols = cols;
	bindToMenu(mainMenu);
}

//use only for constructor.
void Screen::bindToMenu(Menu *menu){
	this->mainMenu = menu;
	cursorPosition = menu->getSelector();
	currentItem = mainMenu->enter();
}

void Screen::up(){
	if(currentItem == currentItem->up()) return;
	index--;
	if(*cursorPosition - 1 >= 0) (*cursorPosition)--;
	currentItem = currentItem->up();
}
void Screen::down(){
	if(!currentItem->down()){
	  return;
	}
	index++;
	if(*cursorPosition + 1 < rows) (*cursorPosition)++;
	currentItem = currentItem->down();
}
void Screen::back(){
	if(!currentItem->back()) return;
	Menu *menu = mainMenu->back();
	//Serial.print(mainMenu->getTitle());Serial.print(" ");
	if(mainMenu != menu && menu){
	  mainMenu = menu;
	  cursorPosition = mainMenu->getSelector();
	  currentItem = currentItem->back();
	  index = currentItem->getIndex();
	}

	//Serial.println(mainMenu->getTitle());
}
void Screen::enter(){
	if(currentItem->isSetting()) {
	  currentItem->enter();
	}
	if(currentItem->getType() != MENU) return;
	if(!currentItem->enter()) return;

	mainMenu = (Menu *) currentItem;
	cursorPosition = mainMenu->getSelector();
	*cursorPosition = 0; // reset the cursor position, because we do the same for the index.
	currentItem = mainMenu->enter();
	index = currentItem->getIndex();
}

void Screen::left(){
	currentItem->left();
}
void Screen::right(){
	currentItem->right();
}
void Screen::flash(Printer func){
	Serial.print(*cursorPosition); Serial.print(space); Serial.println(index);
	func(currentItem);
}
void Screen::show(){
	Serial.print(*cursorPosition); Serial.print(space); Serial.print(index);
	Serial.print(space);Serial.println(mainMenu->getTitle());
	/*
	 * Screen is set up this way:
	 * Each row has 3 parts:
	 * _SLCT_|_________Title__________|_marker_
	 *    >  |     Level setting      |  .xX
	 *       |   Numerical setting    |   6
	 *       |  Boolean false setting |   x
	 *       |  Boolean true setting  |   o
	 *       |     Unsaved setting    |  _o_
	 *       |    Simple separator    |
	 *       |       Menu example     |   +
	 *       
	 * SLCT is a part that determines if the current Item is selected. It takes up 1 column.
	 * Marker is a part that gives additional info on the type of an Item and its' 
	 * state (if it's a Setting). It holds 3 columns.
	 * Title defines what a specific Item represents. It holds (by default) 12 columns.
	 * I'll maybe enable restructuring user interface (column size, etc.) in later stages.
	 */
	Item *item;
	byte processed = 0;
	byte titleCols = cols - 4;
	Serial.println(F("----------------"));
	for(byte i = index - *cursorPosition; i < index - *cursorPosition + rows; i++){
	  
	  item = mainMenu->getSubMenuItem(i);
	  
	  if(item){
		if(processed == *cursorPosition){
		  Serial.print(slct);
		}
		else{
		  Serial.print(space);
		}
		processed++;
		if(strlen(item->getTitle()) >= titleCols){
		  for(byte j = 0; j < titleCols; j++){
			Serial.print(item->getTitle()[j]);
		  }
		}
		else{
		  for(byte j = 0; j < (titleCols - strlen(item->getTitle())) / 2; j++){
			Serial.print(space);
		  }
		  Serial.print(item->getTitle());
		  for(byte j = 0; j < (titleCols - strlen(item->getTitle())) / 2 + 
		  (titleCols - strlen(item->getTitle())) % 2; j++){
			Serial.print(space);
		  }
		}
		if(!(item->isSetting())) Serial.print(space);
		switch(item->getType()){
		  case MENU:
		  Serial.print(menuSign);
		  break;
		  case SETTING:
		  for(int j = 0; j < 3; j++){
			Serial.print(space);
		  }
		  break;
		  case BOOLSETTING:{
		   BoolSetting *setting = (BoolSetting *) item;
			if(setting->isSaved()) Serial.print(space);
			else Serial.print(unsaved);
			
			if(setting->getTempValue()) Serial.print(trueSign);
			else Serial.print(falseSign);

			if(setting->isSaved()) Serial.print(space);
			else Serial.print(unsaved); 
		  }
		  break;
		  
		  case INTSETTING:{
			IntSetting *intSet = (IntSetting *) item;
			
			if(intSet->isSaved()) Serial.print(space);
			else Serial.print(unsaved);
			
			Serial.print(intSet->getTempValue());

			if(intSet->isSaved()) Serial.print(space);
			else Serial.print(unsaved);
		  }
		  break;
		  
		  case ITEM:
		  Serial.print(space);
		  break;
		}
		if(!(item->isSetting())) Serial.print(space);
	  }
	  else{
		for(byte j = 0; j < cols; j++){
		  Serial.print(space);
		}
	  }
	  Serial.println();
	}
	Serial.println(F("----------------"));
}