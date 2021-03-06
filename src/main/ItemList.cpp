#include "ItemList.h"

ItemList::ItemList(Item *parent){
	this->len = 0;
	this->parent = parent;
}

void ItemList::add(Item *item){
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
Item *ItemList::get(int index){
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
void ItemList::process(int index, Printer func){
	Item *item = get(index);
	if(item) func(item);
}
void ItemList::processAll(Printer func){
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
