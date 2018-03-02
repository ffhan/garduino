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
