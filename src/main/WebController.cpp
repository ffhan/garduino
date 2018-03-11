#include "WebController.h"
#include "Control.h"
#include "Measuring.h"
#include "Logging.h"

#include <EEPROM.h>
#include <avr/pgmspace.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 1);
const char *server = "garduinoproject.azurewebsites.net";
//const char server[] PROGMEM = {"garduinoproject.azurewebsites.net"};

//const char token[] PROGMEM = {"Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJhZG1pbkBhZG1pbi5vcmciLCJqdGkiOiI1Mzk1ZDZlMS02MmMwLTRmNjMtYmMzOS1jOTEyZWFkMDhhYmIiLCJodHRwOi8vc2NoZW1hcy54bWxzb2FwLm9yZy93cy8yMDA1LzA1L2lkZW50aXR5L2NsYWltcy9uYW1laWRlbnRpZmllciI6ImI4M2VmZDU0LTVlN2ItNDIwMy04NTNjLWRjODk2MTczNTkxZSIsImV4cCI6MTUxOTAzNjY3NiwiaXNzIjoiaHR0cHM6Ly9sb2NhbGhvc3Q6NDQzOTUiLCJhdWQiOiJodHRwczovL2xvY2FsaG9zdDo0NDM5NSJ9.6MeaXtkiLi-qNvSYHSO9JMvbvo7FwpB8_9_pf9D4fFI\r\n"};
const char tokenHead[] PROGMEM = { "Authorization: Bearer " };
const char host[] PROGMEM = { "Host: garduinoproject.azurewebsites.net\r\n" };
const char codeCall[] PROGMEM = { "PUT /api/Code/latest HTTP/1.1\r\n" };
const char loginPath[] PROGMEM = { "POST /api/account HTTP/1.1\r\n" };
const char codeGetFrnt[] PROGMEM = { "GET /api/Code/latest/" };
const char codeGetBack[] PROGMEM = { " HTTP/1.1\r\n" };
const char nameCall[] PROGMEM = { "GET /api/Device/call/Garduino HTTP/1.1\r\n" };
const char entryPost[] PROGMEM = { "POST /api/entry HTTP/1.1\r\n" };

const char updateFrnt[] PROGMEM = { "PUT /api/Device/" };
const char updateBack[] PROGMEM = { "/status HTTP/1.1\r\n" };

const char updateJson[] PROGMEM = { "{\"State\":" };
const char updateJson2[] PROGMEM = { "}\r\n" };

const char loginJson[] PROGMEM = { "{\"Email\":\"admin@admin.org\",\"Password\":\"Admin_projekt0\",\"RememberMe\":\"false\"}\r\n" };

const char contType[] PROGMEM = { "Content-Type: application/json\r\n" };
const char cacheCtrl[] PROGMEM = { "Cache-Control: no-cache\r\n" };
const char newLine[] PROGMEM = { "\r\n" };
const char contLen[] PROGMEM = { "Content-Length: " };
const char connection[] PROGMEM = { "Connection: close\r\n" };
const char deviceId[] PROGMEM = { "c815dc1d-8dda-437f-7d84-08d5745de76a" };

const char actnFrnt[] PROGMEM = { "\"action\":" }; //action front phrase, "action":
const char actnBack[] PROGMEM = { ",\"actionName" }; //action back phrase
const char minPhrs[] PROGMEM = { "\"" }; //miminal phrase, only " char.

const char code1[] PROGMEM = { "{\"dateTime\":\"" };
const char points[] PROGMEM = { ":" };
const char dash[] PROGMEM = { "-" };
const char space[] PROGMEM = { " " };
const char zero[] PROGMEM = { "0" };
const char code2[] PROGMEM = { "\",\"deviceId\":\"" };
const char code3[] PROGMEM = { "\"}\r\n" };

const char entry1[] PROGMEM = { "{\"measure\":{\"DateTime\":\"" };
const char entry2[] PROGMEM = { "\",\"SoilMoisture\":" };
const char entry3[] PROGMEM = { ",\"SoilDescription\":\"" };
const char entry4[] PROGMEM = { "\",\"AirHumidity\":" };
const char entry5[] PROGMEM = { ",\"AirTemperature\":" };
const char entry6[] PROGMEM = { ",\"LightState\":" };
const char entry7[] PROGMEM = { "},\"deviceId\":\"" };
const char entry8[] PROGMEM = { "\"}\r\n" };

