#include "AutoPilot.h"
#include "Dispositivo.h"
#include "SimpleMap.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
const long utcOffset = -10800;

AutoPilot::AutoPilot() {}
AutoPilot::AutoPilot(Dispositivo _disp, int _timeon, int _timeoff)
{
  this->disp = _disp;
  this->horaON = _timeon;
  this->horaOFF = _timeoff;
  this->isRunning = false;
  this->paused = false;
}
AutoPilot::AutoPilot(Dispositivo _disp)
{
  this->disp = _disp;
  this->isRunning = false;
  this->paused = false;
}

bool AutoPilot::isPaused() { return paused; }

bool AutoPilot::isWorking() { return isRunning; }

void AutoPilot::setRunning(bool start) { this->isRunning = start; }

void AutoPilot::pause(bool pause) { this->paused = pause; }

String AutoPilot::setTime(long _timeon, long _timeoff)
{
  this->timeON = _timeon;
  this->timeOFF = _timeoff;
  return disp.getNombre() + " -> timer updated succesfully!";
}

String AutoPilot::setHours(int _hourON, int _hourOFF)
{
  this->horaON = _hourON;
  this->horaOFF = _hourOFF;
  return disp.getNombre() + " -> timer updated succesfully!";

}

/**
 * sets auto pilot working mode
 * @param _mode: String "manual" or "auto"
 * **************************************/
String AutoPilot::setMode(String _mode)
{
  this->mode = _mode;
  return disp.getNombre() + " ->Autopilot mode updated " + (String) mode;
}

String AutoPilot::getMode()
{
  return mode;
}

void AutoPilot::setStart()
{
  this->anteriorMillis = millis();
  Serial.println("anteriorMillis = " + anteriorMillis);
}

void AutoPilot::startAP()
{

  if (isRunning && !paused)
  {
    WiFiUDP servidorReloj;
    NTPClient clienteReloj(servidorReloj, "south-america.pool.ntp.org", utcOffset);
    clienteReloj.update();
    int horaActual = clienteReloj.getHours();

    //if light cycle is day/night
    if (horaON < horaOFF)
    {

      if ((horaON <= horaActual) && (horaActual < horaOFF))
      {
        disp.on();
      }
      else
      {
        disp.off();
      }
      //else if light cycle is night/day
    }
    else
    {

      if ((horaOFF <= horaActual) && (horaActual < horaON))
      {
        disp.off();
      }
      else
      {
        disp.on();
      }
    }
    Serial.println("hora actual " + (String)horaActual);
  }
}

void AutoPilot::runForTime(void (*callback)())
{

  if (isRunning && !paused)
  {
    unsigned long ahoraMillis = millis();

    //CYCLE ON
    if ((ahoraMillis - anteriorMillis) < timeON)
    {
      disp.on();
      Serial.println(disp.getNombre() + " will be on for " + timeON / 1000 / 60 + " minutes");

      //CYCLE OFF
    }
    else if ((ahoraMillis - anteriorMillis) >= timeON)
    {
      disp.off();
      Serial.println(disp.getNombre() + " will be off for " + timeOFF / 1000 / 60 + " minutes");

      //CYCLE END RESET AND CALLBACK
    }
    else if (ahoraMillis - anteriorMillis > timeON + timeOFF)
    {
      disp.off();
      setStart();
      (*callback)();
    }
  }
}
