#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Dispositivo
{

private:
  int id;
  String nombre;
  uint8_t pin;
  bool estado;

public:
  Dispositivo();
  Dispositivo(uint8_t pin, String _nombre, int _id);
  void on();
  void off();
  void begin();
  void blink();
  void receiveOrder(String);
  String getEstado();
  String getNombre();
  int getId();
  DynamicJsonDocument getJsonData();

};

#endif