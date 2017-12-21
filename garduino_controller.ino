// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"
#include <SPI.h>
#include "SdFat.h"
#include <stdlib.h>
#include <IRremote.h>

#define DHTPIN 7 // dht22 pin
#define DHTTYPE DHT22

#define HEATPIN 8
#define LOGGING_LED 5
#define IR_PIN 3
#define chipSelect 4
#define lightControlPin 6
#define SensorPowerPin 2
#define humiditySensorReadPin A1
#define waterControlPin 9

/*
  const int lightControlPin = 6;
  const int SensorPowerPin = 2;
  const int humiditySensorReadPin = A1;
*/

//const char header[] = "DateTime stamp;Soil moisture;Soil description;Air humidity;Air temperature;Light On/Off;Soil moisture STD;Air humidity STD;Air temperature STD\0";

#define FILE_BASE_NAME "logger" //log file base name.
#define error(msg) sd.errorHalt(F(msg)) // error messages stored in flash.

//SDFat setup
//const uint8_t chipSelect = 4;
SdFat sd; // File system object.
SdFile file; // Log file.

RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);

IRrecv irrecv(IR_PIN);
decode_results results;

/*
   Because of the unfortunate decision to buy a data logging shield, I cannot use a DS3231 RTC on this project (currently).
   The shield already uses I2C connections SCL and SDA and because it occupies the same addresses as DS3231 module, I cannot multiplex them.
   The obvious solution is to buy a standalone SD card module without the RTC.
*/


char fileName[13];

const int measureNumber = 5;

class Measuring {
    int soilMoisture;
    int light;
    float temperature;
    float humidity;

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
  public : void initSD();
  public : void writeHeader();
  public : void printHeader();
  public : void logData(DateTime now, Measuring measure);
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
       | 13  |   0   | FAN ADMIN | 1 - USER CONTROLS THE FAN, 0 - AUTO CONTROLLED FAN
    */

  private : const int _defaultData = 0x0057;
  private : int _data = 0x0057;
  private : int fanSpeed_bitsize = 3;

  private : typedef enum {LOCK = 0, LOGGING = 1, WRITTEN = 2, IS_INITIALISED = 3,
                            LIGHTING_STATE = 4, LIGHT_ADMIN = 5,
                            HEATING_STATE = 6, HEAT_ADMIN = 7,
                            WATERING_STATE = 8, WATERING_ADMIN = 9,
                            FAN_SPEED = 10, FAN_ADMIN = 13
                           } sector;

  public : Measuring measure = Measuring();

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
      if(value == 1){
        setLightAdmin(0);
        setHeatAdmin(0);
        setWateringAdmin(0);
        setFanAdmin(0);
      }
    }
  public : void setLogging(int value) {
      writeState(LOGGING, value);
    }
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

  public : void mainSwitch(int choice, DateTime now) {
      if (getLock()) {
        switch (choice) {
          case 8:
            logger.printTime(now);
            break;
          case 5:
            if (getLogging()) logger.logData(now, measure);
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
        case 12345: { // like SWI 12345
            file.close();
            Serial.println(F("Done"));
            SysCall::halt();
            return;
          }

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
            if (getLogging()) logger.logData(now, measure);
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

  public : void readRemote(DateTime now) {
      if (irrecv.decode(&results))
      {
        //Serial.println(results.value, HEX);
        switch (results.value) {
          case 0xFF00FF:
            Serial.println("POWER ON/OFF");
            mainSwitch(100, now);
            break;
          case 0xFF10EF:
            Serial.println("EJECT");
            mainSwitch(110, now);
            break;
          case 0xFF9867:
            Serial.println("OK");
            printState();
            mainSwitch(120, now);
            break;
          case 0xFFCA35:
            Serial.println("UP");
            break;
          case 0xFF18E7:
            Serial.println("DOWN");
            break;
          case 0xFFF20D:
            Serial.println("LEFT");
            break;
          case 0xFFEA15:
            Serial.println("RIGHT");
            break;
          case 0xFF807F:
            Serial.println("1");
            mainSwitch(1, now);
            break;
          case 0xFFA05F:
            Serial.println("2");
            mainSwitch(2, now);
            break;
          case 0xFF906F:
            Serial.println("3");
            mainSwitch(3, now);
            break;
          case 0xFF40BF:
            Serial.println("4");
            mainSwitch(4, now);
            break;
          case 0xFF609F:
            Serial.println("5");
            mainSwitch(5, now);
            break;
          case 0xFF50AF:
            Serial.println("6");
            mainSwitch(6, now);
            break;
          case 0xFFC03F:
            Serial.println("7");
            mainSwitch(7, now);
            break;
          case 0xFFE01F:
            Serial.println("8");
            mainSwitch(8, now);
            break;
          case 0xFFD02F:
            Serial.println("9");
            mainSwitch(9, now);
            break;
          case 0xFF22DD:
            Serial.println("0");
            mainSwitch(0, now);
            break;
          case 0xFFFFFF:
            break;
        }
        //printState();
        irrecv.resume(); // Receive the next value
      }
    }

    public : void autoLight(DateTime now){
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

    public : Control() {
    }
};

Control sys = Control();

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
void Logging::writeHeader() {

  //file.println(header);
  printHeader();
}
void Logging::printHeader() {
  //Serial.println(header);
}

void Logging::logData(DateTime now, Measuring measure) {

  char onState[] = "On\0", offState[] = "Off\0";

  measure.measure();

  //getFreeRam();

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
  Serial.print(measure.getSoilMoisture());
  Serial.print(";");
  Serial.print(soilCondition(measure.getSoilMoisture()));
  Serial.print(";");
  Serial.print(measure.getHumidity());
  Serial.print(";");
  Serial.print(measure.getTemperature());
  Serial.print(";");
  Serial.println(sys.getLightingState() ? onState : offState);

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

  Serial.println(F("done."));
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

  Serial.begin(1200);

  logger.initSD();
  Serial.println(F("Initialising SD card reader done."));
  /*// Wait for USB Serial
    while (!Serial && millis() < 4000) {
    SysCall::yield();
    }*/

  Serial.print(F("Initialising DHT22 sensor...\t"));
  dht.begin();
  Serial.println(F("done."));

  Serial.print(F("Initialising IRremote sensor...\t"));
  irrecv.enableIRIn();
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

  digitalWrite(LOGGING_LED, HIGH);

  logger.printHeader();
  Serial.println(F("Setup complete!"));

}

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now(); // read time.

  while (Serial.available())
  {
    int choice = Serial.parseInt();
    sys.mainSwitch(choice, now);
  }

  if (sys.getLogging()) digitalWrite(LOGGING_LED, HIGH);
  else digitalWrite(LOGGING_LED, LOW);


  sys.readRemote(now);

  /*
     Ideally the plants should get 14-16 hours of light. We are going to go with 14 hours of light for testing.
     That means 8am - 10pm.
  */

  sys.autoLight(now);

  /**
     Humidity measurement - later we're going to split off measuring soil moisture and other measurements
     beacause some are more critical than others (and should be measured more often).

     We want to measure moisture every 6 hours so that we have 4 measurements per day.
     Considering moisture is not going to change so radically this is just fine.
     Measurement hours: 1am, 7am, 13h, 19h
  */

  if (now.second() == 0 && (now.hour() % 1 == 0) && now.minute() % 30 == 0) {
    if (!sys.getWritten()) {

      if (sys.getLogging()) logger.logData(now, sys.measure);
      else Serial.println(F("Didn't write, that's what you wanted, right?"));

      sys.setWritten(1);
    }
  } else {
    sys.setWritten(0);
  }
}
