#ifndef __ACTION_INCLUDED__
#define __ACTION_INCLUDED__

#include "Control.h"

typedef int (Control::*Promise)();

class FunctionNode {
  public:

    Promise event;

    Control *sys;
    FunctionNode *next;

};

class FunctionList {
  private:

    int len = 0;
    FunctionNode *head;
    FunctionNode *tail;

    Control *sys;

  public:

    FunctionList(Control *control);

    void add(Promise event);

    bool allTrue();
    bool anyTrue();

};

class Action {
  private :

    Control *sys;
    Event event;
    FunctionList *list;

  public :

    Action(Control *control, Event event);

    void addPromises() {}

    template <typename ... Args>
    void addPromises(Promise event, Args ... args) {
      list->add(event);
      addPromises(args...);
    }

    void execute();

};

#endif
