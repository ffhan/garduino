#include "Logging.h"
#include "RTClib.h"
#include "Measuring.h"
#include "Control.h"
#include "WebController.h"

Logging::Logging(Control *sys){
  this->sys = sys;
}

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
  sys->web->postEntry();
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
