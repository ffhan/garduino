#ifndef __ACTION_INCLUDED__
#define __ACTION_INCLUDED__

#include "Arduino.h"
#include "Control.h"

using namespace std;

typedef int (*Promise)();

class FunctionNode {
  public:

    Promise event = NULL;

    int count = 0;

    Control *sys = NULL;
    FunctionNode *next = NULL;

};

class FunctionList {
  private:

    int len = 0;
    FunctionNode *head = NULL;
    FunctionNode *tail = NULL;

    Control *sys = NULL;

  public:

    FunctionList(Control *control);

    int getLen();

    void add(int (*func)());

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
