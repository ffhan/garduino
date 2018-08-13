#include "Remote.h"
#include "../menu/Screen.h"
#include "../../support/pins.h"

Remote::Remote(Control *sys) {
  this->sys = sys;
	this->irrecv = new IRrecv(IR_PIN);
	Serial.print(F("Initialising IRremote sensor...\t"));
	irrecv->enableIRIn();
	Serial.println(F("done."));
}

byte Remote::getInstruction() {
	return sendingCode;
}
byte Remote::getErrorCode() {
	return errorCode;
}

void Remote::bindScreen(Screen *screen){
  this->screen = screen;
}

void Remote::readRemote() {
	long mask = 0xFFFF;
	long prefixMask = 0xFF0000;
	if (irrecv->decode(results))
	{
		//Serial.println(results->value);
		if ((results->value & prefixMask) >> 16 != prefix) {
			Serial.println(F("Wrong remote!"));
		}
		else {
			switch ((code)mask & results->value) {
			case HOLD:
				holdLen++;
				//Serial.println(millis() - timer);
				//Serial.println(holdLen);
				break;

			case TIMEOUT:
				return;

			default:
				lastClick = (code)mask & results->value;
				Serial.println(lastClick, HEX);
				holdLen = 0;
				break;
			}
			timer = millis();
		}

		//printState();
		irrecv->resume(); // Receive the next value
	}
}

bool Remote::isLongPress() {
	if (holdLen > holdRange) return true;
	return false;
}

void Remote::onTick() {
	if (lastClick == TIMEOUT) sendingCode = errorCode; // if nothing is clicked.
	if (millis() - timer >= range) {
		byte mode = errorCode;

    switch(lastClick){
        case ZERO:
        if(isLongPress()) mode = 0;
        break;

        case ONE:
        if(isLongPress()) mode = 4;
        else mode = 1;
        break;

        case TWO:
        mode = 2;
        break;

        case THREE:
        if(isLongPress()) mode = 6;
        else mode = 3;
        break;

        case SEVEN:
        if(isLongPress()) mode = 10;
        else mode = 7;
        break;
        
        case FIVE:
        mode = 5;
        break;

        case 9:
        if(isLongPress()) mode = 11;
        else mode = 9;
        break;

        case EIGHT:
        mode = 8;
        break;

        case UP:
        if(screen) screen->up();
        break;

        case DOWN:
        if(screen) screen->down();
        break;

        case LEFT:
        if(screen) screen->left();
        break;

        case RIGHT:
        if(screen) screen->right();
        break;

        case ENTER:
        if(screen) screen->enter();
        break;

        case MENU:
        if(screen) screen->back();
        break;
    }

		lastClick = TIMEOUT;
		holdLen = 0;
		sendingCode = mode;
	}
}