WebController::WebController(Control *sys) {
	this->sys = sys;

	Serial.print(F("Setup LAN ... "));
	// give the Ethernet shield a second to initialize:
	if (Ethernet.begin(mac) == 0) {
		Serial.print(F("didn't begin. trying static ip.. "));
		Ethernet.begin(mac, ip);
	}

	printOk();
	delay(2500);
}

int WebController::buildEntryJson(Fragmentor fp, Fragmentor tp, EProcessor ep, FloatProcessor flp, DateTime now, int soilMoisture, char *soilDescription, float airHumidity, float airTemperature, bool lightState) {
	int len = 0;
	char ts[5];

	len += (this->*fp)(entry1);
	//Serial.println(len);
	int t = now.year();
	for (int i = 10; i <= 1000; i = i * 10) {
		if (t < i) len += (this->*fp)(zero);
	}
	//Serial.println(len);

	sprintf(ts, "%d", t);
	len += (this->*tp)(ts);
	len += (this->*fp)(dash);
	//Serial.println(len);

	for (int z = 0; z < 5; z++) {
		if (z == 0) t = now.month();
		else if (z == 1) t = now.day();
		else if (z == 2) t = now.hour();
		else if (z == 3) t = now.minute();
		else t = now.second();
		if (t < 10) (this->*fp)(zero);
		sprintf(ts, "%d", t);
		len += (this->*tp)(ts);
		if (z == 0) len += (this->*fp)(dash);
		else if (z == 1) len += (this->*fp)(space);
		else if (z == 2 || z == 3) len += (this->*fp)(points);
	}

	//Serial.println(len);
	len += (this->*fp)(entry2); //soil moisture
	//Serial.println(len);
	sprintf(ts, "%d", soilMoisture); //reuse ts char array
	len += (this->*tp)(ts);


	len += (this->*fp)(entry3); //soil description
	len += (this->*tp)((const char *)soilDescription);

	len += (this->*fp)(entry4); //air humidity
	len += (this->*flp)(airHumidity);

	len += (this->*fp)(entry5); // air temperature
	len += (this->*flp)(airTemperature);

	len += (this->*fp)(entry6); // light state
	if (lightState) len += (this->*tp)("true");
	else len += (this->*tp)("false");

	len += (this->*fp)(entry7); //device ID
	len += (this->*ep)(deviceIdPosition); //get device ID from EEPROM
	len += (this->*fp)(entry8);
	//Serial.println(len);

	//Serial.println(len);
	return len;
}

int WebController::processFloat(float value) {
	char front[4] = "";
	int len = 0;
	sprintf(front, "%d", (int)value);
	len += this->processValue(front);
	len += this->processValue(".");
	sprintf(front, "%d", round((value - (int)value) * 10));
	len += this->processValue(front);
	return len;
}
int WebController::floatLen(float value) {
	char front[4] = "";
	int len = 0;
	sprintf(front, "%d", (int)value);
	len += this->valueLen(front);
	len += this->valueLen(".");
	sprintf(front, "%d", round((value - (int)value) * 10));
	len += this->valueLen(front);
	return len;
}

void WebController::buildGetCodeRoute(char *route) {
	int index = 0;
	index += this->dumpFragment(codeGetFrnt, route);
	index += this->dumpEeprom(deviceIdPosition, route + index);
	index += this->dumpFragment(codeGetBack, route + index);
	route[index - 2] = '\r';
	route[index - 1] = '\n';
	route[index] = '\0';
}
void WebController::buildUpdateStateRoute(char *route) {
	int index = 0;
	index += this->dumpFragment(updateFrnt, route);
	index += this->dumpEeprom(deviceIdPosition, route + index);
	index += this->dumpFragment(updateBack, route + index);
	route[index - 2] = '\r';
	route[index - 1] = '\n';
	route[index] = '\0';
}

int WebController::buildUpdateJson(Fragmentor fp, Fragmentor tp, EProcessor ep) {
	int len = 0;
	len += (this->*fp)(updateJson);
	char ts[8] = "";
	sprintf(ts, "%d", sys->getRawData());
	len += (this->*tp)(ts);
	len += (this->*fp)(updateJson2);
	return len;
}

