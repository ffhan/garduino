#include "Remote.h"
#include "pins.h"

Remote::Remote() {
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

void Remote::readRemote() {
  long mask = 0xFFFF;
  long prefixMask = 0xFF0000;
  if (irrecv->decode(results))
  {
    //Serial.println(results.value, HEX);
    if ((results->value & prefixMask) >> 16 != prefix) {
      Serial.println(F("Wrong remote!"));
    }
    else {
      switch ((code) mask & results->value) {
        case HOLD:
          holdLen++;
          //Serial.println(millis() - timer);
          //Serial.println(holdLen);
          break;

        case TIMEOUT:
          return;

        default:
          lastClick = (code) mask & results->value;
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
    switch (lastClick) {
      case LOCK:
        mode = 0;
        break;

      case LIGHT:
        if (isLongPress()) mode = 4;
        else mode = 1;
        break;

      case LOGGING:
        mode = 2;
        break;

      case HEATING:
        if (isLongPress()) mode = 6;
        else mode = 3;
        break;

      case WATERING:
        if (isLongPress()) mode = 10;
        else mode = 7;
        break;

      case MEASURE:
        mode = 5;
        break;

      case FAN:
        if (isLongPress()) mode = 11;
        else mode = 9;
        break;

      case TIME:
        mode = 8;
        break;
    }
    lastClick = TIMEOUT;
    holdLen = 0;
    sendingCode = mode;
  }
}
