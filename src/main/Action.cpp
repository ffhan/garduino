#include "Action.h"

FunctionList::FunctionList(Control *control) {
  sys = control;
}

void FunctionList::add(Promise event) {
  FunctionNode *node = new FunctionNode();
  node->event = event;
  node->sys = sys;
  node->count = len++;
  //Serial.println(node->count);

  //printItem(node->item);

  if (head == NULL) {
    head = node;
    tail = head;

    //printItem(node->item);
    return;
  }

  tail->next = node;

  tail = node;

  //printItem(node->item);
  return;
}

int FunctionList::getLen() {
  return len;
}

bool FunctionList::allTrue() {
  if (len == 0) {
    Serial.println("empty.");
    return true;
  }

  FunctionNode *head = this->head;
  Promise item = head->event;

  //Serial.print("0"); Serial.println((sys->*item)());
  if (!(sys->*item)()) return false;

  int i = 1;
  while (head) {
    head = head->next;
    item = head->event;

    //Serial.print(i); Serial.println((sys->*item)());
    if (!(sys->*item)()) return false;

    i++;
  }

  return true;
}

bool FunctionList::anyTrue() {
  if (len == 0) {
    Serial.println("empty.");
    return true;
  }

  FunctionNode *head = this->head;
  Promise item = head->event;

  if ((sys->*item)()) return true;

  int i = 1;
  while (head) {
    head = head->next;
    item = head->event;

    if ((sys->*item)()) return true;

    i++;
  }

  return false;
}

Action::Action(Control *control, Event event) {
  sys = control;
  list = new FunctionList(sys);
  this->event = event;
}

void Action::execute() {
  if (list->allTrue()) (sys->*event)();
}

