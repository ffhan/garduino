#include "Control.h"
#include "WebController.h"
#include "Measuring.h"
#include "remote.h"
#include "Logging.h"
#include "ActionBinaryTree.h"
#include "Action.h"
#include "pins.h"

extern double getDecimalTime(DateTime now);

Control::Control() {
	Serial.print(F("Initialising RTC communications...\t"));
	if (!rtc->begin()) {
		Serial.println(F("Couldn't find RTC"));
		while (1);
	}

	Serial.println(F("done."));

	measure = new Measuring();
	remote = new Remote();
	logger = new Logging(this);
	actions = new ActionBinaryTree();

	globalLockPromise = new PromisePack(this, &Control::getLock, "ACCESS DENIED: GLOBAL LOCK ON.", true);
	lightAdminPromise = new PromisePack(this, &Control::getLightAdmin, "Light access denied: Light admin OFF.");
	heatAdminPromise = new PromisePack(this, &Control::getHeatAdmin, "Heating access denied: Heating admin OFF.");
	loggingPromise = new PromisePack(this, &Control::getLogging, "Logging disabled.");
	wateringAdminPromise = new PromisePack(this, &Control::getWateringAdmin, "Watering access denied: Watering admin OFF.");
	fanAdminPromise = new PromisePack(this, &Control::getFanAdmin, "Fan access denied: Fan admin OFF.");

	globalLockAction = new Action(this, 0, &Control::globalLockEvent);

	lightAdminAction = new Action(this, 4, &Control::lightAdminEvent);
	lightAdminAction->addPromises(globalLockPromise);

	lightStateAction = new Action(this, 1, &Control::lightStateEvent);
	lightStateAction->addPromises(globalLockPromise, lightAdminPromise);

	printTimeAction = new Action(this, 8, &Control::printTimeEvent);

	heatAdminAction = new Action(this, 6, &Control::heatAdminEvent);
	heatAdminAction->addPromises(globalLockPromise);
	heatStateAction = new Action(this, 3, &Control::heatStateEvent);
	heatStateAction->addPromises(globalLockPromise, heatAdminPromise);

	wateringAdminAction = new Action(this, 10, &Control::wateringAdminEvent);
	wateringAdminAction->addPromises(globalLockPromise);
	wateringStateAction = new Action(this, 7, &Control::wateringStateEvent);
	wateringStateAction->addPromises(globalLockPromise, wateringAdminPromise);

	fanAdminAction = new Action(this, 11, &Control::fanAdminEvent);
	fanAdminAction->addPromises(globalLockPromise);

	fanSpeedAction = new Action(this, 9, &Control::fanStateEvent);
	fanSpeedAction->addPromises(globalLockPromise, fanAdminPromise);

	actions->insert(
		globalLockAction, lightAdminAction, lightStateAction,
		heatAdminAction, heatStateAction, fanAdminAction, fanSpeedAction);

	web = new WebController(this);
}

int Control::readPosition(int posit) {
	int tmp = 1 << posit; //creates an integer with only one bit on the position posit.
	return (_data & tmp) / tmp;
}

int Control::readSpeed(int init_position, int bitSize) {
	int fanSpeed = 0;
	byte expo = 1;
	for (int i = 0; i < bitSize; i++) {
		fanSpeed += readPosition(init_position + i) * expo;
		expo *= 2;
	}
	return fanSpeed;
}
int Control::readState(sector search) {
	switch (search) {
	case FAN_SPEED:
		return readSpeed(FAN_SPEED, fanSpeed_bitsize);

	default:
		return readPosition(search);
	}
}

int Control::getRawData() {
	int t;
	t = _data;
	return t;
}

void Control::setPosition(int posit) {
	int tmp = 1 << posit;
	_data |= tmp;
}

void Control::erasePosition(int posit) {
	unsigned int tmp = 0;
	tmp = --tmp ^ (unsigned int)(1 << posit);
	_data = _data & tmp;
}

void Control::writePosition(int posit, bool value) {
	value ? setPosition(posit) : erasePosition(posit);
}
void Control::writeSpeed(int posit, int bitSize, int value) {
	byte mask = 1;
	for (int i = 0; i < bitSize; i++) {
		writePosition(posit + i, (bool)(value & mask));
		value = value >> 1;
	}
}
void Control::writeState(sector search, int value) {
	switch (search) {
	case FAN_SPEED:
		writeSpeed(FAN_SPEED, fanSpeed_bitsize, value);
		break;
	default:
		writePosition(search, (bool)value);
		break;
	}
}

