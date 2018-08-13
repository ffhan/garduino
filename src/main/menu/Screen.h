#ifndef __SCREEN_INCLUDED__
#define __SCREEN_INCLUDED__

#include "Arduino.h"
//#include "Item.h"
//#include "IntSetting.h"
//#include "BoolSetting.h"
//#include "Setting.h"
#include "Menu.h"

class Screen{
  
  private : 

  static const char space = ' ';
  static const char slct = '>';
  static const char menuSign = '+';
  static const char falseSign = 'x';
  static const char trueSign = 'o';
  static const char unsaved = '_';
  
  byte rows, cols;
  Menu *mainMenu = NULL;
  Item *currentItem = NULL;

  /*
   * cursorPosition is the position on the screen. It's dictated by the screen size.
   * index determines the position inside a subMenu.
   * 
  */
  byte *cursorPosition, index = 0, mainPosition = 0;

  public : 

  Screen(byte cols, byte rows, Menu *mainMenu);

  //use only for constructor.
  void bindToMenu(Menu *menu);
  
  void up();
  void down();
  void back();
  void enter();
  
  void left();
  void right();
  
  void flash(Printer func);
  template <class T>
  void valueSettingTrailPrint(Item *item);
  void show();
};

#endif
