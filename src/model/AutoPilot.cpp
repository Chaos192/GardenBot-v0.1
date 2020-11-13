#include "AutoPilot.h"
#include "Dispositivo.h"
#include "SimpleMap.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
const long utcOffset = -10800;


AutoPilot::AutoPilot(Dispositivo _disp, int _timeon, int _timeoff) {
    this->disp = _disp;
    this->horaON = _timeon;
    this->horaOFF = _timeoff;

}
AutoPilot::AutoPilot(Dispositivo _disp) {
  this-> disp = _disp;
}

void AutoPilot::setTime(long _timeon, long _timeoff) {
  this->timeON = _timeon;
  this->timeOFF = _timeoff;
}


void AutoPilot::setStart(){
  this->anteriorMillis = millis();
  Serial.println("anteriorMillis = " + anteriorMillis);
}

void AutoPilot::startAP() {
    WiFiUDP servidorReloj;
    NTPClient clienteReloj(servidorReloj, "south-america.pool.ntp.org", utcOffset);

    clienteReloj.update();
    int horaActual = clienteReloj.getHours();
    Serial.println("hora actual" + horaActual);

    if ((horaON <= horaActual) && (horaActual < horaOFF)) {
      disp.on();

    } else {

      disp.off();
    }
    
}

void AutoPilot::runForTime(void(*callback)()) {
  unsigned long ahoraMillis = millis();
  if ((ahoraMillis - anteriorMillis < timeON))
    {
      disp.on();
      Serial.println(disp.nombre() + " will be on for " + timeON);
    }
    else if ((ahoraMillis - anteriorMillis >= timeON) && (ahoraMillis - anteriorMillis < timeOFF))
    {
      disp.off();
      Serial.println(disp.nombre() + " will be off for " + timeOFF);
    } else if (ahoraMillis - anteriorMillis > timeON + timeOFF) {
      disp.off();
      setStart();
      (*callback)();
    }
}

