// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"
#include <SPI.h>
#include "SdFat.h"

#define DHTPIN 7 // dht22 pin
#define DHTTYPE DHT22

#define FILE_BASE_NAME "logger" //log file base name.
#define error(msg) sd.errorHalt(F(msg)) // error messages stored in flash.

//SDFat setup
const uint8_t chipSelect = 4;
SdFat sd; // File system object.
SdFile file; // Log file.

RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);

char fileName[13];

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
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

measures *measurements = (measures*) malloc(sizeof(measures));

void setup () {

  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";

  Serial.begin(9600);
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
  /*rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  DS1307 doesn't have the lost power option. TODO: enable time setting,
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }*/
  
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
  /* DO THIS IF YOU WANT TO WRITE TO A DIFFERENT FILE EVERY TIME YOU UPLOAD NEW CODE.
  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } else {
      error("Can't create file name");
    }
  }*/
  
  pinMode(lightControlPin, OUTPUT); // Control light control pin as output
  pinMode(SensorPowerPin, OUTPUT); // Control humidity sensor power as output
  pinMode(humiditySensorReadPin, INPUT); // Get data from humidity sensor
  
}

void writeHeader(){
  file.println(header);
  printHeader();
}
void logData(measures* track, DateTime now){
  String line = "";
  String yr = String(now.year(), DEC);
  String mnth = String(now.month(), DEC);
  String dy = String(now.day(), DEC);
  String hr = String(now.hour(), DEC);
  String mnt = String(now.minute(), DEC);
  String sc = String(now.second(), DEC);
  String moisture = String(track->soilMoisture, DEC);
  String condition = soilCondition(track->soilMoisture);
  String humidity = String(track->humidity, DEC);
  String tmp = String(track->temperature, DEC);
  String lightCond = (lightCondition ? "On" : "Off");
  
  line += yr + '/' + mnth + '/' + dy + ' ';
  line += hr + ':' + mnt + ':' + sc + ';';
  line += moisture + ';' + condition + ';' + humidity + ';';
  line += tmp + ';';
  line += lightCond;
  /*
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(";");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(";");
  Serial.print(measurements->soilMoisture);
  Serial.print(";");
  Serial.print(soilDescription);
  Serial.print(";");
  Serial.print(measurements->humidity);
  Serial.print("%;");
  Serial.print(measurements->temperature);
  Serial.print("°C;");
  Serial.println(lightCondition ? "On" : "Off");*/
  Serial.println(line);
  file.println(line);

  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) {
    error("write error");
  }
}

void printHeader(){
  Serial.println(header);
}

void measure(){
  // Powers up sensors and reads the data.

  digitalWrite(SensorPowerPin, HIGH);

  delay(1000); // Wait for the connection to stabilize.

  measurements->soilMoisture = analogRead(humiditySensorReadPin);

  measurements->humidity = dht.readHumidity();
  measurements->temperature = dht.readTemperature();

  delay(1000);
  digitalWrite(SensorPowerPin, LOW); // Turn the sensor power off.

}

double getDecimalTime(DateTime now){
  return (double)now.hour() + ((double)now.minute())/((double)60.0) + ((double)now.second())/((double)3600.0);
}
String soilCondition(int soilValue){
  // Returns a descriptive string of the humidity of the soil
  String string;
  if(soilValue > water && soilValue < (water + interval)){
    string = "Very wet";
  } else
  if(soilValue > (water + interval) && soilValue < (air - interval)){
    string = "Wet";
  } else
  if(soilValue < air && soilValue > (air - interval)){
    string = "Dry";
  }else string = "Out of range";

  return string;
}

void loop () {

    DateTime now = rtc.now(); // read time.
    
    while(Serial.available())
    {
      lightAdmin = Serial.parseInt();
      switch(lightAdmin){
        case 12345: // like SWI 12345
        file.close();
        Serial.println(F("Done"));
        SysCall::halt();
        break;
        case 21:
        if(!isInitialised){
          const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
          char fileName[13] = FILE_BASE_NAME "00.csv";
          if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
            sd.initErrorHalt();
          }
          if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
            if(!file.open(fileName, O_WRITE | O_AT_END)){
              Serial.println(fileName);
              
              error("file.open");
            }
            else Serial.println("File already existing, opened successfully.");
          } 
          else{
            Serial.println("File didn't exist. Writing a header to it.");
            writeHeader(); // if file doesn't exist write a header to it.
          }
          Serial.print(F("Logging to: "));
          Serial.println(fileName);
          printHeader();
          logging = true;
          isInitialised = true;
          Serial.println("Custom mode: logging on. \t Reminder: Did you insert SD card?");
        }
        break;
        case 20: // 2 is switch for logging. 20 disables, 21 enables.
        logging = false;
        if(isInitialised){
          file.close();
          Serial.println("Custom mode: logging off. You can safely remove SD card.");
          isInitialised = false;
        }
        break;
        case 11: // switch for lighting
        Serial.println("Custom mode: light on.");
        lightCondition = true;
        digitalWrite(lightControlPin, LOW);
        break;
        case 10:
        Serial.println("Custom mode: light off.");
        lightCondition = false;
        digitalWrite(lightControlPin, HIGH);
        break;
        case 0:
        Serial.println("Normal mode on.");
        break;
      }
    }

    /*
     * Ideally the plants should get 14-16 hours of light. We are going to go with 14 hours of light for testing.
     * That means 8am - 10pm.
     */

     if(lightAdmin == 0){
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

    if(now.second() == 0 && (now.hour() % 1 == 0) && now.minute() == 0){
      if(!written){
            
        measure();

        if(logging) logData(measurements, now);

        written = true;
      }
    } else{
      written = false;
    }
}