int WebController::buildCodeJson(Fragmentor fp, Fragmentor tp, EProcessor ep, DateTime now) {
	//fp is fragment processor, tp is time processor. We use delegates because we have to ensure that we
	//go through the same exact process for content-length and for writing out json.
	//now is needed to process code execution time.
	//ep is EEPROM processor, needed to get device Id out of EEPROM.
	int len = 0;
	char ts[5];

	len += (this->*fp)(code1);
	//Serial.println(len);
	int t = now.year();
	for (int i = 10; i <= 1000; i = i * 10) {
		if (t < i) len += (this->*fp)(zero);
	}
	//Serial.println(len);

	sprintf(ts, "%d", t);
	len += (this->*tp)(ts);
	len += (this->*fp)(dash);
	//Serial.println(len);

	for (int z = 0; z < 5; z++) {
		if (z == 0) t = now.month();
		else if (z == 1) t = now.day();
		else if (z == 2) t = now.hour();
		else if (z == 3) t = now.minute();
		else t = now.second();
		if (t < 10) (this->*fp)(zero);
		sprintf(ts, "%d", t);
		len += (this->*tp)(ts);
		if (z == 0) len += (this->*fp)(dash);
		else if (z == 1) len += (this->*fp)(space);
		else if (z == 2 || z == 3) len += (this->*fp)(points);
	}
	//Serial.println(len);
	len += (this->*fp)(code2);
	//Serial.println(len);
	len += (this->*ep)(deviceIdPosition); //the device Id address.
	//Serial.println(len);
	len += (this->*fp)(code3);
	//Serial.println(len);
	return len;
}


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

void WebController::printEeprom(int index) {
	char buff;
	EEPROM.get(index, buff);
	while (buff != '\0') {
		index++;
		Serial.print(buff);
		EEPROM.get(index, buff);
	}
}

int WebController::dumpEeprom(int index, char *dest) {
	char buff;
	int i = 0;
	EEPROM.get(index, buff);
	while (buff != '\0') {
		index++;
		*(dest + i) = buff;
		i++;
		EEPROM.get(index, buff);
	}
	return i;
}

int WebController::_eeprom(int index, bool doWrite) {
	char buff;
	EEPROM.get(index, buff);
	while (buff != '\0') {
		index++;
		if (doWrite) {
			client.print(buff);
			Serial.print(buff);
		}
		if (!((buff >= 65 && buff <= 90) || (buff >= 97 && buff <= 122) || buff == '-' || (buff >= 48 && buff <= 57))) Serial.print("WARNING");
		EEPROM.get(index, buff);
	}
	return index;
}
int WebController::processEeprom(int index) {
	return _eeprom(index, true) - index;
}

int WebController::eepromLen(int index) {
	return _eeprom(index, false) - index;
}

int WebController::processValue(const char *value) {
	client.print(value);
	Serial.print(value);
	return strlen(value);
}
int WebController::valueLen(const char *value) {
	return strlen(value);
}

void WebController::writeFragment(const char *fragment) { // fragment NEEDS string escape char.
	char buff = pgm_read_byte_near(fragment);
	int i = 0;
	while (buff != '\0') {
		i++;
		client.print(buff);
		Serial.print(buff);
		buff = pgm_read_byte_near(fragment + i);
	}
}

int WebController::dumpFragment(const char *fragment, char *dest) {
	char buff = pgm_read_byte_near(fragment);
	int i = 0;
	while (buff != '\0') {
		*(dest + i) = buff;
		i++;

		buff = pgm_read_byte_near(fragment + i);
	}
	return i;
}

int WebController::processFragment(const char *fragment) {
	char buff = pgm_read_byte_near(fragment);
	int i = 0;
	while (buff != '\0') {
		i++;
		client.print(buff);
		Serial.print(buff);
		buff = pgm_read_byte_near(fragment + i);
	}
	return i;
}

int WebController::fragmentLen(const char *fragment) {
	char buff = pgm_read_byte_near(fragment);
	int i = 0;
	while (buff != '\0') {
		i++;
		buff = pgm_read_byte_near(fragment + i);
	}
	return i;
}

int WebController::strLen(char *str) {
	char buff = *(str);
	int i = 0;
	int len = 0;
	while (buff != '\0') {
		if (buff != ' ') len += 1;
		i++;
		buff = *(str + i);
	}
	return len;
}

bool WebController::connectToServ() {
	int f = client.connect(server, 80);
	Serial.println(f);
	return f;
}

