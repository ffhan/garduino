template<size_t SIZE, class T> inline size_t array_size(T (&arr)[SIZE]) {
    return SIZE;
}
typedef enum {
    ITEM = 0, SETTING = 1, MENU = 2
} itemType;


/*
 * Base menu item. Used to implement Settings and Menus. 
 * If using the Item itself you're just putting a title in the menu.
 */
class Item{

  public : 
  itemType type;
  
  private :

  byte index;
  char *title;

  Item *parent = NULL;
  bool hasParent = false;
  
  Item *before = NULL;
  bool top = false;
  
  Item *after = NULL;
  bool bottom = false;
  
  public : 
  Item(char *title){
    this->title = title;
    type = ITEM;
  }
  char *getTitle(){
    return title;
  }
  itemType getType(){
    return type;
  }
  void setBefore(Item *before){
    this->before = before;
    top = true;
  }
  Item *getBefore(){
    return before;
  }
  void setAfter(Item *after){
    this->after = after;
    bottom = true;
  }
  Item *getAfter(){
    return after;
  }
  void setParent(Item *parent){
    this->parent = parent;
    hasParent = true;
  }
  Item *getParent(){
    return parent;
  }
  Item *up(){
    if(top) return before;
    return this;
  }
  Item *down(){
    if(bottom) return after;
    return this;
  }
  Item *back(){
    if(hasParent) return parent;
    return this;
  }

  char *getStringType(){
    switch(type){
      case ITEM:
      return "Item";
      case SETTING:
      return "Setting";
      case MENU:
      return "Menu";
    }
  }
  public : byte getIndex(){
    return index;
  }
  byte setIndex(byte index){
    this->index = index;
  }
};

typedef void (*Printer) (Item *item);




class ItemNode{
  public : 
  ItemNode *previous = NULL;
  ItemNode *next = NULL;
  Item *item = NULL;
};
/*
 * a better name would be SubMenu, but I didn't want to draw a too close relationship to
 * Menu, because this holds all Item objects and does not behave like a menu; it's a helper
 * class.
 */

class ItemList{
  public:
  int len = 0;
  ItemNode *head = NULL;
  ItemNode *tail = NULL;

  Item *parent;

  ItemList(Item *parent){
    this->len = 0;
    this->parent = parent;
  }

  void add(Item *item){
    ItemNode *node = new ItemNode();
    node->item = item;
    node->item->setIndex(len);

    //printItem(node->item);
    
    item->setParent(parent);

    if(head == NULL){
      head = node;
      tail = head;
      len++;
      //printItem(node->item);
      return;
    }

    node->previous = tail;
    
    item->setBefore(tail->item);
    tail->item->setAfter(node->item);
    
    tail->next = node;
    tail = node;
    len++;
    //printItem(node->item);
    return;
  }
  Item *get(int index){
    if(len == 0){
      Serial.println("empty.");
      return NULL;
    }
    else if(index >= len) return NULL;
    
    ItemNode *head = this->head;
    Item *item = head->item;
    int i = 1;
    while(head && i <= index && item){
      head = head->next;
      item = head->item;
      i++;
    }
    if(!head && i > index) item = NULL;
    
    return item;
  }
  void process(int index, Printer func){
    Item *item = get(index);
    if(item) func(item);
  }
  void processAll(Printer func){
    ItemNode *head = this->head;
    Item *item = head->item;
    Serial.print(F("---"));Serial.print(item->getParent()->getTitle());Serial.println(F("---"));
    int i = 0;
    while(head){
      func(item);
      head = head->next;
      item = head->item;
    }
    Serial.println(F("-------END--------"));
  }
};
class Menu : public Item{

  private :

  //contains all items that are shown when this menu is clicked. 
  ItemList childItems = ItemList(this); 

  byte selector = 0;
  
  public: 
  Menu(char *title) : Item(title){
    type = MENU;
  }
  void addItem(Item *item){
    childItems.add(item);
  }
  void addItems(){}
  template <typename Item, typename ... Items>
  void addItems(Item item, Items ... rest){
    addItem((Item) item);
    addItems(rest...);
  }
  Item *enter(){
    Item *temp = childItems.get(0);
    if(temp) return temp;
    return this;
  }
  void processChildItem(int index, Printer func){
    childItems.process(index, func);
  }
  void processChildItems(Printer func){
    childItems.processAll(func);
  }
  Item *getSubMenuItem(int index){
    return childItems.get(index);
  }

  byte *getSelector(){
    return &selector;
  }
  //not really needed since I exposed selector directly, but it's less verbose.
  byte getSelectorValue(){ 
    return selector;
  }
};

