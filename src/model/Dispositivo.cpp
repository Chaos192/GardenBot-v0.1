#include "Dispositivo.h"
#include <ArduinoJson.h>
#include "./utils/Constants.h"

Dispositivo::Dispositivo(){}
Dispositivo::Dispositivo(uint8_t pin, String nombre, int id) {
    this -> _pin = pin;
    this -> _estado = LOW;
    this -> _nombre = nombre;
    this -> _id = id;
    begin();
}

void Dispositivo::begin() {
    pinMode(_pin, OUTPUT);
    off();
}

void Dispositivo::off() {
    digitalWrite(_pin, LOW);
    _estado = LOW;
}

void Dispositivo::on() {
    digitalWrite(_pin, HIGH);
    _estado = HIGH;
}
void Dispositivo::blink() {
    on();
    delay(500);
    off();
}

String Dispositivo::nombre() {
    return this->_nombre;
}

String Dispositivo::estado() {
    return this->_estado ? "ON" : "OFF";
}

int Dispositivo::id() {
    return this -> _id;
}

/**
 * returns Json document 
 * containing device data and
 * measurements*/

DynamicJsonDocument Dispositivo::getJsonData() {
    StaticJsonDocument<512> json;
    json[Constants::DEVICE_ID] = _id;
    json[Constants::DEVICE_NAME] = _nombre;
    json[Constants::STATUS] = estado();
    return json; 
}
