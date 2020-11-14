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
  bool isRunning;
  bool paused;
  unsigned long anteriorMillis;

public:
  AutoPilot(Dispositivo _disp, int _timeon, int _timeoff);
  AutoPilot(Dispositivo _disp);
  void setTime(long _timeon, long _timeoff);
  void setHours(int, int);
  bool isWorking();
  bool isPaused();
  void pause(bool);
  void setRunning(bool);
  void startAP();
  void runForTime(void (*callback)());
  void setStart();
};

#endif