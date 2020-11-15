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
        Dispositivo vent;
        Dispositivo intr;
        Dispositivo extr;
        SensorAmbiente sensor;
        AutoPilot pilot;
        bool isRunning;
        bool paused;

    public:
        EnvironmentControl(SensorAmbiente _sensor, Dispositivo _vent, Dispositivo _intr, Dispositivo _extr, AutoPilot _pilot);
        String setParams(int _minHum, int _maxHum);
        void pause(bool);
        void start();
        void stop();
        bool isWorking();
        bool isPaused();
        String checkEnvironment();
};



#endif