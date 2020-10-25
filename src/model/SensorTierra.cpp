#include "SensorTierra.h"
#include <ArduinoJson.h>

const char *DEVICE_ID= "Device ID";
const char *DEV_NAME= "Sensor Tierra";
const char *HUM= "Humedad";

SensorTierra::SensorTierra(uint8_t pinVCC, uint8_t pinanalog, int id, String nombre) {
    this->_pinVCC = pinVCC;
    this->_pinAnalog = pinanalog;
    this->_id = id;
    this->_nombre = nombre;
}

void SensorTierra::begin() {
    pinMode(_pinVCC, OUTPUT);
    off();
}

void SensorTierra::off() {
    digitalWrite(_pinVCC, LOW);
}
void SensorTierra::on() {
    digitalWrite(_pinVCC, HIGH);
}

String SensorTierra::nombre() {
    return this -> _nombre;
}

int SensorTierra::id(){
    return this->_id;
}

/**
 * returns _humSuelo: Int 
 * after 3 reads and avg*/

int SensorTierra::getDataSuelo() {
    on();                       //sensor ON
    delay(500);
    int N = 3;
    for (int i = 0; i < N; i++) // read n times and add to get avg RH
    {
      _humSuelo += analogRead(_pinAnalog);
      delay(150);
    }
    off();                      //sensor OFF
    _humSuelo = _humSuelo / N;  //AVG
    _humSuelo = map(_humSuelo, 1024, 0, 0, 100);
    printToSerial(_humSuelo);
    return _humSuelo;
}

void SensorTierra::printToSerial(int h) {
    Serial.print(" Soil Humidity: ");
    Serial.print(h);
    Serial.println(" %");
}

/**
 * returns Json document 
 * containing device data and
 * measurements*/

DynamicJsonDocument SensorTierra::getJsonData() {
    StaticJsonDocument<512> json;
    json[DEVICE_ID] = _id;
    json[DEV_NAME] = _nombre;
    json[HUM] = getDataSuelo();
    return json; 
}