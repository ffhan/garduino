void printEvent(){
  Serial.println("HEY MAN");
}
void printEvent2(){
  Serial.println("WOOOOO");
}
bool test = true;
bool test2 = false;
int val = 3;

Screen *screen = (Screen*) malloc(sizeof(Screen));
Menu mainMenu = Menu("My main menu");
  
Menu firstMenu = Menu("My first menu");
//BoolSetting *bSetting = (BoolSetting *) malloc(sizeof(BoolSetting));
Menu secondMenu = Menu("Test holding menu");

//BoolSetting *b2Setting = (BoolSetting *) malloc(sizeof(BoolSetting));
Item firstItem = Item("My first item");
Item secondItem = Item("Test item");
Menu randomMenu = Menu("Testing interoperability");
Item thirdItem = Item("Third item");

Item fourthItem = Item("fourth");
Menu random2Menu = Menu("Final menu");
//IntSetting *iSetting = (IntSetting *) malloc(sizeof(IntSetting));
Menu rand3Menu = Menu("just a test");
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  BoolSetting *bSetting = new BoolSetting("Bool 1", &test, &printEvent);
  BoolSetting *b2Setting = new BoolSetting("Bool 2", &test2, &printEvent);
  IntSetting *iSetting = new IntSetting("Int 1", &val, &printEvent2);

  mainMenu.addItems(&firstMenu, bSetting, &secondMenu);
  secondMenu.addItems(b2Setting, &firstItem, &secondItem, &randomMenu, &thirdItem);
  randomMenu.addItems(&fourthItem, &random2Menu, iSetting, &rand3Menu);

  *screen = Screen(16, 2, &mainMenu);
  
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
        case 7:
        mainMenu.processChildItems(&printItem);
        secondMenu.processChildItems(&printItem);
        randomMenu.processChildItems(&printItem);
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

