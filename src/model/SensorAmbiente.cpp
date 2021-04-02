#include "SensorAmbiente.h"
#include "./utils/Constants.h"
#include <DHT.h>
#include <SimpleMap.h>
#include <ArduinoJson.h>


SensorAmbiente::SensorAmbiente(){}

SensorAmbiente::SensorAmbiente(uint8_t pin, uint8_t type, int id, String nombre): sensor(pin, type) {
    this ->_nombre = nombre;
    this ->_id = id;
}

void SensorAmbiente::begin() {
    sensor.begin();
}

int SensorAmbiente::id() {
    return this->_id;
}

String SensorAmbiente::nombre() {
    return this->_nombre;
}

SimpleMap<String, float> SensorAmbiente::getData() {
    _temp = sensor.readTemperature();
    _hum = sensor.readHumidity();
    SimpleMap<String, float> dataMap = SimpleMap<String, float>([](String &a, String &b) -> int {
            if (a == b) return 0;      // a and b are equal
            else if (a > b) return 1;  // a is bigger than b
            else return -1;            // a is smaller than b
        });
    delay(2000);                   // delay for getting DHT11 data
    if (isnan(_hum) || isnan(_temp)) // Check if any reads failed and exit early (to try again).
    {
        Serial.println("Failed to read from DHT sensor!");
        dataMap.put(Constants::AIR_TEMP, 0.0f);
        dataMap.put(Constants::AIR_HUM, 0.0f);

    } else {
        dataMap.put(Constants::AIR_TEMP, _temp);
        dataMap.put(Constants::AIR_HUM, _hum);
        printToSerial(_temp, _hum);
    }

    return dataMap;
}

void SensorAmbiente::printToSerial(float t, float h) {
    Serial.print(" Temperature: ");
    Serial.print(t);
    Serial.print("oC   Humidity: ");
    Serial.print(h);
    Serial.println("%");
}