void Control::printState() {
	unsigned int tmp = (unsigned int)_data;
	unsigned y;
	for (int i = 0; i < sizeof(int) * 8; i++) {
		y = tmp;
		tmp = (tmp << 1) >> 1;
		if (i > 0 && i % 4 == 0)Serial.print(F("|"));
		Serial.print((y - tmp) != 0);
		tmp = tmp << 1;
	}
	Serial.println();
}

int Control::getCodeFetch() {
	return readState(CODE_FETCH);
}
void Control::setCodeFetch(int value) {
	writeState(CODE_FETCH, value);
}
int Control::getNetReconf() {
	return readState(NET_RECONF);
}
void Control::setNetReconf(int value) {
	writeState(NET_RECONF, value);
}
int Control::getLock() {
	return readState(LOCK);
}
int Control::getLogging() {
	return readState(LOGGING);
}
int Control::getWritten() {
	return readState(WRITTEN);
}
int Control::getIsInitialised() {
	return readState(IS_INITIALISED);
}
int Control::getLightingState() {
	return readState(LIGHTING_STATE);
}
int Control::getLightAdmin() {
	return readState(LIGHT_ADMIN);
}
int Control::getHeatingState() {
	return readState(HEATING_STATE);
}
int Control::getHeatAdmin() {
	return readState(HEAT_ADMIN);
}
int Control::getWateringState() {
	return readState(WATERING_STATE);
}
int Control::getWateringAdmin() {
	return readState(WATERING_ADMIN);
}
int Control::getFanSpeed() {
	return readState(FAN_SPEED);
}
int Control::getFanAdmin() {
	return readState(FAN_ADMIN);
}

void Control::setLock(int value) {
	writeState(LOCK, value);
	if (value == 1) {
		setLightAdmin(0);
		setHeatAdmin(0);
		setWateringAdmin(0);
		setFanAdmin(0);
	}
}
void Control::setLogging(int value) {
	writeState(LOGGING, value);
	if (value) digitalWrite(LOGGING_LED, HIGH);
	else digitalWrite(LOGGING_LED, LOW);
}

void Control::setWritten(int value) {
	writeState(WRITTEN, value);
}
void Control::setIsInitialised(int value) {
	writeState(IS_INITIALISED, value);
}
void Control::setLightingState(int value) {
	writeState(LIGHTING_STATE, value);
}
void Control::setLightAdmin(int value) {
	writeState(LIGHT_ADMIN, value);
}
void Control::setHeatingState(int value) {
	writeState(HEATING_STATE, value);
}
void Control::setHeatAdmin(int value) {
	writeState(HEAT_ADMIN, value);
}
void Control::setWateringState(int value) {
	writeState(WATERING_STATE, value);
}
void Control::setWateringAdmin(int value) {
	writeState(WATERING_ADMIN, value);
}
void Control::setFanSpeed(int value) {
	Serial.println(getFanSpeed());
	writeState(FAN_SPEED, value);
	Serial.println(getFanSpeed());
}
void Control::setFanAdmin(int value) {
	writeState(FAN_ADMIN, value);
}

int Control::getEnumSize() {
	return sizeof(sector) * 8;
}

void Control::heatSwitch(int state) {
	switch (state) {
	case 0:
	{
		digitalWrite(HEATPIN, LOW);
		setHeatingState(0);
		break;
	}
	case 1: {
		digitalWrite(HEATPIN, HIGH);
		setHeatingState(1);
		break;
	}
	}
}

void Control::mainSwitch(int choice) {
	Action *action = actions->retrieve(choice);
	if (action) action->execute();
	/*
	  if (getLock()) {
	  switch (choice) {

		case 5:
		  if (getLogging()) logger->logData(now, measure);
		  else Serial.println(F("Didn't write, that's what you wanted, right?"));
		case 0:

		  return;
		default:
		  Serial.println(F("ACCESS DENIED: GLOBAL LOCK ON."));
		  return;
	  }
	  return;
	  }
	  switch (choice) {
	  case 200:
		web->getMyId();
		return;
	  case 420:
		renewNetwork(true);
	  case 500:
		return;
	  /*
		case 12345: { // like SWI 12345
		  file.close();
		  Serial.println(F("Done"));
		  SysCall::halt();
		  return;
		}

	  case 1: // switch for lighting


	  case 3:
		if (!getHeatAdmin()) {
		  Serial.println(F("Heating access denied: Heat admin mode OFF."));
		  return;
		}


	  case 4:


	  case 5: {
		  if (getLogging()) logger->logData(now, measure);
		  else Serial.println(F("Didn't write, that's what you wanted, right?"));
		  return;
		}

	  case 6:


	  case 7:
		if (!getWateringAdmin()) {
		  Serial.println(F("Watering access denied: Watering admin mode OFF."));
		  return;
		}


	  case 8:

		return;

	  case 9: // TODO: FAN SPEED SET
		Serial.println("This implementation is wrong because it doesn't take fan admin into account");
		return;

	  case 10:


	  case 0:


	  case 11: //TODO: FAN ADMIN
		if (getFanAdmin()) {
		  setFanAdmin(0);
		  Serial.println(F("Fan admin mode OFF."));
		  return;
		}
		setFanAdmin(1);
		Serial.println(F("Fan admin mode ON."));
		return;

	  }
	*/
}

