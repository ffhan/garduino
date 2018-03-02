template<size_t SIZE, class T> inline size_t array_size(T (&arr)[SIZE]) {
    return SIZE;
}
typedef enum {
    ITEM = 0, BOOLSETTING = 1, INTSETTING = 2, MENU = 3
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
  virtual Item *enter(){
    return this;
  }
  virtual void left(){
    return;
  }
  virtual void right(){
    return;
  }

  bool isMenu(){
    return (type == MENU);
  }
  bool isSetting(){
    return (type == BOOLSETTING || type == INTSETTING);
  }

  char *getStringType(){
    switch(type){
      case ITEM:
      return "Item";
      case BOOLSETTING:
      return "Boolean setting";
      case INTSETTING:
      return "Integer setting";
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
