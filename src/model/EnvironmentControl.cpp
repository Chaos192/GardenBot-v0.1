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
    return "Parametros de temperatura y humedad actualizados";
}

bool EnvironmentControl::isTempFine(int temp) { return (temp > minTemp) && (temp < maxTemp); }

bool EnvironmentControl::isHumFine(int hum) { return (hum > minHum) && (hum < maxHum); }

bool EnvironmentControl::everythingsFine(int temp, int hum) { return isTempFine(temp) && isHumFine(hum); }

bool EnvironmentControl::is2Cold(int temp) { return (temp < minTemp); }

bool EnvironmentControl::is2Hot(int temp) { return (temp > maxTemp); }

bool EnvironmentControl::is2Wet(int hum) { return (hum > maxHum); }

bool EnvironmentControl::is2Dry(int hum) { return (hum < minHum); }

void EnvironmentControl::turnEverythingOn()
{
    pilot.pause(true);
    vent.on();
    intr.on();
    extr.on();
}

void EnvironmentControl::ventAuto()
{
    pilot.pause(false);
    intr.off();
    extr.off();
}
void EnvironmentControl::ventOnIntrOn()
{
    pilot.pause(true);
    vent.on();
    intr.on();
    extr.off();
}
void EnvironmentControl::ventAutoIntrOn()
{
    pilot.pause(false);
    intr.on();
    extr.off();
}
void EnvironmentControl::extrOn()
{
    pilot.pause(true);
    vent.off();
    intr.off();
    extr.on();
}
void EnvironmentControl::ventAutoExtrOn()
{
    pilot.pause(false);
    intr.off();
    extr.on();
}
/**
+-----+-----+----+----+----+----+------+-----+-----+-----------------------+
| Hok | Tok | H^ | Hv | T^ | Tv | Vent | Ext | Int |                       |
+-----+-----+----+----+----+----+------+-----+-----+-----------------------+
|   0 |   0 |  1 |  0 |  1 |  0 | 1    |   1 |   1 | Hot Humid Conditions  | turnEverythingOn()
|   0 |   0 |  1 |  0 |  0 |  1 | 1    |   0 |   1 | Cold Humid Conditions | ventOnIntrOn()
|   1 |   0 |  0 |  0 |  1 |  0 | A    |   0 |   1 | Hot Conditions        | ventAutoIntrOn()
|   1 |   0 |  0 |  0 |  0 |  1 | A    |   0 |   0 | Cold Conditions       | ventAuto()
|   0 |   1 |  1 |  0 |  0 |  0 | 1    |   1 |   1 | Humid Conditions      | turnEverythingOn()
|   0 |   1 |  0 |  1 |  0 |  0 | 0    |   1 |   0 | Dry Conditions        | extrOn()
|   0 |   0 |  0 |  1 |  1 |  0 | 0    |   1 |   0 | Hot Dry Conditions    | extrOn()
|   0 |   0 |  0 |  1 |  0 |  1 | A    |   0 |   0 | Cold Dry Conditions   | ventAuto()
|   1 |   1 |  0 |  0 |  0 |  0 | A    |   1 |   0 | Everything's fine     | ventAutoExtrOn()
+-----+-----+----+----+----+----+------+-----+-----+-----------------------+
 **/
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
        ventAuto();
        return "Humedad ambiental baja y temperatura alta, intentando regular";
    }
    else if (is2Wet(humidity) && is2Hot(temp)) //HOT HUMID CONDITIONS
    {
        Serial.println("HOT HUMID CONDITIONS");
        turnEverythingOn();
        return "Temperatura alta y humedad alta, intentando regular";
    }
    else if (isHumFine(humidity) && is2Cold(temp)) //COLD CONDITIONS
    {
        Serial.println("COLD CONDITIONS");
        ventAuto();
        return "Temperatura baja, intentando subir...";
    }
    else if (isHumFine(humidity) && is2Hot(temp)) //HOT CONDITIONS
    {
        Serial.println("HOT CONDITIONS");
        ventAutoIntrOn();
        return "Temperatura alta, intentando bajar...";
    }
    else if (isTempFine(temp) && is2Wet(humidity)) //HIGH HUMIDITY CONDITIONS
    {
        Serial.println("HIGH HUMIDITY CONDITIONS");
        turnEverythingOn();
        return "Humedad ambiental alta, intentando bajar...";
    }
    else if (isTempFine(temp) && is2Dry(humidity)) //LOW HUMIDITY CONDITIONS
    {
        Serial.println("LOW HUMIDITY CONDITIONS");
        extrOn();
        return "Humedad ambiental baja, intentando subir...";
    }
    else if (is2Hot(temp) && is2Dry(humidity)) //DRY HOT CONDITIONS
    {
        Serial.println("DRY HOT CONDITIONS");
        extrOn();
        return "Humedad ambiental baja y temperatura alta, intentando regular...";
    }
    else if (is2Cold(temp) && is2Wet(humidity)) //WET COLD CONDITIONS
    {
        Serial.println("WET COLD CONDITIONS");
        ventOnIntrOn();
        return "Humedad ambiental alta y temperatura baja, intentando regular...";
    }
    else if (everythingsFine(temp, humidity)) //NORMAL OPERATION
    {
        Serial.println("NORMAL OPERATION");
        ventAutoExtrOn();
    }
    return "Humedad ambiental dentro de parametros normales, activando piloto automatico...";
}
