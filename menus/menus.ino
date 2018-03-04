#include "Item.h"
#include "ItemNode.h"
#include "Menu.h"
#include "Setting.h"
#include "BoolSetting.h"
#include "IntSetting.h"
#include "Screen.h"

Screen *screen = (Screen*) malloc(sizeof(Screen));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Dummy *dummy = new Dummy(false, true, 6);

  Menu *mainMenu = new Menu("My main menu");
    
  Menu *firstMenu = new Menu("My first menu");
  Setting *nSetting = new Setting("Trigger", dummy, &Dummy::printEnter);
  Menu *secondMenu = new Menu("Test holding menu");
  
  Item *firstItem = new Item("My first item");
  Item *secondItem = new Item("Test item");
  Menu *randomMenu = new Menu("Testing interoperability");
  Item *thirdItem = new Item("Third item");
  
  Item *fourthItem = new Item("fourth");
  Menu *random2Menu = new Menu("Final menu");
  Menu *rand3Menu = new Menu("just a test");

  Setting *bSetting = new BoolSetting("Bool 1", &(dummy->test), dummy, &Dummy::printStates);
  Setting *b2Setting = new BoolSetting("Bool 2", &(dummy->test2), dummy, &Dummy::printStates);
  Setting *iSetting = new IntSetting("Int 1", &(dummy->val), dummy, &Dummy::printStates);

  mainMenu->addItems(firstMenu, nSetting);
  firstMenu->addItems(bSetting, secondMenu);
  secondMenu->addItems(b2Setting, firstItem, secondItem, randomMenu, thirdItem);
  randomMenu->addItems(fourthItem, random2Menu, iSetting, rand3Menu);

  screen = new Screen(16, 2, mainMenu);
  
  //mainMenu.processChildItem(1, &printItem);
  //printItem(mainMenu.getSubMenuItem(1));
  //screen.flash(&printItem);
  //mainMenu.processChildItems(&printItem);
  //secondMenu.processChildItems(&printItem);
  //randomMenu.processChildItems(&printItem);
}

void loop() {
  // put your main code here, to run repeatedly:
  bool typed = false;
  while(Serial.available() > 0){
    int choice = Serial.parseInt();
    if(!typed){
      Serial.println(choice);
      switch(choice){
        case 8:
        screen->up();
        break;
        case 2:
        screen->down();
        break;
        case 5: 
        screen->enter();
        break;
        case 3:
        screen->back();
        break;
        case 4:
        screen->left();
        break;
        case 6:
        screen->right();
        break;
      }
    //screen->flash(&printItem);
    screen->show();
    typed = true;
    }
  }
}


