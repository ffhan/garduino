// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"
#include <SPI.h>
#include "SdFat.h"

#define DHTPIN 7 // dht22 pin
#define DHTTYPE DHT22

#define HEATPIN 8

#define FILE_BASE_NAME "logger" //log file base name.
#define error(msg) sd.errorHalt(F(msg)) // error messages stored in flash.

//SDFat setup
const uint8_t chipSelect = 4;
SdFat sd; // File system object.
SdFile file; // Log file.


/*
 * Because of the unfortunate decision to buy a data logging shield, I cannot use a DS3231 RTC on this project (currently).
 * The shield already uses I2C connections SCL and SDA and because it occupies the same addresses as DS3231 module, I cannot multiplex them.
 * The obvious solution is to buy a standalone SD card module without the RTC.
 */
RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);

char fileName[13];

const String header = "DateTime stamp;Soil moisture;Soil description;Air humidity;Air temperature;Light On/Off";
const int lightControlPin = 6;
const int SensorPowerPin = 2;
const int humiditySensorReadPin = A1;

// Humidity sensor calibrated characteristics
const int air = 531;
const int water = 256;
int interval = (air - water) / 3;
int soilValue = 0;

bool lightCondition = true;
int lightAdmin = 0; // 0 - turn admin privilege off, 1 - turn admin privilege on -> light off, 2 - turn admin privilege on -> light on
bool written = false;
bool logging = true;
bool isInitialised = false;

typedef struct{
  int soilMoisture;
  int light;
  float temperature;
  float humidity;
} measures;

void setup () {

  Serial.begin(9600);

  initSD();
  /*// Wait for USB Serial 
  while (!Serial && millis() < 4000) {
    SysCall::yield();
  }*/
  dht.begin();

  delay(1000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  /*
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  DateTime nowtime = rtc.now();
  DateTime future(nowtime.unixtime() + 12);
  rtc.adjust(future);*/
  /*DS1307 doesn't have the lost power option. TODO: enable time setting,
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }*/
  
  pinMode(lightControlPin, OUTPUT); // Control light control pin as output
  pinMode(SensorPowerPin, OUTPUT); // Control humidity sensor power as output
  pinMode(humiditySensorReadPin, INPUT); // Get data from humidity sensor
  pinMode(HEATPIN, OUTPUT);
  
}

void initSD(){
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";

  // Find an unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }

  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }
  if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
    if(!file.open(fileName, O_WRITE | O_AT_END)) error("file.open");
    else Serial.println("File already existing, opened successfully.");
  } 
  else{
    Serial.println("File didn't exist. Writing a header to it.");
    writeHeader(); // if file doesn't exist write a header to it.
  }
  Serial.print(F("Logging to: "));
  Serial.println(fileName);
  printHeader();
  isInitialised = true;
}

void writeHeader(){
  file.println(header);
  printHeader();
}
void logData(DateTime now){
  measures track;
  measure(&track);
  String lightCond = (lightCondition ? "On" : "Off");
  /*
  line += yr + "/" + mnth + "/" + dy + " ";
  line += hr + ":" + mnt + ":" + sc + ";";
  line += moisture + ";" + condition + ";" + humidity + ";";
  line += tmp + ";";
  line += lightCond;
  stamp;Soil moisture;Soil description;Air humidity;Air temperature;Light On/Off
  */
  
  
  //sprintf(line, "%d/%d/%d %d:%d:%d;%d;%s;%f;%f;%s", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), track.soilMoisture, soilCondition(track.soilMoisture), track.humidity, track.temperature, lightCond);
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
  Serial.print(track.soilMoisture);
  Serial.print(";");
  Serial.print(soilCondition(track.soilMoisture));
  Serial.print(";");
  Serial.print(track.humidity);
  Serial.print(";");
  Serial.print(track.temperature);
  Serial.print(";");
  Serial.println(lightCond);

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
  file.print(track.soilMoisture);
  file.print(";");
  file.print(soilCondition(track.soilMoisture));
  file.print(";");
  file.print(track.humidity);
  file.print(";");
  file.print(track.temperature);
  file.print(";");
  file.println(lightCond);

  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) {
    error("write error");
  }
}

void printHeader(){
  Serial.println(header);
}

void measure(measures* measurements){
  // Powers up sensors and reads the data.

  digitalWrite(SensorPowerPin, HIGH);

  delay(1000); // Wait for the connection to stabilize.

  measurements->soilMoisture = analogRead(humiditySensorReadPin);

  measurements->humidity = dht.readHumidity();
  measurements->temperature = dht.readTemperature();

  digitalWrite(SensorPowerPin, LOW); // Turn the sensor power off.

}