void WebController::writeConnection() {
	writeFragment(connection);
}

void WebController::writeJwtToken() {
	char buff;
	int i = 0;
	EEPROM.get(i, buff);
	while (buff != '\0') {
		i++;
		client.print(buff);
		Serial.print(buff);
		EEPROM.get(i, buff);
	}
}

void WebController::writeRoute(const char *route) {
	this->writeFragment(route);
}
void WebController::writeHost() {
	this->writeFragment(host);
}
void WebController::writeContentType() {
	this->writeFragment(contType);
}
void WebController::writeCacheType() {
	this->writeFragment(cacheCtrl);
}

void WebController::writeNewLine() {
	this->writeFragment(newLine);
}

void WebController::printMakeHttp() {
	Serial.println(F("Make a HTTP request ... "));
}
void WebController::printOk() {
	Serial.println(F("ok"));
}
void WebController::printConnFailed() {
	Serial.println(F("connection failed"));
}

void WebController::writeStaticRoute(const char *route) {
	client.print(route);
	Serial.print(route);
}

void WebController::getter(const char *route, bool staticPath) {
	if (connectToServ() > 0) {
		this->printMakeHttp();

		if (!staticPath) writeRoute(route);
		else writeStaticRoute(route);
		this->writeHost();
		this->writeJwtToken();
		this->writeConnection();
		this->writeCacheType();
		this->writeNewLine();
		this->printOk();
	}
	else {
		// kf you didn't get a connection to the server:
		this->printConnFailed();
	}
}
void WebController::writePostContent() {
	this->writeFragment(contLen);
	DateTime now = sys->rtc->now();

	int len = this->buildEntryJson(&fragmentLen, &valueLen, &eepromLen, &floatLen, sys->getTime(), sys->measure->soilMoisture,
		sys->logger->soilCondition(sys->measure->soilMoisture), sys->measure->humidity, sys->measure->temperature, sys->getLightingState());
	client.print(len);
	Serial.print(len);
	this->writeNewLine();
	this->writeNewLine();
	this->buildEntryJson(&processFragment, &processValue, &processEeprom, &processFloat, sys->getTime(), sys->measure->soilMoisture,
		sys->logger->soilCondition(sys->measure->soilMoisture), sys->measure->humidity, sys->measure->temperature, sys->getLightingState());
}

void WebController::writeCodeContent() {
	this->writeFragment(contLen);
	DateTime now = sys->rtc->now();
	int len = this->buildCodeJson(&fragmentLen, &valueLen, &eepromLen, now);
	client.print(len);
	Serial.print(len);
	this->writeNewLine();
	this->writeNewLine();
	this->buildCodeJson(&processFragment, &processValue, &processEeprom, now);
}
void WebController::writeUpdateContent() {
	this->writeFragment(contLen);
	int len = this->buildUpdateJson(&fragmentLen, &valueLen, &eepromLen);
	client.print(len);
	Serial.print(len);
	this->writeNewLine();
	this->writeNewLine();
	this->buildUpdateJson(&processFragment, &processValue, &processEeprom);
}

void WebController::writeLoginContent() {
	this->writeFragment(contLen);
	int len = this->fragmentLen(loginJson);
	client.print(len);
	Serial.print(len);
	this->writeNewLine();
	this->writeNewLine();
	this->processFragment(loginJson);
}

void WebController::writeContent(char *json) {
	this->writeFragment(contLen);
	client.print(strlen(json));
	Serial.print(strlen(json));
	this->writeNewLine();
	this->writeNewLine();
	client.print(json); //for now, later we'll be using buffer write.
	Serial.print(json);
}

//USE FOR POST & PUT
void WebController::poster(const char *route, bool writeToken, ContentWriter cw, bool staticPath) {
	if (connectToServ() > 0) {
		this->printMakeHttp();
		if (!staticPath) this->writeRoute(route);
		else this->writeStaticRoute(route);
		this->writeHost();
		if (writeToken) this->writeJwtToken();
		this->writeConnection();
		this->writeContentType();
		this->writeCacheType();
		(this->*cw)();
		this->writeNewLine();
		this->printOk();
	}
	else {
		// kf you didn't get a connection to the server:
		this->printConnFailed();
	}
}

