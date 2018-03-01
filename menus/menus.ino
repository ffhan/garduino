
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

  public : itemType type;
  private :

  
  char *title;
  
  byte selection = 0;

  Item *parent;
  bool hasParent = false;
  
  Item *before;
  bool top = false;
  
  Item *after;
  bool bottom = false;
  
  public : Item(char *title){
    this->title = title;
    type = ITEM;
  }
  public : char *getTitle(){
    return title;
  }
  public : itemType getType(){
    return type;
  }
  public : void setBefore(Item *before){
    this->before = before;
    top = true;
  }
  public : Item *getBefore(){
    return before;
  }
  public : void setAfter(Item *after){
    this->after = after;
    bottom = true;
  }
  public : Item *getAfter(){
    return after;
  }
  public : void setParent(Item *parent){
    this->parent = parent;
    hasParent = true;
  }
  public : Item *getParent(){
    return parent;
  }
  public : Item *up(){
    if(top) return before;
    return this;
  }
  public : Item *down(){
    if(bottom) return after;
    return this;
  }
  public : Item *back(){
    if(hasParent) return parent;
    return this;
  }

  public : char *getStringType(){
    switch(type){
      case ITEM:
      return "Item";
      case SETTING:
      return "Setting";
      case MENU:
      return "Menu";
    }
  }
};

typedef void (*Printer) (Item *item);


void printItem(Item *item){
  Serial.println(F("-------------------"));
  Serial.print(F("Item title: ")); Serial.println(item->getTitle());
  if(item->getBefore()){
    Serial.print(F("Item before: ")); Serial.println((item->getBefore())->getTitle());
  }
  if(item->getAfter()){
    Serial.print(F("Item after: ")); Serial.println((item->getAfter())->getTitle());
  }
  Serial.print(F("Item type: ")); Serial.println(item->getStringType());
}


class ItemNode{
  public : 
  ItemNode *previous = NULL;
  ItemNode *next = NULL;
  Item *item;
};
class ItemList{
  public:
  int len = 0;
  ItemNode *head;
  ItemNode *tail;

  Item *parent;

  ItemList(Item *parent){
    this->len = 0;
    this->head = NULL;
    this->tail = NULL;
    this->parent = parent;
  }

  void add(Item *item){
    ItemNode *node = new ItemNode();
    node->item = item;

    //printItem(node->item);

    if(head == NULL){
      head = node;
      tail = head;
      len++;
      printItem(node->item);
      return;
    }

    item->setParent(parent);

    node->previous = tail;
    
    item->setBefore(tail->item);
    tail->item->setAfter(node->item);
    
    tail->next = node;
    tail = node;
    len++;
    printItem(node->item);
    return;
  }
  Item *get(int index){
    ItemNode *head = this->head;
    Item *item = NULL;
    int i = 0;
    while(head && i <= index){
      head = head->next;
      item = head->item;
      i++;
    }
    return item;
  }
  void process(int index, Printer func){
    ItemNode *head = this->head;
    Item *item = NULL;
    int i = 0;
    while(head && i <= index){
      head = head->next;
      item = head->item;
      i++;
    }
    func(item);
  }
  void processAll(Printer func){
    Serial.println("--------START--------");
    ItemNode *head = this->head;
    Item *item = head->item;
    int i = 0;
    while(head){
      func(item);
      head = head->next;
      item = head->item;
    }
    Serial.println("-------END--------");
  }
};
class Menu : Item{

  private :

  //contains all items that are shown when this menu is clicked. 
  ItemList childItems = ItemList(this); 
  
  public : Menu(char *title) : Item(title){
    type = MENU;
  }
  public : void addItem(Item *item){
    childItems.add(item);
  }
  public : void addItems(){}
  template <typename Item, typename ... Items>
  void addItems(Item item, Items ... rest){
    addItem((Item) item);
    addItems(rest...);
  }
  public : Item *enter(){
    return childItems.get(0);
  }
  public : void processChildItems(Printer func){
    childItems.processAll(func);
  }
};

Menu mainMenu = Menu("My main menu");

Menu firstMenu = Menu("My first menu");
Menu secondMenu = Menu("Test holding menu");

Item firstItem = Item("My first item");
Item secondItem = Item("Test item");
Item thirdItem = Item("Third item");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mainMenu.addItems(&firstMenu, &secondMenu);
  secondMenu.addItems(&firstItem, &secondItem, &thirdItem);

  mainMenu.processChildItems(&printItem);
  secondMenu.processChildItems(&printItem);
}

void loop() {
  // put your main code here, to run repeatedly:

}
