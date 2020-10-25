#include "AutoPilot.h"
#include "Dispositivo.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
const long utcOffset = -10800;


AutoPilot::AutoPilot(Dispositivo _disp, long _timeon, long _timeoff, bool _ishours) {
    this->disp = _disp;
    this->timeON = _timeon;
    this->timeOFF = _timeoff;
    this->isHours = _ishours;

}

void AutoPilot::startAP() {
    WiFiUDP servidorReloj;
    NTPClient clienteReloj(servidorReloj, "south-america.pool.ntp.org", utcOffset);

    unsigned long ahoraMillis = millis();
    clienteReloj.update();

    if (isHours)
    {
      int horaActual = clienteReloj.getHours();
      if (((int)timeON <= horaActual) && ((int)horaActual < timeOFF))
      {
        disp.on();
        if (disp.estado())
        {
          Serial.println(disp.nombre() + " encendido correctamente");
        }
        else
        {
          Serial.println(disp.nombre() + ", hubo un problema al encender");
        }
      }
      else
      {
        disp.off();
        if (!disp.estado())
        {
          Serial.println(disp.nombre() + " apagado correctamente");
        }
        else
        {
          Serial.println(disp.nombre() + ", hubo un problema al apagar");
        }
      }
    }
    else

        if ((disp.estado()) && (ahoraMillis - anteriorMillis >= timeON))
    {
      disp.off();
    }
    else if ((!disp.estado()) && (ahoraMillis - anteriorMillis >= timeOFF))
    {
      disp.on();
    }
}

