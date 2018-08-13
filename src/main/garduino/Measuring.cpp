#include "Measuring.h"
#include "../../support/pins.h"

Measuring::Measuring() {
  soilMoisture = 0;
  light = 0;
  temperature = 0;
  humidity = 0;

  dht = new DHT(DHTPIN, DHTTYPE);

  Serial.print(F("Initialising DHT22 sensor...\t"));
  dht->begin();
  Serial.println(F("done."));

  delay(1000); // wait for console opening
}

void Measuring::measure() {
  digitalWrite(SensorPowerPin, HIGH); // Powers up sensors and reads the data.

  delay(1000);

  soilMoisture = analogRead(humiditySensorReadPin);
  humidity = dht->readHumidity();
  temperature = dht->readTemperature();

  digitalWrite(SensorPowerPin, LOW); // Turn the sensor power off.
}

int Measuring::getSoilMoisture() {
  return soilMoisture;
}
float Measuring::getTemperature() {
  return temperature;
}
float Measuring::getHumidity() {
  return humidity;
}
int Measuring::getLight() {
  return light;
}

