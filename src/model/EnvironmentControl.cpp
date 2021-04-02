#include "EnvironmentControl.h"
#include "Dispositivo.h"
#include "AutoPilot.h"
#include "./utils/Constants.h"


/********************************************
 * Environment control class with updateable
 * params
 * 
 * @param _sensor: DHT sensor custom class
 * @param _vent: oscillating fan
 * @param _intr: inline input fan
 * @param _extr:inline output fan
 * @param _pilot: oscillating fan autopilot
 * ******************************************/
EnvironmentControl::EnvironmentControl( SensorAmbiente _sensor,
                                        Dispositivo _vent,
                                        Dispositivo _intr,
                                        Dispositivo _extr,
                                        AutoPilot _pilot) {
    this->vent = _vent;
    this->extr = _extr;
    this->intr = _intr;
    this->sensor = _sensor;
    this->pilot = _pilot;
}

void EnvironmentControl::start() { this->isRunning = true; }

void EnvironmentControl::stop() { this->isRunning = false; }

void EnvironmentControl::pause(bool _pause) { this->paused = _pause; }

bool EnvironmentControl::isPaused() { return paused; } 

bool EnvironmentControl::isWorking() { return isRunning; }

/**
 * Update operating parameters
 * 
 * @param _minHum: minimum operating relative humidity
 * @param _maxHum: maximum operating relative humidity
 * ****************************************************/
String EnvironmentControl::setParams(int _minHum, int _maxHum) {
    this->minHum = _minHum;
    this->maxHum = _maxHum;
    return "Updated humidity parameters successfully";
}

String EnvironmentControl::checkEnvironment() {

    SimpleMap<String, float> currentData = sensor.getData();
    int humidity = round(currentData.get(Constants::AIR_HUM));
    Serial.println("HUM " + (String) humidity);
    Serial.println("MAX HUM " + (String) minHum);
    Serial.println("MIN HUM " + (String) maxHum);

    if (humidity < minHum) // DRY CONDITIONS
    {
        Serial.println("DRY CONDITIONS");
        pilot.pause(true);
        extr.off();
        intr.off();
        vent.on();
        return "Humedad ambiental baja, intentando subir...";
    }
    else if (humidity > maxHum) //WET CONDITIONS
    {
        Serial.println("WET CONDITIONS");
        pilot.pause(true);
        intr.on();
        extr.on();
        vent.on();
        return "Humedad ambiental alta, intentando bajar...";
    }
    else  //NORMAL OPERATION
    {
        Serial.println("NORMAL OPERATION");
        pilot.pause(false);
        extr.on();
        intr.off();  
    }
    return "Humedad ambiental dentro de parametros normales, activando piloto automatico...";
}
