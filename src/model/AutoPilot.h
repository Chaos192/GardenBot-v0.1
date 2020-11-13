#ifndef AUTO_PILOT_H
#define AUTO_PILOT_H

#include <Arduino.h>
#include "Dispositivo.h"
#include "SimpleMap.h"

class AutoPilot
{
private:
  long timeON;
  long timeOFF;
  int horaON;
  int horaOFF;
  Dispositivo disp;
  unsigned long anteriorMillis;

public:
  AutoPilot(Dispositivo _disp, int _timeon, int _timeoff);
  AutoPilot(Dispositivo _disp);
  void setTime(long _timeon, long _timeoff);
  void startAP();
  void runForTime(void(*callback)());
  void setStart();
};

#endif