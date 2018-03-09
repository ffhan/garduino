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
#include "Item.h"
#include "ItemNode.h"
#include "Menu.h"
#include "Setting.h"
#include "BoolSetting.h"
#include "IntSetting.h"
#include "IntBitSetting.h"
#include "BoolBitSetting.h"
#include "Screen.h"
#include "Action.h"

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
Screen *screen = (Screen*) malloc(sizeof(Screen));

double getDecimalTime(DateTime now) {
  return (double)now.hour() + ((double)now.minute()) / ((double)60.0) + ((double)now.second()) / ((double)3600.0);
}

Action *action = (Action *) malloc(sizeof(Action));

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

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  sys = new Control();

  Menu *mainMenu = new Menu("Main menu");

  Menu *sysControlMenu = new Menu("Settings");

  Menu *commandMenu = new Menu("Commands");

  Menu *lightSettings = new Menu("Light controls");
  Menu *heatSettings = new Menu("Heat controls");
  Menu *wateringSettings = new Menu("Watering controls");
  Menu *fanSettings = new Menu("Fan settings");

  //  Setting *measureCommand = new Setting("Measure", sys, Control::
  //  currently not able to execute commands unless through mainSwitch.
  //  The current system is messy. I want to delegate everything through the mainSwitch,
  //  because dealing with an enormous amount of member functions just to evade adding
  //  int code in settings by default increases system complication.
  //  Con of this approach is that I'm cornering myself into using only mainSwitch.
  //  That's bad design, already messing with me with BoolBitSettings.

  BoolBitSetting *globalLockSetting = new BoolBitSetting("Global lock", sys, sys->globalLockAction, &Control::getLock);

  BoolBitSetting *lightAdminSetting = new BoolBitSetting("Light admin", sys, sys->lightAdminAction, &Control::getLightAdmin);
  BoolBitSetting *lightStateSetting = new BoolBitSetting("Light state", sys, sys->lightStateAction, &Control::getLightingState);

  BoolBitSetting *heatAdminSetting = new BoolBitSetting("Heating admin", sys, sys->heatAdminAction, &Control::getHeatAdmin);
  BoolBitSetting *heatStateSetting = new BoolBitSetting("Heating state", sys, sys->heatStateAction, &Control::getHeatingState);

  BoolBitSetting *wateringAdminSetting = new BoolBitSetting("Watering admin", sys, sys->wateringAdminAction, &Control::getWateringAdmin);
  BoolBitSetting *wateringStateSetting = new BoolBitSetting("Watering state", sys, sys->wateringStateAction, &Control::getWateringState);

  //BoolBitSetting *fanAdminSetting = new BoolBitSetting("Fan admin", sys, &Control::empty, &Control::getFanAdmin, 11);
  //IntBitSetting *fanSpeedSetting = new IntBitSetting("Fan speed", sys, &Control::empty, &Control::getFanSpeed, 9, 7);

  Setting *printTimeCommand = new Setting("Print time", sys, sys->printTimeAction);
  commandMenu->addItems(printTimeCommand);

  mainMenu->addItems(commandMenu, sysControlMenu);
  sysControlMenu->addItems(globalLockSetting, lightSettings, heatSettings, wateringSettings, fanSettings);
  lightSettings->addItems(lightAdminSetting, lightStateSetting);
  heatSettings->addItems(heatAdminSetting, heatStateSetting);
  wateringSettings->addItems(wateringAdminSetting, wateringStateSetting);
  //fanSettings->addItems(fanAdminSetting, fanSpeedSetting);

  screen = new Screen(16, 2, mainMenu);

  screen->show();

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

  bool typed = false;
  while (Serial.available() > 0) {
    int choice = Serial.parseInt();
    if (!typed) {
      Serial.println(choice);
      switch (choice) {
        case 8:
          screen->up();
          break;
        case 2:
          screen->down();
          break;
        case 5:
          screen->enter();
          break;
        case 3:
          screen->back();
          break;
        case 4:
          screen->left();
          break;
        case 6:
          screen->right();
          break;
      }
      //screen->flash(&printItem);
      screen->show();
      typed = true;
    }
  }
  /*
    while (Serial.available())
    {
    int choice = Serial.parseInt();
    sys->mainSwitch(choice);
    }
  */
  sys->update();
}

