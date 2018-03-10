#ifndef __ACTION_INCLUDED__
#define __ACTION_INCLUDED__

#include "Arduino.h"
#include "Control.h"

typedef int (Control::*Promise)();

class PromisePack {
  private:

    Control *sys;
    char *failMessage;
    Promise promise = NULL;
    bool complement;

  public:

    PromisePack(Control *sys, Promise promise, char *failMessage, bool complement = false);

    bool go();

};

class FunctionNode {
  public:

    PromisePack *event = NULL;

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

    void add(PromisePack *promise);

    void add() {}

    template <typename ... Args>
    void add(PromisePack *event, Args ... args) {
      add(event);
      add(args...);
    }

    bool allTrue();
    bool anyTrue();

};

class Action {
  private :

    Control *sys = NULL;
    Event event = NULL;
    FunctionList *list = NULL;

	SetBit setBit = NULL;
	int *bindValue = NULL;

  public :

    int code;
	int remoteCode;

    Action(Control *control, int code, Event event);
	Action(Control *control, int code, SetBit setBit, int *value);
	Action(Control *control, int code, SetBit setBit);

	void bindToValue(int *value);

	void bindToRemoteCode(int remoteCode);

    void addPromises() {}

    template <typename ... Args>
    void addPromises(PromisePack *event, Args ... args) {
      list->add(event);
      addPromises(args...);
    }

    void execute();

};

#endif