void Control::globalLockEvent() {
	if (getLock()) {
		setLock(0);
		Serial.println(F("GLOBAL LOCK OFF."));
		return;
	}
	setLock(1);
	Serial.println(F("GLOBAL LOCK ON."));
	return;
}

void Control::lightAdminEvent() {
	if (getLightAdmin()) {
		setLightAdmin(0);
		Serial.println(F("Light admin mode OFF."));
		return;
	}
	setLightAdmin(1);
	Serial.println(F("Light admin mode ON."));
	return;
}

void Control::lightStateEvent() {
	if (getLightingState()) {
		Serial.println(F("Custom mode: light OFF."));
		setLightingState(0);
		digitalWrite(lightControlPin, HIGH);
		return;
	}
	Serial.println(F("Custom mode: light ON."));
	setLightingState(1);
	digitalWrite(lightControlPin, LOW);
	return;
}

void Control::heatAdminEvent() {
	if (getHeatAdmin()) {
		setHeatAdmin(0);
		Serial.println(F("Heat admin mode OFF."));
		return;
	}
	setHeatAdmin(1);
	Serial.println(F("Heat admin mode ON."));
	return;
}
void Control::heatStateEvent() {
	if (getHeatingState()) {
		Serial.println(F("Custom mode: heating OFF."));
		//setHeatingState(0);
		heatSwitch(0);
		return;
	}
	Serial.println(F("Custom mode: heating ON."));
	//setLightingState(1);
	heatSwitch(1);
	return;
}

void Control::loggingEvent() {
	/*
		if (getIsInitialised()) {
		setLogging(0);
		file.close();
		Serial.println(F("Logging mode OFF. \t You can safely remove SD card."));
		setIsInitialised(0);
		}
		else {
		logger->initSD();
		setLogging(1);
		Serial.println(F("Logging mode ON."));
		}
		return;
	*/
}

void Control::wateringAdminEvent() {
	if (getWateringAdmin()) {
		setWateringAdmin(0);
		Serial.println(F("Watering admin mode OFF."));
		return;
	}
	setWateringAdmin(1);
	Serial.println(F("Watering admin mode ON."));
	return;
}
void Control::wateringStateEvent() {
	if (getWateringState()) {
		Serial.println(F("Custom mode: watering OFF."));
		setWateringState(0);
		digitalWrite(waterControlPin, LOW);
		return;
	}
	Serial.println(F("Custom mode: watering ON."));
	setWateringState(1);
	digitalWrite(waterControlPin, HIGH);
	return;
}

void Control::fanAdminEvent() {
	if (getFanAdmin()) {
		setFanAdmin(0);
		Serial.println(F("Fan admin mode OFF."));
		return;
	}
	setFanAdmin(1);
	Serial.println(F("Fan admin mode ON."));
	return;
}
void Control::fanStateEvent(int value) {
	setFanSpeed(value);
	return;
}

void Control::measureEvent() {
	logger->logData(now, measure);
}

void Control::printTimeEvent() {
	logger->printTime(now);
}
void Control::setTimeEvent() {
	Serial.println(F("unimplemented."));
}

DateTime Control::getTime() {
	return now;
}
void Control::updateTime() {
	now = rtc->now();
}

void Control::tick() {

	if (now.second() % 2 == 0 && (now.hour() % 1 == 0) && now.minute() % 1 == 0) {
		digitalWrite(LOGGING_LED, LOW); // ensure every 2 seconds is only mode colors.
		if (getLock()) {
			digitalWrite(RgbControlPin, HIGH);
			digitalWrite(rGbControlPin, LOW);
		}
		else {
			digitalWrite(rGbControlPin, HIGH);
			if (getLightAdmin() || getHeatAdmin() || getWateringAdmin() || getFanAdmin()) digitalWrite(RgbControlPin, HIGH);
			else digitalWrite(RgbControlPin, LOW);
		}
	}
	else {
		if (getLogging()) digitalWrite(LOGGING_LED, HIGH);
		else digitalWrite(LOGGING_LED, LOW);

		digitalWrite(RgbControlPin, LOW);
		digitalWrite(rGbControlPin, LOW);
	}
}

