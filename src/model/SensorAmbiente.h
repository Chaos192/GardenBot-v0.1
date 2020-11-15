#ifndef SENSOR_AMBIENTE_H
#define SENSOR_AMBIENTE_H

#include <Arduino.h>
#include <SimpleMap.h>
#include <DHT.h>
#include <ArduinoJson.h>


class SensorAmbiente
{
  

private:
    int _id;
    String _nombre;
    float _temp;
    float _hum;
    uint8_t _pin;
    uint8_t _type;
    bool _estado;
    DHT sensor;
    


public:
    SensorAmbiente();
    SensorAmbiente(uint8_t pin, uint8_t type, int _id, String _nombre);
    SimpleMap<String, float> getData();
    void begin();
    int id();
    String nombre();
    void printToSerial(float t, float h);
    JsonObject getJsonData();


};

#endif