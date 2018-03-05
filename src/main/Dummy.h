#ifndef __DUMMY_INCLUDED__
#define __DUMMY_INCLUDED__

#include "Arduino.h"

class Dummy{
  public:
  bool test;
  bool test2;
  int val;

  Dummy(bool t, bool t1, int value);

  void printStates();
  
  void printEnter();
};

typedef void (Dummy::*Event) ();

#endif