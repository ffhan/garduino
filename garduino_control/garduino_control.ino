// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
//#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"
#include <SPI.h>
//#include "SdFat.h"
#include <stdlib.h>
//#include <IRremote.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

#define DHTTYPE DHT22

#define SensorPowerPin 2
#define IR_PIN 3
#define chipSelect 4
#define LOGGING_LED 5
#define lightControlPin 6
#define DHTPIN 7 // dht22 pin
#define HEATPIN 8
#define waterControlPin 9
//PINS 10,11,12
#define rGbControlPin A0
#define humiditySensorReadPin A1
#define RgbControlPin A2

typedef int (*Fragmentor) (char *fragment);
typedef int (*EProcessor) (int index);
typedef void (*ContentWriter)();
typedef int (*FloatProcessor) (float value);


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,0,1);
const char *server = "garduino20180215111317.azurewebsites.net";
//const char server[] PROGMEM = {"garduino20180215111317.azurewebsites.net"};

//const char token[] PROGMEM = {"Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJhZG1pbkBhZG1pbi5vcmciLCJqdGkiOiI1Mzk1ZDZlMS02MmMwLTRmNjMtYmMzOS1jOTEyZWFkMDhhYmIiLCJodHRwOi8vc2NoZW1hcy54bWxzb2FwLm9yZy93cy8yMDA1LzA1L2lkZW50aXR5L2NsYWltcy9uYW1laWRlbnRpZmllciI6ImI4M2VmZDU0LTVlN2ItNDIwMy04NTNjLWRjODk2MTczNTkxZSIsImV4cCI6MTUxOTAzNjY3NiwiaXNzIjoiaHR0cHM6Ly9sb2NhbGhvc3Q6NDQzOTUiLCJhdWQiOiJodHRwczovL2xvY2FsaG9zdDo0NDM5NSJ9.6MeaXtkiLi-qNvSYHSO9JMvbvo7FwpB8_9_pf9D4fFI\r\n"};
const char tokenHead[] PROGMEM = {"Authorization: Bearer "};
const char host[] PROGMEM = {"Host: garduino20180215111317.azurewebsites.net\r\n"};
const char codeCall[] PROGMEM = {"PUT /api/Code/latest HTTP/1.1\r\n"};
const char loginPath[] PROGMEM = {"POST /api/account HTTP/1.1\r\n"};
const char codeGetFrnt[] PROGMEM = {"GET /api/Code/latest/"};
const char codeGetBack[] PROGMEM = {" HTTP/1.1\r\n"};
const char nameCall[] PROGMEM = {"GET /api/Device/call/Garduino HTTP/1.1\r\n"};
const char entryPost[] PROGMEM = {"POST /api/entry HTTP/1.1\r\n"};

const char loginJson[] PROGMEM = {"{\"Email\":\"admin@admin.org\",\"Password\":\"Fran_97Sokol\",\"RememberMe\":\"false\"}\r\n"};

const char contType[] PROGMEM = {"Content-Type: application/json\r\n"};
const char cacheCtrl[] PROGMEM = {"Cache-Control: no-cache\r\n"};
const char newLine[] PROGMEM = {"\r\n"};
const char contLen[] PROGMEM = {"Content-Length: "};
const char connection[] PROGMEM = {"Connection: close\r\n"};
const char deviceId[] PROGMEM = {"c815dc1d-8dda-437f-7d84-08d5745de76a"};

const char actnFrnt[] PROGMEM = {"\"action\":"}; //action front phrase, "action":
const char actnBack[] PROGMEM = {",\"actionName"}; //action back phrase
const char minPhrs[] PROGMEM = {"\""}; //miminal phrase, only " char.

const char code1[] PROGMEM = {"{\"dateTime\":\""};
const char points[] PROGMEM = {":"};
const char dash[] PROGMEM = {"-"};
const char space[] PROGMEM = {" "};
const char zero[] PROGMEM = {"0"};
const char code2[] PROGMEM = {"\",\"deviceId\":\""};
const char code3[] PROGMEM = {"\"}\r\n"};

