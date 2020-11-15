#include "Dispositivo.h"
#include <ArduinoJson.h>
#include "./utils/Constants.h"

Dispositivo::Dispositivo() {}
Dispositivo::Dispositivo(uint8_t _pin, String _nombre, int _id)
{
    this->pin = _pin;
    this->estado = LOW;
    this->nombre = _nombre;
    this->id = _id;
    begin();
}

void Dispositivo::begin()
{
    pinMode(pin, OUTPUT);
    off();
}

void Dispositivo::off()
{
    digitalWrite(pin, LOW);
    estado = LOW;
}

void Dispositivo::on()
{
    digitalWrite(pin, HIGH);
    estado = HIGH;
}
void Dispositivo::blink()
{
    on();
    delay(300);
    off();
    delay(300);
    on();
    delay(300);
    off();
    
}

String Dispositivo::getNombre()
{
    return this->nombre;
}

String Dispositivo::getEstado()
{
    return this->estado ? "ON" : "OFF";
}

int Dispositivo::getId()
{
    return this->id;
}

/**
 * returns Json document 
 * containing device data and
 * measurements*/

DynamicJsonDocument Dispositivo::getJsonData()
{
    StaticJsonDocument<512> json;
    json[Constants::DEVICE_ID] = getId();
    json[Constants::DEVICE_NAME] = getNombre();
    json[Constants::STATUS] = getEstado();
    return json;
}

/**
 * receives MQTT order from server and 
 * acts accordingly 
 * */   

void Dispositivo::receiveOrder(String message) {
    if (message.equalsIgnoreCase("on")) {
        on();
    } else if (message.equalsIgnoreCase("blink")) {
        blink();
    } else {
        off();
    }
}
