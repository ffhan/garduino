#ifndef __WEB_INCLUDED__
#define __WEB_INCLUDED__

#include <Ethernet.h>
#include "RTClib.h"

class Control;
class Measuring;
class Logging;

class WebController {
private:

	Control * sys;
	EthernetClient client;

	typedef int (WebController::*Fragmentor) (const char *fragment);
	typedef int (WebController::*EProcessor) (int index);
	typedef void (WebController::*ContentWriter)();
	typedef int (WebController::*FloatProcessor) (float value);

public:

	WebController(Control *sys);

	int buildEntryJson(Fragmentor fp, Fragmentor tp, EProcessor ep, FloatProcessor flp, DateTime now, int soilMoisture, char *soilDescription, float airHumidity, float airTemperature, bool lightState);

	int processFloat(float value);
	int floatLen(float value);

	void buildGetCodeRoute(char *route);
	void buildUpdateStateRoute(char *route);

	int buildUpdateJson(Fragmentor fp, Fragmentor tp, EProcessor ep);

	int buildCodeJson(Fragmentor fp, Fragmentor tp, EProcessor ep, DateTime now);


	/* LEGACY CODE USED TO TRANSPORT TOKEN FROM ROM TO EEPROM.
	  void saveToken(){
	  char buff;
	  char test;
	  int i = 0;
	  do{
		buff = pgm_read_byte_near(token + i);
		EEPROM.put(i, buff);
		EEPROM.get(i, test);
		Serial.print(test);
		i++;
	  }while(buff != '\0');
	  Serial.println(F("success."));
	  }*/

	void printEeprom(int index);

	int dumpEeprom(int index, char *dest);

	int _eeprom(int index, bool doWrite);

	int processEeprom(int index);

	int eepromLen(int index);

	int processValue(const char *value);
	int valueLen(const char *value);

	void writeFragment(const char *fragment);

	int dumpFragment(const char *fragment, char *dest);

	int processFragment(const char *fragment);

	int fragmentLen(const char *fragment);

	int strLen(char *str);

	bool connectToServ();

	void writeConnection();

	void writeJwtToken();

	void writeRoute(const char *route);
	void writeHost();
	void writeContentType();
	void writeCacheType();

	void writeNewLine();

	void printMakeHttp();
	void printOk();
	void printConnFailed();

	void writeStaticRoute(const char *route);

	void getter(const char *route, bool staticPath);
	void writePostContent();

	void writeCodeContent();
	void writeUpdateContent();

	void writeLoginContent();

	void writeContent(char *json);

	//USE FOR POST & PUT
	void poster(const char *route, bool writeToken, ContentWriter cw, bool staticPath);
	void getMyId();

	int getCode();

	void login();

	void completeCode();

	void postEntry();

	void updateState();

	void printResponse();

	void parseResponse(const char *before, const char *after, int *action, bool eepromWrite, bool writeNewLine, int eepromIndex);

};

#endif
