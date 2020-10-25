#ifndef SENSOR_TIERRA_H
#define SENSOR_TIERRA_H

#include <Arduino.h>
#include <ArduinoJson.h>

class SensorTierra
{

private:
  int _id;
  String _nombre;
  int _humSuelo = 0;
  uint8_t _pinVCC;
  uint8_t _pinAnalog;

public:
  SensorTierra(uint8_t pinVCC, uint8_t pinanalog, int id, String nombre);
  void begin();
  void off();
  void on();
  int getDataSuelo();
  int id();
  String nombre();
  void printToSerial(int h);
  DynamicJsonDocument getJsonData();
};

#endif