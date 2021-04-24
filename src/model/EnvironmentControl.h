#ifndef ENVIRONMENT_CONTROL_H
#define ENVIRONMENT_CONTROL_H
#include <Arduino.h>
#include <SimpleMap.h>
#include "./model/Dispositivo.h"
#include "./model/AutoPilot.h"
#include "./model/SensorAmbiente.h"

class EnvironmentControl 
{
    private:
        int minHum;
        int maxHum;
        int minTemp;
        int maxTemp;
        Dispositivo vent;
        Dispositivo intr;
        Dispositivo extr;
        SensorAmbiente sensor;
        AutoPilot pilot;
        bool isRunning;
        bool paused;

    public:
        EnvironmentControl(SensorAmbiente _sensor, Dispositivo _vent, Dispositivo _intr, Dispositivo _extr, AutoPilot _pilot);
        String setParams(int _minHum, int _maxHum, int _minTemp, int _maxTemp);
        void pause(bool);
        void start();
        void stop();
        bool isWorking();
        bool isPaused();
        bool everythingsFine(int temp, int hum);
        bool isHumFine(int hum);
        bool isTempFine(int temp);
        bool is2Hot(int temp);
        bool is2Cold(int temp);
        bool is2Wet(int hum);
        bool is2Dry(int hum);
        String checkEnvironment();
};



#endif