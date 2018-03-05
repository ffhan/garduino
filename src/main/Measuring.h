#ifndef __MEASURING_INCLUDED__
#define __MEASURING_INCLUDED__

class DHT;

class Measuring {
  public :

    int soilMoisture;
    int light;
    float temperature;
    float humidity;

    DHT *dht;

    Measuring();

    void measure();

    int getSoilMoisture();
    float getTemperature();
    float getHumidity();
    int getLight();
};

#endif
