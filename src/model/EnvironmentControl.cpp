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
EnvironmentControl::EnvironmentControl(SensorAmbiente _sensor,
                                       Dispositivo _vent,
                                       Dispositivo _intr,
                                       Dispositivo _extr,
                                       AutoPilot _pilot)
{
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
String EnvironmentControl::setParams(int _minHum, int _maxHum, int _minTemp, int _maxTemp)
{
    this->minHum = _minHum;
    this->maxHum = _maxHum;
    this->minTemp = _minTemp;
    this->maxTemp = _maxTemp;
    return "Updated humidity and temperature parameters successfully";
}

bool EnvironmentControl::isTempFine(int temp) { return (temp > minTemp) && (temp < maxTemp); }

bool EnvironmentControl::isHumFine(int hum) { return (hum > minHum) && (hum < maxHum); }

bool EnvironmentControl::everythingsFine(int temp, int hum) { return isTempFine(temp) && isHumFine(hum); }

bool EnvironmentControl::is2Cold(int temp) { return (temp < minTemp); }

bool EnvironmentControl::is2Hot(int temp) { return (temp > maxTemp); }

bool EnvironmentControl::is2Wet(int hum) { return (hum > maxHum); }

bool EnvironmentControl::is2Dry(int hum) { return (hum < minHum); }

/**
     * +-----+-----+----+----+----+----+------+-----+-----+
     * | Hok | Tok | H^ | Hv | T^ | Tv | Vent | Int | Ext |
     * +-----+-----+----+----+----+----+------+-----+-----+
     * | 1   | 1   | 0  | 0  | 0  | 0  | Auto | 0   | 1   | x NORMAL OP
     * +-----+-----+----+----+----+----+------+-----+-----+
     * | 0   | x   | 0  | 1  | 0  | 1  | Auto | 0   | 0   | x DRY COLD 
     * +-----+-----+----+----+----+----+------+-----+-----+
     * | 1   | 0   | 0  | 0  | 0  | 1  | Auto | 1   | 0   | x COLD
     * +-----+-----+----+----+----+----+------+-----+-----+
     * | 0   | 1   | 1  | 0  | 0  | 0  | 1    | 1   | 1   | x HIGH HUMIDITY 
     * +-----+-----+----+----+----+----+------+-----+-----+
     * | 0   | 0   | 1  | 0  | 0  | 1  | 1    | 1   | 0   | x WET COLD
     * +-----+-----+----+----+----+----+------+-----+-----+
     * | 0   | 0   | 0  | 1  | 1  | 0  | 0    | 0   | 1   | x DRY HOT
     * +-----+-----+----+----+----+----+------+-----+-----+
     * **/
String EnvironmentControl::checkEnvironment()
{

    SimpleMap<String, float> currentData = sensor.getData();
    int humidity = round(currentData.get(Constants::AIR_HUM));
    int temp = round(currentData.get(Constants::AIR_TEMP));

    Serial.println("HUM " + (String)humidity);
    Serial.println("TEMP " + (String)temp);
    Serial.println("MAX HUM " + (String)minHum);
    Serial.println("MIN HUM " + (String)maxHum);
    Serial.println("MIN TEMP " + (String)minTemp);
    Serial.println("MAX TEMP " + (String)maxTemp);

    if (is2Dry(humidity) && is2Cold(temp)) // DRY COLD CONDITIONS
    {
        Serial.println("DRY COLD CONDITIONS");
        pilot.pause(false);
        intr.off();
        extr.off();
        return "Humedad ambiental baja y temperatura alta, intentando regular";
    }
    else if (isHumFine(humidity) && is2Cold(temp)) //COLD CONDITIONS
    {
        Serial.println("COLD CONDITIONS");
        pilot.pause(false);
        intr.on();
        extr.off();
        return "Temperatura baja, intentando subir...";
    }
    else if (isTempFine(temp) && is2Wet(humidity)) //HIGH HUMIDITY CONDITIONS
    {
        Serial.println("HIGH HUMIDITY CONDITIONS");
        pilot.pause(true);
        vent.on();
        intr.on();
        extr.on();
        return "Humedad ambiental alta, intentando bajar...";
    }
    else if (is2Hot(temp) && is2Dry(humidity)) //DRY HOT CONDITIONS
    {
        Serial.println("DRY HOT CONDITIONS");
        pilot.pause(true);
        vent.off();
        intr.off();
        extr.on();
        return "Humedad ambiental alta y temperatura baja, intentando regular...";
    }
    else if (is2Cold(temp) && is2Wet(humidity)) //WET COLD CONDITIONS
    {
        Serial.println("WET COLD CONDITIONS");
        pilot.pause(true);
        vent.on();
        intr.on();
        extr.off();
        return "Humedad ambiental alta y temperatura baja, intentando regular...";
    }
    else if (everythingsFine(temp, humidity)) //NORMAL OPERATION
    {
        Serial.println("NORMAL OPERATION");
        pilot.pause(false);
        intr.off();
        extr.on();
    }
    return "Humedad ambiental dentro de parametros normales, activando piloto automatico...";
}
