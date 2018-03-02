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