const char entry1[] PROGMEM = {"{\"measure\":{\"DateTime\":\""};
const char entry2[] PROGMEM = {"\",\"SoilMoisture\":"};
const char entry3[] PROGMEM = {",\"SoilDescription\":\""};
const char entry4[] PROGMEM = {"\",\"AirHumidity\":"};
const char entry5[] PROGMEM = {",\"AirTemperature\":"};
const char entry6[] PROGMEM = {",\"LightState\":"};
const char entry7[] PROGMEM = {"},\"deviceId\":\""};
const char entry8[] PROGMEM = {"\"}\r\n"};

/*
  const int lightControlPin = 6;
  const int SensorPowerPin = 2;
  const int humiditySensorReadPin = A1;
*/

//const char header[] = "DateTime stamp;Soil moisture;Soil description;Air humidity;Air temperature;Light On/Off;Soil moisture STD;Air humidity STD;Air temperature STD\0";

//#define FILE_BASE_NAME "logger" //log file base name.
//#define error(msg) sd.errorHalt(F(msg)) // error messages stored in flash.

//SDFat setup
//const uint8_t chipSelect = 4;
//SdFat sd; // File system object.
//SdFile file; // Log file.

RTC_DS3231 rtc;
DHT dht(DHTPIN, DHTTYPE);

//IRrecv irrecv(IR_PIN);
//decode_results results;

/*
   Because of the unfortunate decision to buy a data logging shield, I cannot use a DS3231 RTC on this project (currently).
   The shield already uses I2C connections SCL and SDA and because it occupies the same addresses as DS3231 module, I cannot multiplex them.
   The obvious solution is to buy a standalone SD card module without the RTC.
*/


//char fileName[13];

const int measureNumber = 5;
/*
class Remote {
  private : byte prefix = 0xFF;

  private : byte errorCode = 200; // change it in code enum too.
  
  private : typedef enum {LOCK = 0x22DD, LIGHT = 0x807F, LOGGING = 0xA05F,
                            HEATING = 0x906F, WATERING = 0x40BF, MEASURE = 0x609F,
                            FAN = 0x50AF, TIME = 0xE01F,
                            HOLD = 0xFFFF, TIMEOUT = 0x200 //TIMEOUT IS CODE 200, HTTP CODE FOR OK; HERE MEANING NOTHING SHOULD HAPPEN.
                           } code;
  private : code lastClick;
  private : byte holdLen = 0;
  private : unsigned long timer = 0;

  private : byte holdRange = 5; //in hold signals
  private : byte range = 150; //in milliseconds -> IF THE REMOTE DOESN'T RESPOND, READ OR IF ANY ERROR IS OCCURING THIS IS THE FIRST THING TO CHECK.
  //CHECK THE READING TIME.

  private : byte sendingCode = errorCode;

  public : byte getInstruction(){
    return sendingCode;
  }
  public : byte getErrorCode(){
    return errorCode;
  }
  
  public : void readRemote(){
    long mask = 0xFFFF;
    long prefixMask = 0xFF0000;
    if (irrecv.decode(&results))
      {
        //Serial.println(results.value, HEX);
        if((results.value & prefixMask) >> 16 != prefix){
          Serial.println(F("Wrong remote!"));
        }
        else{
          switch((code) mask & results.value){
            case HOLD:
            holdLen++;
            //Serial.println(millis() - timer);
            //Serial.println(holdLen);
            break;
      
            case TIMEOUT:
            return;
      
            default:
            lastClick = (code) mask & results.value;
            Serial.println(lastClick, HEX);
            holdLen = 0;
            break;
          }
          timer = millis();
        }
        
        //printState();
        irrecv.resume(); // Receive the next value
      }
  }
  
  private : bool isLongPress(){
    if(holdLen > holdRange) return true;
    return false;
  }

  public : void onTick(){
    if(lastClick == TIMEOUT) sendingCode = errorCode; // if nothing is clicked.
    if(millis() - timer >= range){
      byte mode = errorCode;
      switch(lastClick){
        case LOCK:
        mode = 0;
        break;

        case LIGHT:
        if(isLongPress()) mode = 4;
        else mode = 1;
        break;

        case LOGGING:
        mode = 2;
        break;

        case HEATING:
        if(isLongPress()) mode = 6;
        else mode = 3;
        break;

        case WATERING:
        if(isLongPress()) mode = 10;
        else mode = 7;
        break;
        
        case MEASURE:
        mode = 5;
        break;

        case FAN:
        if(isLongPress()) mode = 11;
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
};
*/
class Measuring {
    public : int soilMoisture;
    public : int light;
    public : float temperature;
    public : float humidity;

