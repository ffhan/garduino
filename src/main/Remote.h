#ifndef __REMOTE_INCLUDED__
#define __REMOTE_INCLUDED__

#include <IRremote.h>

class Remote {
  private : 

  IRrecv *irrecv;
  decode_results *results;
  
  byte prefix = 0xFF;
  byte errorCode = 200; // change it in code enum too.
  typedef enum {LOCK = 0x22DD, LIGHT = 0x807F, LOGGING = 0xA05F,
                            HEATING = 0x906F, WATERING = 0x40BF, MEASURE = 0x609F,
                            FAN = 0x50AF, TIME = 0xE01F,
                            HOLD = 0xFFFF, TIMEOUT = 0x200 //TIMEOUT IS CODE 200, HTTP CODE FOR OK; HERE MEANING NOTHING SHOULD HAPPEN.
                           } code;
  code lastClick;
  byte holdLen = 0;
  unsigned long timer = 0;

  byte holdRange = 5; //in hold signals
  byte range = 150; //in milliseconds -> IF THE REMOTE DOESN'T RESPOND, READ OR IF ANY ERROR IS OCCURING THIS IS THE FIRST THING TO CHECK.
  //CHECK THE READING TIME.

  byte sendingCode = errorCode;

  public : 

  Remote();
  
  byte getInstruction();
  byte getErrorCode();
  
  void readRemote();
  
  bool isLongPress();

  void onTick();
};

#endif
