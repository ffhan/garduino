#ifndef __CONTROL_INCLUDED__
#define __CONTROL_INCLUDED__

#include "RTClib.h"

class WebController;
class Remote;
class Measuring;
class Logging;

class Control {
    /*
       | BIT | STATE | NAME  |    WHAT IT DOES
       -------------------------------------
       |  0  |   1   | AUTO LOCK | 1 SETS ALL ADMIN PRIVILEGES OFF, 0 ENABLES CUSTOM PRIVILEGES
       |  1  |   1   | LOGGING | 1 - LOGGING ON, 0 - LOGGING IS OFF (NOT WRITING TO SD CARD)
       |  2  |   0   | WRITTEN | 1 - CURRENT WRITE CYCLE COMPLETED, 0 - NOT COMPLETED -- USED ONLY TO ENSURE THAT LOGGING IS DONE ONLY ONCE PER CYCLE.
       |  3  |  _0_  | IS_INITIALISED | 1 - SD CARD INITIALISED, 0 - SD CARD NOT INITIALISED
       |  4  |   1   | LIGHTS ON/OFF | CONTROLS LIGHT
       |  5  |   0   | LIGHT ADMIN | 1 - USER CONTROLS LIGHTING, 0 - AUTO CONTROLLED LIGHTING
       |  6  |   1   | HEAT ON/OFF | CONTROLS HEATING
       |  7  |  _0_  | HEAT ADMIN | 1 - USER CONTROLS HEATING, 0 - AUTO CONTROLLED HEATING
       |  8  |   0   | WATERING ON/OFF | CONTROLS WATERING
       |  9  |   0   | WATERING ADMIN | 1 - USER CONTROLS WATERING, 0 - AUTO CONTROLLED WATERING
       | 10  |   0   | FAN SPEED 0 |
       | 11  |  _0_  | FAN SPEED 1 | BITS REPRESENTING FAN SPEED (0 - FULL OFF, 7 - FULL ON)
       | 12  |   0   | FAN SPEED 2 |
       | 13  |   0   | FAN ADMIN   | 1 - USER CONTROLS THE FAN, 0 - AUTO CONTROLLED FAN
       | 14  |   0   | CODE_FETCH  | 0 - CODE FETCHED, 1 - CODE NOT YET FETCHED
       | 15  |  _0_  | NET_RECONF  | 0 - NETWORK CONFIGURED, 1 - NETWORK NEEDS RECONFIGURATION
    */

  private : 
  
  const int _defaultData = 0x0057;
  int _data = 0x0057;
  int fanSpeed_bitsize = 3;

  DateTime now;

  typedef enum {LOCK = 0, LOGGING = 1, WRITTEN = 2, IS_INITIALISED = 3,
                            LIGHTING_STATE = 4, LIGHT_ADMIN = 5,
                            HEATING_STATE = 6, HEAT_ADMIN = 7,
                            WATERING_STATE = 8, WATERING_ADMIN = 9,
                            FAN_SPEED = 10, FAN_ADMIN = 13,
                            CODE_FETCH = 14, NET_RECONF = 15
                           } sector;

  public : 
  
  Measuring *measure;
  Logging *logger;
  Remote *remote;
  RTC_DS3231 *rtc;
  WebController *web;

  Control();
  
  private : int readPosition(int posit);

  private : int readSpeed(int init_position, int bitSize);
  private : int readState(sector search);

  public : int getRawData();
    
  private : void setPosition(int posit);

  private : void erasePosition(int posit);

  private : void writePosition(int posit, bool value);
  private : void writeSpeed(int posit, int bitSize, int value);
  private : void writeState(sector search, int value);

  public : void printState();

  public : int getCodeFetch();
  public : void setCodeFetch(int value);
  public : int getNetReconf();
  public : void setNetReconf(int value);
  public : int getLock();
  public : int getLogging();
  public : int getWritten();
  public : int getIsInitialised();
  public : int getLightingState();
  public : int getLightAdmin();
  public : int getHeatingState();
  public : int getHeatAdmin();
  public : int getWateringState();
  public : int getWateringAdmin();
  public : int getFanSpeed();
  public : int getFanAdmin();

  public : void setLock(int value);
  public : void setLogging(int value);
    
  public : void setWritten(int value);
  public : void setIsInitialised(int value);
  public : void setLightingState(int value);
  public : void setLightAdmin(int value);
  public : void setHeatingState(int value);
  public : void setHeatAdmin(int value);
  public : void setWateringState(int value);
  public : void setWateringAdmin(int value);
  public : void setFanSpeed(int value);
  public : void setFanAdmin(int value);

  public : int getEnumSize();

  public : void heatSwitch(int state);

  public : void mainSwitch(int choice);

  public : DateTime getTime();
  public : void updateTime();

  public : void tick();

  public : void autoLight();

  public : void autoLight(DateTime now);
    
  public : void getRemoteInstructions();

  public : void update();
  public : void tick(DateTime now);
  public : void renewNetwork(bool rewriteDevice);

  public : void getRemoteInstructions(DateTime now);

  public : void logControl();

  public : void empty();
};

typedef void (Control::*Event)();
typedef int (Control::*GetBit)();
typedef void (Control::*SetBit)(int value);

#endif
