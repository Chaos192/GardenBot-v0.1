#ifndef AUTO_PILOT_H
#define AUTO_PILOT_H

#include <Arduino.h>
#include "Dispositivo.h"

class AutoPilot
{
private:
  long timeON;
  long timeOFF;
  bool isHours;
  Dispositivo disp;
  unsigned long anteriorMillis = 0;

public:
  AutoPilot(Dispositivo _disp, long _timeon, long _timeoff, bool _ishours);
  void startAP();
};

#endif