  public : Measuring() {
      soilMoisture = 0;
      light = 0;
      temperature = 0;
      humidity = 0;
    }

  public : void measure() {
      digitalWrite(SensorPowerPin, HIGH); // Powers up sensors and reads the data.

      delay(1000);

      soilMoisture = analogRead(humiditySensorReadPin);
      humidity = dht.readHumidity();
      temperature = dht.readTemperature();

      digitalWrite(SensorPowerPin, LOW); // Turn the sensor power off.
    }

  public : int getSoilMoisture() {
      return soilMoisture;
    }
  public : float getTemperature() {
      return temperature;
    }
  public : float getHumidity() {
      return humidity;
    }
  public : int getLight() {
      return light;
    }
};

class Logging {
  //public : void initSD();
  public : void writeHeader();
  public : void printHeader();
  public : void logData(DateTime now, Measuring *measure);
  public : char *soilCondition(int soilValue);
  public : void printTime(DateTime now);
} logger;

int getFreeRam()
{
  extern int __heap_start, *__brkval;
  int v;

  v = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

  Serial.print("Free RAM = ");
  Serial.println(v, DEC);

  return v;
}

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

  private : const int _defaultData = 0x0057;
  private : int _data = 0x0057;
  private : int fanSpeed_bitsize = 3;

  private : typedef enum {LOCK = 0, LOGGING = 1, WRITTEN = 2, IS_INITIALISED = 3,
                            LIGHTING_STATE = 4, LIGHT_ADMIN = 5,
                            HEATING_STATE = 6, HEAT_ADMIN = 7,
                            WATERING_STATE = 8, WATERING_ADMIN = 9,
                            FAN_SPEED = 10, FAN_ADMIN = 13,
                            CODE_FETCH = 14, NET_RECONF = 15
                           } sector;

  public : Measuring measure = Measuring();
  //public : Remote remote = Remote();
  private : DateTime now;
  
  private : int readPosition(int posit) {
      int tmp = 1 << posit; //creates an integer with only one bit on the poisition posit.
      return (_data & tmp) / tmp;
    }

  private : int readSpeed(int init_position, int bitSize) {
      int fanSpeed = 0;
      for (int i = 0; i < bitSize; i++) {
        fanSpeed += readPosition(init_position + i) * pow(2, i);
      }
      return fanSpeed;
    }
  private : int readState(sector search) {
      switch (search) {
        case FAN_SPEED:
          return readSpeed(FAN_SPEED, fanSpeed_bitsize);

        default:
          return readPosition(search);
      }
    }

  private : void setPosition(int posit) {
      int tmp = 1 << posit;
      _data |= tmp;
    }

  private : void erasePosition(int posit) {
      unsigned int tmp = 0;
      tmp = --tmp ^ (unsigned int) (1 << posit);
      _data = _data & tmp;
    }

  private : void writePosition(int posit, bool value) {
      value ? setPosition(posit) : erasePosition(posit);
    }
  private : void writeSpeed(int posit, int bitSize, int value) {
      for (int i = 0; i < bitSize; i++) {
        writePosition(posit + i, (bool) value % 2);
        value /= 2;
      }
    }
  private : void writeState(sector search, int value) {
      switch (search) {
        case FAN_SPEED:
          writeSpeed(FAN_SPEED, fanSpeed_bitsize, value);
          break;
        default:
          writePosition(search, (bool) value);
          break;
      }
    }

  public : void printState() {
      unsigned int tmp = (unsigned int) _data;
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

  public : int getCodeFetch(){
    return readState(CODE_FETCH);
  }
  public : void setCodeFetch(int value){
    writeState(CODE_FETCH, value);
  }
  public : int getNetReconf(){
    return readState(NET_RECONF);
  }
  public : void setNetReconf(int value){
    writeState(NET_RECONF, value);
  }
  public : int getLock() {
      return readState(LOCK);
    }
  public : int getLogging() {
      return readState(LOGGING);
    }
  public : int getWritten() {
      return readState(WRITTEN);
    }
  public : int getIsInitialised() {
      return readState(IS_INITIALISED);
    }
  public : int getLightingState() {
      return readState(LIGHTING_STATE);
    }
  public : int getLightAdmin() {
      return readState(LIGHT_ADMIN);
    }
  public : int getHeatingState() {
      return readState(HEATING_STATE);
    }
  public : int getHeatAdmin() {
      return readState(HEAT_ADMIN);
    }
  public : int getWateringState() {
      return readState(WATERING_STATE);
    }
  public : int getWateringAdmin() {
      return readState(WATERING_ADMIN);
    }
  public : int getFanSpeed() {
      return readState(FAN_SPEED);
    }
  public : int getFanAdmin() {
      return readState(FAN_ADMIN);
    }

  public : void setLock(int value) {
      writeState(LOCK, value);
      if (value == 1) {
        setLightAdmin(0);
        setHeatAdmin(0);
        setWateringAdmin(0);
        setFanAdmin(0);
      }
    }
    /*
  public : void setLogging(int value) {
      writeState(LOGGING, value);
      if (value) digitalWrite(LOGGING_LED, HIGH);
      else digitalWrite(LOGGING_LED, LOW);
    }
    */
  public : void setWritten(int value) {
      writeState(WRITTEN, value);
    }
  public : void setIsInitialised(int value) {
      writeState(IS_INITIALISED, value);
    }
  public : void setLightingState(int value) {
      writeState(LIGHTING_STATE, value);
    }
  public : void setLightAdmin(int value) {
      writeState(LIGHT_ADMIN, value);
    }
  public : void setHeatingState(int value) {
      writeState(HEATING_STATE, value);
    }
  public : void setHeatAdmin(int value) {
      writeState(HEAT_ADMIN, value);
    }
  public : void setWateringState(int value) {
      writeState(WATERING_STATE, value);
    }
  public : void setWateringAdmin(int value) {
      writeState(WATERING_ADMIN, value);
    }
  public : void setFanSpeed(int value) {
      writeState(FAN_SPEED, value);
    }
  public : void setFanAdmin(int value) {
      writeState(FAN_ADMIN, value);
    }

  public : int getEnumSize() {
      return sizeof(sector) * 8;
    }

  public : void heatSwitch(int state) {
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

  public : void mainSwitch(int choice) {
      if (getLock()) {
        switch (choice) {
          case 200:
          getMyId();
          return;
          case 500:
          return;
          case 8:
            logger.printTime(now);
            break;
          case 5:
            if (getLogging()) logger.logData(now, &measure);
            else Serial.println(F("Didn't write, that's what you wanted, right?"));
            break;
          case 0:
            setLock(0);
            Serial.println(F("GLOBAL LOCK OFF."));
            return;
          default:
            Serial.println(F("ACCESS DENIED: GLOBAL LOCK ON."));
            return;
        }
        return;
      }
      switch (choice) {
        case 200:
        getMyId();
        return;
        case 500:
        return;
        /*
        case 12345: { // like SWI 12345
            file.close();
            Serial.println(F("Done"));
            SysCall::halt();
            return;
          }
          */
        case 1: // switch for lighting
          if (!getLightAdmin()) {
            Serial.println(F("Lighting access denied: Light admin mode OFF."));
            return;
          }
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
          /*
        case 2:
          if (getIsInitialised()) {
            setLogging(0);
            file.close();
            Serial.println(F("Logging mode OFF. \t You can safely remove SD card."));
            setIsInitialised(0);
          }
          else {
            logger.initSD();
            setLogging(1);
            Serial.println(F("Logging mode ON."));
          }
          return;
          */
        case 3:
          if (!getHeatAdmin()) {
            Serial.println(F("Heating access denied: Heat admin mode OFF."));
            return;
          }
          if (getHeatingState()) {
            Serial.println(F("Custom mode: heating OFF."));
            setHeatingState(0);
            heatSwitch(0);
            return;
          }
          Serial.println(F("Custom mode: heating ON."));
          setLightingState(1);
          heatSwitch(1);
          return;

        case 4:
          if (getLightAdmin()) {
            setLightAdmin(0);
            Serial.println(F("Light admin mode OFF."));
            return;
          }
          setLightAdmin(1);
          Serial.println(F("Light admin mode ON."));
          return;

        case 5: {
            if (getLogging()) logger.logData(now, &measure);
            else Serial.println(F("Didn't write, that's what you wanted, right?"));
            return;
          }

        case 6:
          if (getHeatAdmin()) {
            setHeatAdmin(0);
            Serial.println(F("Heat admin mode OFF."));
            return;
          }
          setHeatAdmin(1);
          Serial.println(F("Heat admin mode ON."));
          return;

        case 7:
          if (!getWateringAdmin()) {
            Serial.println(F("Watering access denied: Watering admin mode OFF."));
            return;
          }
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

        case 8:
          logger.printTime(now);
          return;

        case 9: // TODO: FAN SPEED SET
          return;

        case 10:
          if (getWateringAdmin()) {
            setWateringAdmin(0);
            Serial.println(F("Watering admin mode OFF."));
            return;
          }
          setWateringAdmin(1);
          Serial.println(F("Watering admin mode ON."));
          return;

        case 0:
          if (getLock()) {
            setLock(0);
            Serial.println(F("GLOBAL LOCK OFF."));
            return;
          }
          setLock(1);
          Serial.println(F("GLOBAL LOCK ON."));
          return;

        case 11: //TODO: FAN ADMIN
          return;

      }
    }

    public : DateTime getTime(){
    return now;  
  }
  public : void updateTime(){
    now = rtc.now();
}

public : void tick() {
    
    if (now.second() % 2 == 0 && (now.hour() % 1 == 0) && now.minute() % 1 == 0) {
      digitalWrite(LOGGING_LED, LOW); // ensure every 2 seconds is only mode colors.
      if (getLock()) {
        digitalWrite(RgbControlPin, HIGH);
        digitalWrite(rGbControlPin, LOW);
      } else {
        digitalWrite(rGbControlPin, HIGH);
        if (getLightAdmin() || getHeatAdmin() || getWateringAdmin() || getFanAdmin()) digitalWrite(RgbControlPin, HIGH);
        else digitalWrite(RgbControlPin, LOW);
      }
    } else {
      if(getLogging()) digitalWrite(LOGGING_LED, HIGH);
      else digitalWrite(LOGGING_LED, LOW);
      
      digitalWrite(RgbControlPin, LOW);
      digitalWrite(rGbControlPin, LOW);
    }
}

public : void autoLight() {
      if (!getLightAdmin()) {
        // if time is inside interval <8am,10pm> and light is not on (user has not turned it on) then:
        if (getDecimalTime(now) >= 8.0 && getDecimalTime(now) < 22.0) { //TODO: not constantly turning on
          digitalWrite(lightControlPin, LOW); // it's NC (normally closed), so LOW turns the light on.
          if (!getLightingState()) setLightingState(1);
        } else {
          digitalWrite(lightControlPin, HIGH); // turn the light off.
          if (getLightingState()) setLightingState(0);
        }
      }
}

  public : void autoLight(DateTime now) {
      if (!getLightAdmin()) {
        // if time is inside interval <8am,10pm> and light is not on (user has not turned it on) then:
        if (getDecimalTime(now) >= 8.0 && getDecimalTime(now) < 22.0) { //TODO: not constantly turning on
          digitalWrite(lightControlPin, LOW); // it's NC (normally closed), so LOW turns the light on.
          if (!getLightingState()) setLightingState(1);
        } else {
          digitalWrite(lightControlPin, HIGH); // turn the light off.
          if (getLightingState()) setLightingState(0);
        }
      }
    }
/*
  public : void getRemoteInstructions() {
    byte instr = remote.getInstruction();
    if(instr == remote.getErrorCode()) return;
    mainSwitch(remote.getInstruction());
}
*/
  public : void update(){

    updateTime();
    
    //remote.readRemote();
    //remote.onTick();
    //getRemoteInstructions();

    autoLight();
    tick();
    logControl();
    
  }
  public : void tick(DateTime now) {
      if (now.second() % 2 == 0 && (now.hour() % 1 == 0) && now.minute() % 1 == 0) {
        if (getLock()) {
          digitalWrite(RgbControlPin, HIGH);
          digitalWrite(rGbControlPin, LOW);
        } else {
          digitalWrite(rGbControlPin, HIGH);
          if (getLightAdmin() || getHeatAdmin() || getWateringAdmin() || getFanAdmin()) digitalWrite(RgbControlPin, HIGH);
          else digitalWrite(RgbControlPin, LOW);
        }
      } else {
        digitalWrite(RgbControlPin, LOW);
        digitalWrite(rGbControlPin, LOW);
      }
  }
/*
  public : void getRemoteInstructions(DateTime now) {
    byte instr = remote.getInstruction();
    if(instr == remote.getErrorCode()) return;
    mainSwitch(remote.getInstruction());
  }
*/
  public : void logControl(){
    if (now.second() == 0 && (now.hour() % 1 == 0) && now.minute() % 30 == 0) {
      setWritten(0);
    }
    if(now.second() % 5 == 0 && now.minute() % 1 == 0){
      setCodeFetch(1);
    }
    if(now.second() == 0 && now.minute() == 0 && now.hour() == 1){
      setNetReconf(1);
    }
    
    if(!getWritten()) {
  
        if (getLogging()) logger.logData(now, &measure);
        else Serial.println(F("Didn't write, that's what you wanted, right?"));
  
        setWritten(1);
    }
    if(getCodeFetch()){
      int code = getCode();
      mainSwitch(code);
      if(code != 500) completeCode();
      setCodeFetch(0);
    }
    if(getNetReconf()){
      byte response = Ethernet.maintain(); // renew DHCP lease
      switch(response){
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
      login(); // renew access token.
      setNetReconf(0);
    }
    
  }

  public : Control() {
  
  }
};

Control sys = Control();
/*
void Logging::initSD() {
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";

  // Find an unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }


  getFreeRam();
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }
  if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
    if (!file.open(fileName, O_WRITE | O_AT_END)) error("file.open");
    else Serial.println("File already existing, opened successfully.");
  }
  else {
    Serial.println("File didn't exist. Writing a header to it.");
    writeHeader(); // if file doesn't exist write a header to it.
  }
  Serial.print(F("Logging to: "));
  Serial.println(fileName);
  sys.setIsInitialised(1);
}
*/
void Logging::writeHeader() {

  //file.println(header);
  printHeader();
}
void Logging::printHeader() {
  //Serial.println(header);
}

void Logging::logData(DateTime now, Measuring *measure) {

  char onState[] = "On\0", offState[] = "Off\0";

  (*measure).measure();

  //getFreeRam();

/*
  Serial.print(now.year());
  Serial.print("/");
  Serial.print(now.month());
  Serial.print("/");
  Serial.print(now.day());
  Serial.print(" ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.print(";");
  Serial.print((*measure).getSoilMoisture());
  Serial.print(";");
  Serial.print(soilCondition((*measure).getSoilMoisture()));
  Serial.print(";");
  Serial.print((*measure).getHumidity());
  Serial.print(";");
  Serial.print((*measure).getTemperature());
  Serial.print(";");
  Serial.println(sys.getLightingState() ? onState : offState);*/
/*
  file.print(now.year());
  file.print("/");
  file.print(now.month());
  file.print("/");
  file.print(now.day());
  file.print(" ");
  file.print(now.hour());
  file.print(":");
  file.print(now.minute());
  file.print(":");
  file.print(now.second());
  file.print(";");
  file.print(measure.getSoilMoisture());
  file.print(";");
  file.print(soilCondition(measure.getSoilMoisture()));
  file.print(";");
  file.print(measure.getHumidity());
  file.print(";");
  file.print(measure.getTemperature());
  file.print(";");
  file.println(sys.getLightingState() ? onState : offState);
  
  Serial.print(F("Flushing... "));

  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) {
    error("write error");
  }

  Serial.println(F("done."));*/
  postEntry();
}

char *Logging::soilCondition(int soilValue) {
  // Returns a descriptive string of the humidity of the soil

  //char dry[] = "Dry", wet[] = "Wet", veryWet[] = "Very wet", outOfRange[] = "Out of range";

  int air = 531;
  int water = 256;
  int interval = (air - water) / 3;

  if (soilValue >= water && soilValue < (water + interval)) {
    return "Very Wet";
  } else if (soilValue >= (water + interval) && soilValue <= (air - interval)) {
    return "Wet";
  } else if (soilValue <= air && soilValue > (air - interval)) {
    return "Dry";
  } else return "Out of range";

  //return outOfRange;
}

void Logging::printTime(DateTime now) {
  Serial.print(now.year()); Serial.print(F("/")); Serial.print(now.month()); Serial.print(F("/"));
  Serial.print(now.day()); Serial.print(F(" ")); Serial.print(now.hour()); Serial.print(F(":"));
  Serial.print(now.minute()); Serial.print(F(":")); Serial.print(now.second()); Serial.println(F(";"));
}

double getDecimalTime(DateTime now) {
  return (double)now.hour() + ((double)now.minute()) / ((double)60.0) + ((double)now.second()) / ((double)3600.0);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
/*
  logger.initSD();
  Serial.println(F("Initialising SD card reader done."));
  */
  /*// Wait for USB Serial
    while (!Serial && millis() < 4000) {
    SysCall::yield();
    }*/

  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  Serial.print(F("Setup LAN ... "));
  // give the Ethernet shield a second to initialize:
  if(Ethernet.begin(mac) == 0){
    Serial.print(F("didn't begin. trying static ip.. "));
    Ethernet.begin(mac,ip);
  }
  printOk();
  delay(6000);

  Serial.print(F("Initialising DHT22 sensor...\t"));
  dht.begin();
  Serial.println(F("done."));

  Serial.print(F("Initialising IRremote sensor...\t"));
  //irrecv.enableIRIn();
  Serial.println(F("done."));

  delay(1000); // wait for console opening

  Serial.print(F("Initialising RTC communications...\t"));
  if (! rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  }
  Serial.println(F("done."));

  pinMode(lightControlPin, OUTPUT); // Control light control pin as output
  pinMode(SensorPowerPin, OUTPUT); // Control humidity sensor power as output
  pinMode(humiditySensorReadPin, INPUT); // Get data from humidity sensor
  pinMode(HEATPIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(LOGGING_LED, OUTPUT);
  pinMode(RgbControlPin, OUTPUT);
  pinMode(rGbControlPin, OUTPUT);

  digitalWrite(LOGGING_LED, HIGH);

  logger.printHeader();
  Serial.println(F("Setup complete!"));

}

void loop() {

  while (Serial.available())
  {
    int choice = Serial.parseInt();
    sys.mainSwitch(choice);
  }

  sys.update();
}