void Control::autoLight() {
	if (!getLightAdmin()) {
		// if time is inside interval <8am,10pm> and light is not on (user has not turned it on) then:
		if (getDecimalTime(now) >= 8.0 && getDecimalTime(now) < 22.0) { //TODO: not constantly turning on
			digitalWrite(lightControlPin, LOW); // it's NC (normally closed), so LOW turns the light on.
			if (!getLightingState()) setLightingState(1);
		}
		else {
			digitalWrite(lightControlPin, HIGH); // turn the light off.
			if (getLightingState()) setLightingState(0);
		}
	}
}

void Control::autoLight(DateTime now) {
	if (!getLightAdmin()) {
		// if time is inside interval <8am,10pm> and light is not on (user has not turned it on) then:
		if (getDecimalTime(now) >= 8.0 && getDecimalTime(now) < 22.0) { //TODO: not constantly turning on
			digitalWrite(lightControlPin, LOW); // it's NC (normally closed), so LOW turns the light on.
			if (!getLightingState()) setLightingState(1);
		}
		else {
			digitalWrite(lightControlPin, HIGH); // turn the light off.
			if (getLightingState()) setLightingState(0);
		}
	}
}

void Control::getRemoteInstructions() {
	byte instr = remote->getInstruction();
	if (instr == remote->getErrorCode()) return;
	mainSwitch(remote->getInstruction());
}

void Control::update() {

	updateTime();

	remote->readRemote();
	remote->onTick();
	getRemoteInstructions();

	autoLight();
	tick();
	logControl();

}
void Control::tick(DateTime now) {
	if (now.second() % 2 == 0 && (now.hour() % 1 == 0) && now.minute() % 1 == 0) {
		if (getLock()) {
			digitalWrite(RgbControlPin, HIGH);
			digitalWrite(rGbControlPin, LOW);
		}
		else {
			digitalWrite(rGbControlPin, HIGH);
			if (getLightAdmin() || getHeatAdmin() || getWateringAdmin() || getFanAdmin()) digitalWrite(RgbControlPin, HIGH);
			else digitalWrite(RgbControlPin, LOW);
		}
	}
	else {
		digitalWrite(RgbControlPin, LOW);
		digitalWrite(rGbControlPin, LOW);
	}
}
void Control::renewNetwork(bool rewriteDevice) {
	byte response = Ethernet.maintain(); // renew DHCP lease
	switch (response) {
	case 0:
		Serial.println(F("DHCP renewal: nothing happened"));
		break;
	case 1:
		Serial.println(F("DHCP renewal: failed"));
		break;
	case 2:
		Serial.println(F("DHCP renewal: success"));
		break;
	case 3:
		Serial.println(F("DHCP rebind: failed"));
		break;
	case 4:
		Serial.println(F("DHCP rebind: success"));
		break;
	}
	web->login(); // renew access token.
	if (rewriteDevice) web->getMyId();
	setNetReconf(0);
}

void Control::getRemoteInstructions(DateTime now) {
	byte instr = remote->getInstruction();
	if (instr == remote->getErrorCode()) return;
	mainSwitch(remote->getInstruction());
}

void Control::logControl() {
	/*
	if (now.second() == 0 && (now.hour() % 1 == 0) && ((now.minute() % 30 == 0) || now.minute() == 0)) {
	  setWritten(0);
	}
	if (now.second() == 30 && now.minute() % 1 == 0) {
	  setCodeFetch(1);
	}
	if (now.second() == 0 && now.minute() == 0 && now.hour() == 1) {
	  setNetReconf(1);
	}
	*/

	if (!getWritten()) {

		if (getLogging()) logger->logData(now, measure);
		else Serial.println(F("Didn't write, that's what you wanted, right?"));

		setWritten(1);
	}

	if (getCodeFetch()) {
		int code = web->getCode();
		mainSwitch(code);
		if (code != 500) web->completeCode();
		setCodeFetch(0);
		web->updateState();
	}

	if (getNetReconf()) {
		renewNetwork(false);
	}

}

void Control::empty() {
	//Serial.println("TEST PRINT");
}
void Control::test() {
	Serial.println("TESTING");
}
