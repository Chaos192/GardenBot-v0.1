#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Dispositivo
{

private:
  int _id;
  String _nombre;
  uint8_t _pin;
  bool _estado;

public:
  Dispositivo();
  Dispositivo(uint8_t pin, String _nombre, int _id);
  void on();
  void off();
  void begin();
  void blink();
  String estado();
  String nombre();
  int id();
  DynamicJsonDocument getJsonData();

};

#endif