double getDecimalTime(DateTime now){
  return (double)now.hour() + ((double)now.minute())/((double)60.0) + ((double)now.second())/((double)3600.0);
}

//439.33 is the boundary between wet and dry.
String soilCondition(int soilValue){
  // Returns a descriptive string of the humidity of the soil
  String string;
  if(soilValue >= water && soilValue < (water + interval)){
    string = "Very wet";
  } else
  if(soilValue >= (water + interval) && soilValue <= (air - interval)){
    string = "Wet";
  } else
  if(soilValue <= air && soilValue > (air - interval)){
    string = "Dry";
  }else string = "Out of range";

  return string;
}

void heatSwitch(int state){
  switch(state){
    case 0:
    {
      digitalWrite(HEATPIN, LOW);
      break;
    }
    case 1:{
      digitalWrite(HEATPIN, HIGH);
      break;
    }
  }
}

int getFreeRam()
{
  extern int __heap_start, *__brkval; 
  int v;

  v = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

  Serial.print("Free RAM = ");
  Serial.println(v, DEC);

  return v;
}

void printTime(DateTime now){
  String line = "";
  String yr = String(now.year(), DEC);
  String mnth = String(now.month(), DEC);
  String dy = String(now.day(), DEC);
  String hr = String(now.hour(), DEC);
  String mnt = String(now.minute(), DEC);
  String lst = String(now.second(), DEC);
  
  line += yr + '/' + mnth + '/' + dy + ' ';
  line += hr + ':' + mnt + ':' + lst + ';';
  Serial.println(line);
}

void loop () {

    DateTime now = rtc.now(); // read time.
    
    while(Serial.available())
    {
      lightAdmin = Serial.parseInt();
      switch(lightAdmin){
        case 12345:{ // like SWI 12345
        file.close();
        Serial.println(F("Done"));
        SysCall::halt();
        break;
        }
        case 5:{
          if(logging) logData(now);
          else Serial.println("Didn't write, that's what you wanted, right?");
          break;
        }
        case 41:{
          heatSwitch(1);
          break;
        }
        case 40:{ // heat controller
          heatSwitch(0);
          break;
        }
        case 31:{
          Serial.println("Updating time..");
          rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
          printTime(now);
          break;
        }
        case 30:{ // check time
        printTime(now);
        break;
        }
        case 21:{
        if(!isInitialised){
          initSD();
          logging = true;
          Serial.println("Custom mode: logging on. \t Reminder: Did you insert SD card?");
        }
        break;
        }
        case 20:{ // 2 is switch for logging. 20 disables, 21 enables.
        if(isInitialised){
          logging = false;
          file.close();
          Serial.println("Custom mode: logging off. You can safely remove SD card.");
          isInitialised = false;
        }
        break;
        }
        case 11:{ // switch for lighting
        Serial.println("Custom mode: light on.");
        lightCondition = true;
        digitalWrite(lightControlPin, LOW);
        break;
        }
        case 10:{
        Serial.println("Custom mode: light off.");
        lightCondition = false;
        digitalWrite(lightControlPin, HIGH);
        break;
        }
        case 0:{
        Serial.println("Normal mode on.");
        break;
        }
      }
      if(lightAdmin != 10 && lightAdmin != 11) lightAdmin = 0;
    }

    /*
     * Ideally the plants should get 14-16 hours of light. We are going to go with 14 hours of light for testing.
     * That means 8am - 10pm.
     */

     if(lightAdmin != 10 && lightAdmin != 11){
      // if time is inside interval <8am,10pm> and light is not on (user has not turned it on) then:
      if(getDecimalTime(now) >= 8.0 && getDecimalTime(now) < 22.0){ //TODO: not constantly turning on
        digitalWrite(lightControlPin, LOW); // it's NC (normally closed), so LOW turns the light on.
        lightCondition = true;
      } else{
        digitalWrite(lightControlPin, HIGH); // turn the light off.
        lightCondition = false;
      }
     }
     

    /**
     * Humidity measurement - later we're going to split off measuring soil moisture and other measurements 
     * beacause some are more critical than others (and should be measured more often).
     * 
     * We want to measure moisture every 6 hours so that we have 4 measurements per day.
     * Considering moisture is not going to change so radically this is just fine.
     * Measurement hours: 1am, 7am, 13h, 19h
     */
     
    if(now.second() == 0 && (now.hour() % 1 == 0) && now.minute() % 30 == 0){
      if(!written){

        if(logging) logData(now);
        else Serial.println("Didn't write, that's what you wanted, right?");

        written = true;
      }
    } else{
      written = false;
    }
}


