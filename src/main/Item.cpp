#include "Item.h"

Item::Item(char *title) {
  this->title = title;
  type = ITEM;
}
char *Item::getTitle() {
  return title;
}
itemType Item::getType() {
  return type;
}
void Item::setBefore(Item *before) {
  this->before = before;
  top = true;
}
Item *Item::getBefore() {
  return before;
}
void Item::setAfter(Item *after) {
  this->after = after;
  bottom = true;
}
Item *Item::getAfter() {
  return after;
}
void Item::setParent(Item *parent) {
  this->parent = parent;
  hasParent = true;
}
Item *Item::getParent() {
  return parent;
}
Item *Item::up() {
  if (top) return before;
  return this;
}
Item *Item::down() {
  if (bottom) return after;
  return NULL;
}
Item *Item::back() {
  if (hasParent) return parent;
  return NULL;
}
Item *Item::enter() {
  return NULL;
}
void Item::left() {
  return;
}
void Item::right() {
  return;
}

bool Item::isMenu() {
  return (type == MENU);
}
bool Item::isSetting() {
  return (type == BOOLSETTING || type == INTSETTING || type == SETTING || type == INTBITSETTING || type == BOOLBITSETTING);
}

char *Item::getStringType() {
  switch (type) {
    case ITEM:
      return "Item";
    case BOOLSETTING:
      return "Boolean setting";
    case INTSETTING:
      return "Integer setting";
    case SETTING:
      return "Basic setting";
    case INTBITSETTING:
      return "Bit-controlled integer setting";
    case BOOLBITSETTING:
      return "Bit-controlled boolean setting";
    case MENU:
      return "Menu";
  }
}
byte Item::getIndex() {
  return index;
}
byte Item::setIndex(byte index) {
  this->index = index;
}
