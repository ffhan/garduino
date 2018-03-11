#ifndef __CONTROL_INCLUDED__
#define __CONTROL_INCLUDED__

#include "RTClib.h"

class WebController;
class Remote;
class Measuring;
class Logging;
class ActionBinaryTree;
class PromisePack;
class Action;
class Screen;

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

private:

	const int _defaultData = 0x0057;
	int _data = 0x0057;
	int fanSpeed_bitsize = 3;

	//printTime probably won't work if this is not set.
	DateTime *now = (DateTime *)malloc(sizeof(DateTime));

	typedef enum {
		LOCK = 0, LOGGING = 1, WRITTEN = 2, IS_INITIALISED = 3,
		LIGHTING_STATE = 4, LIGHT_ADMIN = 5,
		HEATING_STATE = 6, HEAT_ADMIN = 7,
		WATERING_STATE = 8, WATERING_ADMIN = 9,
		FAN_SPEED = 10, FAN_ADMIN = 13,
		CODE_FETCH = 14, NET_RECONF = 15
	} sector;

public:

	Measuring * measure = NULL;
	Logging *logger = NULL;
	Remote *remote = NULL;
	RTC_DS3231 *rtc = NULL;
	WebController *web = NULL;
	ActionBinaryTree *actions = NULL;

	/*
	  void globalLockEvent();
	  void lightAdminEvent();
	  void lightStateEvent();
	  void heatAdminEvent();
	  void heatStateEvent();
	  void loggingEvent();
	  void wateringAdminEvent();
	  void wateringStateEvent();
	  void measureEvent();
	  void printTimeEvent();
	  void setTimeEvent();
	*/

	PromisePack *globalLockPromise = NULL;
	PromisePack *lightAdminPromise = NULL;
	PromisePack *heatAdminPromise = NULL;
	PromisePack *loggingPromise = NULL;
	PromisePack *wateringAdminPromise = NULL;
	PromisePack *fanAdminPromise = NULL;

	Action *globalLockAction = NULL;

	Action *lightAdminAction = NULL;
	Action *lightStateAction = NULL;

	Action *printTimeAction = NULL;
	Action *measureAction = NULL;

	Action *heatAdminAction = NULL;
	Action *heatStateAction = NULL;

	Action *wateringAdminAction = NULL;
	Action *wateringStateAction = NULL;

	Action *fanAdminAction = NULL;
	Action *fanSpeedAction = NULL;

	Action *renewNetAction = NULL;

	Control();

private: int readPosition(int posit);

		 int readSpeed(int init_position, int bitSize);
		 int readState(sector search);

public: int getRawData();

private: void setPosition(int posit);

		 void erasePosition(int posit);

		 void writePosition(int posit, bool value);
		 void writeSpeed(int posit, int bitSize, int value);
		 void writeState(sector search, int value);

public:

	void printState();

	int getCodeFetch();
	void setCodeFetch(int value);
	int getNetReconf();
	void setNetReconf(int value);
	int getLock();
	int getLogging();
	int getWritten();
	int getIsInitialised();
	int getLightingState();
	int getLightAdmin();
	int getHeatingState();
	int getHeatAdmin();
	int getWateringState();
	int getWateringAdmin();
	int getFanSpeed();
	int getFanAdmin();

	void setLock(int value);
	void setLogging(int value);

	void setWritten(int value);
	void setIsInitialised(int value);
	void setLightingState(int value);
	void setLightAdmin(int value);
	void setHeatingState(int value);
	void setHeatAdmin(int value);
	void setWateringState(int value);
	void setWateringAdmin(int value);
	void setFanSpeed(int value);
	void setFanAdmin(int value);

	int getEnumSize();

	void heatSwitch(int state);

	void mainSwitch(int choice);

	void globalLockEvent();
	void lightAdminEvent();
	void lightStateEvent();
	void heatAdminEvent();
	void heatStateEvent();
	void loggingEvent();
	void wateringAdminEvent();
	void wateringStateEvent();
	void fanAdminEvent();
	void fanStateEvent(int value);
	void measureEvent();
	void printTimeEvent();
	void setTimeEvent();

	void renewNetEvent();

	DateTime getTime();
	void updateTime();

	void tick();

	void autoLight();

	void getRemoteInstructions();

	void update();

	void renewNetwork(bool rewriteDevice);

	void logControl();

	void empty();
	void test();

	void bindScreenToRemote(Screen *screen);

};

typedef void (Control::*Event)();
typedef int (Control::*GetBit)();
typedef void (Control::*SetBit)(int value);

#endif
