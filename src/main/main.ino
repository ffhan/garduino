// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
//#include <Wire.h>
//#include "RTClib.h"
//#include "DHT.h"
#include <SPI.h>
//#include "SdFat.h"
#include <stdlib.h>
//#include <IRremote.h>
//#include <Ethernet.h>
//#include <EEPROM.h>
//#include <avr/pgmspace.h>
#include "Control.h"
#include "WebController.h"
#include "pins.h"
#include "Logging.h"

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
/*
IRrecv irrecv(IR_PIN);
decode_results results;

RTC_DS3231 rtc;

DHT dht(DHTPIN, DHTTYPE);
*/

//char fileName[13];

const int measureNumber = 5;

int getFreeRam()
{
  extern int __heap_start, *__brkval;
  int v;

  v = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

  Serial.print("Free RAM = ");
  Serial.println(v, DEC);

  return v;
}

Control *sys;

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

  sys = new Control();

  pinMode(lightControlPin, OUTPUT); // Control light control pin as output
  pinMode(SensorPowerPin, OUTPUT); // Control humidity sensor power as output
  pinMode(humiditySensorReadPin, INPUT); // Get data from humidity sensor
  pinMode(HEATPIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(LOGGING_LED, OUTPUT);
  pinMode(RgbControlPin, OUTPUT);
  pinMode(rGbControlPin, OUTPUT);

  digitalWrite(LOGGING_LED, HIGH);

  sys->logger->printHeader();
  Serial.println(F("Setup complete!"));

}

void loop() {

  while (Serial.available())
  {
    int choice = Serial.parseInt();
    sys->mainSwitch(choice);
  }

  sys->update();
}

