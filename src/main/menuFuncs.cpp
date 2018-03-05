#include "menuFuncs.h"

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