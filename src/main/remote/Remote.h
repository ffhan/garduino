#ifndef __REMOTE_INCLUDED__
#define __REMOTE_INCLUDED__

#include <IRremote.h>

class Control;
class Screen;

class Remote {
private:

	IRrecv *irrecv = (IRrecv *) malloc(sizeof(IRrecv));
	decode_results *results = (decode_results *) malloc(sizeof(decode_results));

  Control *sys;
  Screen *screen;

	byte prefix = 0xFF;
	byte errorCode = 200; // change it in code enum too.
  
	typedef enum {
		ZERO = 0x22DD, ONE = 0x807F, TWO = 0xA05F,
		THREE = 0x906F, FOUR = 0x40BF, FIVE = 0x609F,
		SIX = 0xFFAA, SEVEN = 0xFFBB,
		NINE = 0x50AF, EIGHT = 0xE01F,
    UP = 0xCA35, DOWN = 0x18E7,
    LEFT = 0xF20D, RIGHT = 0xEA15, ENTER = 0x9867,
    MENU = 0x2AD5,
		HOLD = 0xFFFF, TIMEOUT = 0x200 //TIMEOUT IS CODE 200, HTTP CODE FOR OK; HERE MEANING NOTHING SHOULD HAPPEN.
	} code;
	code lastClick;
	byte holdLen = 0;
	unsigned long timer = 0;

	byte holdRange = 5; //in hold signals
	byte range = 150; //in milliseconds -> IF THE REMOTE DOESN'T RESPOND, READ OR IF ANY ERROR IS OCCURING THIS IS THE FIRST THING TO CHECK.
	//CHECK THE READING TIME.

	byte sendingCode = errorCode;

public:

	Remote(Control *sys);

 void bindScreen(Screen *screen);

	byte getInstruction();
	byte getErrorCode();

	void readRemote();

	bool isLongPress();

	void onTick();
};

#endif