void WebController::getMyId() { //working

	this->getter(nameCall, false);
	this->parseResponse(minPhrs, minPhrs, (int*)NULL, true, false, deviceIdPosition);
	this->printEeprom(deviceIdPosition);
}

int WebController::getCode() { //working
	char route[80] = "test";
	this->buildGetCodeRoute(route);
	int code = 500; //if 500 then some error happened. parser has to change this value.
	this->getter(route, true);
	/*
	   Http response parser. If it screws something up it shouldn't screw up EEPROM values.
	   If it does, it'll screw up deviceId storage (600++)
	   As long as code address exists parser won't write to EEPROM.
	*/
	this->parseResponse(actnFrnt, actnBack, &code, false, false, deviceIdPosition);
	//Serial.print("I parsed this: ");
	Serial.println(code);
	return code;
}

void WebController::login() { //working
	//COMMENT THIS OUT IF AUTHORIZATION: BEARER GETS DELETED.
	EEPROM.put(0, "Authorization: Bearer ");
	this->poster(loginPath, false, &(this->writeLoginContent), false);
	
	this->parseResponse(minPhrs, minPhrs, (int*)NULL, true, true, jwtPosition); //store token between " and " to EEPROM address 0+. Start from address 22 because Authorization_token takes that amount of space
}

void WebController::completeCode() { //working
	this->poster(codeCall, true, &(this->writeCodeContent), false);
	this->printResponse();
}

//TODO: fix

void WebController::postEntry() { //working

	this->poster(entryPost, true, &(this->writePostContent), false);
	this->printResponse();
}

void WebController::updateState() {
	char route[80] = "test";
	this->buildUpdateStateRoute(route);
	this->poster(route, true, &(this->writeUpdateContent), true);
	this->printResponse();
}

void WebController::printResponse() {
	char c;
	while (client.connected()) {
		while (client.available()) {
			c = client.read();
			Serial.print(c);
		}
	}
	Serial.println(F("disconnecting."));
	client.stop();
}


void WebController::parseResponse(const char *before, const char *after, int *action, bool eepromWrite, bool writeNewLine, int eepromIndex) {
	char actStrg[30] = "";
	bool done = false; //is search currently on
	bool found = false; //did we find the correct search phrase?
	byte index = 0; //index of current search within phrase
	byte searchSize = fragmentLen(before); // size of our phrase
	char c;
	int writeIndex = 0; //write head position.
	while (client.connected()) {
		while (client.available()) {
			c = client.read();
			if (done) {
				//keep empty.
			}
			else if (c == pgm_read_byte_near(before + index) && !found) {
				index++;
				if (index >= searchSize) {
					found = true;
					index = 0; // reuse the index for after.
					searchSize = fragmentLen(after);
				}
			}
			else if (c != pgm_read_byte_near(before + index) && !found) {
				index = 0; //reset the index because it's not what we were looking for.
			}
			else if (found) { // DON'T DELETE ELSE BECAUSE FOUND HAS TO START AT THE NEXT CYCLE.
				if (eepromWrite) {
					EEPROM.put(eepromIndex + writeIndex, c);
				}
				else {
					actStrg[writeIndex] = c;
				}
				writeIndex++;
				if (c != pgm_read_byte_near(after + index)) {
					index = 0;

				}
				else if (c == pgm_read_byte_near(after + index)) {
					index++;
					if (index >= searchSize) {
						done = 1;
						if (eepromWrite) {
							if (writeNewLine) EEPROM.put(eepromIndex + writeIndex - searchSize, "\r\n\0");
							else EEPROM.put(eepromIndex + writeIndex - searchSize, '\0');
						}
						else actStrg[writeIndex - searchSize] = '\0';
					}
				}
			}
			Serial.print(c);
		}
	}
	actStrg[29] = '\0';
	Serial.println(F("disconnecting."));
	client.stop();
	Serial.print("inside parse: ");
	Serial.println(actStrg);
	if (!eepromWrite) {
		Serial.println(strLen(actStrg));
		if (strLen(actStrg) != 0) {
			*action = atoi(actStrg);
		}
		else *action = 500;
	}
	Serial.print("which atoied to: ");
	Serial.println(*action);

	/*
	  EEPROM.get(eepromIndex, c);
	  index = 0;
	  while(c != '\0'){
	  index++;
	  Serial.print(c);
	  EEPROM.get(eepromIndex + index, c);
	  }
	*/
}