void printItem(Item *item){
  Serial.println(F("-------------------"));
  Serial.print(F("Item title: ")); Serial.println(item->getTitle());
  if(item->getBefore()){
    Serial.print(F("Item before: ")); Serial.println((item->getBefore())->getTitle());
  }
  if(item->getAfter()){
    Serial.print(F("Item after: ")); Serial.println((item->getAfter())->getTitle());
  }
  if(item->getParent()){
    Serial.print(F("Parent: ")); Serial.println((item->getParent())->getTitle());
  }
  Serial.print(F("Item type: ")); Serial.println(item->getStringType());
  if(item->getType() == MENU){
    Serial.print(F("Selector: ")); Serial.print(" "); Serial.println(((Menu*)item)->getSelectorValue());
  }
}



class Screen{
  
  private : 

  static const char space = ' ';
  static const char slct = '>';
  static const char menuSign = '+';
  
  byte rows, cols;
  Menu *mainMenu;
  Item *currentItem;

  /*
   * cursorPosition is the position on the screen. It's dictated by the screen size.
   * index determines the position inside a subMenu.
   * 
  */
  byte *cursorPosition, index = 0, mainPosition = 0;

  public : 

  Screen(byte cols, byte rows, Menu *mainMenu){
    this->rows = rows;
    this->cols = cols;
    bindToMenu(mainMenu);
  }

  //use only for constructor.
  void bindToMenu(Menu *menu){
    this->mainMenu = menu;
    cursorPosition = menu->getSelector();
    currentItem = mainMenu->enter();
  }
  
  void up(){
    if(currentItem == currentItem->up()) return;
    index--;
    if(*cursorPosition - 1 >= 0) (*cursorPosition)--;
    currentItem = currentItem->up();
  }
  void down(){
    if(currentItem == currentItem->down()){
      return;
    }
    index++;
    if(*cursorPosition + 1 < rows) (*cursorPosition)++;
    currentItem = currentItem->down();
  }
  void back(){
    if(currentItem == currentItem->back()) return;
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
  void enter(){
    if(currentItem->getType() != MENU) return; //expand later.
    //if it's a menu
    Menu *menu = (Menu*) currentItem;
    
    if(currentItem == menu->enter()) return;
    mainMenu = menu;
    cursorPosition = mainMenu->getSelector();
    *cursorPosition = 0; // reset the cursor position, because we do the same for the index.
    currentItem = mainMenu->enter();
    index = currentItem->getIndex();
  }
  void flash(Printer func){
    Serial.print(*cursorPosition); Serial.print(space); Serial.println(index);
    func(currentItem);
  }
  void show(){
    Serial.print(*cursorPosition); Serial.print(space); Serial.print(index);
    Serial.print(space);Serial.println(mainMenu->getTitle());
    /*
     * Screen is set up this way:
     * Each row has 3 parts:
     * _SLCT_|_________Title__________|_marker_
     *    >  |     Level setting      |  .xX
     *       |   Numerical setting    |   6
     *       |  Boolean false setting |   o
     *       |  Boolean true setting  |   x
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
        switch(item->getType()){
          case MENU:
          Serial.print(space);
          Serial.print(menuSign);
          Serial.print(space);
          break;
          case ITEM:
          for(byte j = 0; j < 3; j++){
            Serial.print(space);
          }
          break;
        }
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
};

Screen *screen = (Screen*) malloc(sizeof(Screen));
Menu mainMenu = Menu("My main menu");
  
Menu firstMenu = Menu("My first menu");
Menu secondMenu = Menu("Test holding menu");

Item firstItem = Item("My first item");
Item secondItem = Item("Test item");
Menu randomMenu = Menu("Testing interoperability");
Item thirdItem = Item("Third item");

Item fourthItem = Item("fourth");
Menu random2Menu = Menu("Final menu");
Menu rand3Menu = Menu("just a test");
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  

  mainMenu.addItems(&firstMenu, &secondMenu);
  secondMenu.addItems(&firstItem, &secondItem, &randomMenu, &thirdItem);
  randomMenu.addItems(&fourthItem, &random2Menu, &rand3Menu);

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
        case 0:
        screen->up();
        break;
        case 1:
        screen->down();
        break;
        case 2: 
        screen->enter();
        break;
        case 3:
        screen->back();
        break;
        case 4:
        mainMenu.processChildItems(&printItem);
        secondMenu.processChildItems(&printItem);
        randomMenu.processChildItems(&printItem);
        break;
      }
    //screen->flash(&printItem);
    screen->show();
    typed = true;
    }
  }
}
