#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <SimpleMap.h>
#include "./model/Dispositivo.h"
#include "./model/AutoPilot.h"
#include "./model/SensorTierra.h"
#include "./model/SensorAmbiente.h"
#include "./utils/Constants.h"


// WIFI setup
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

ESP8266WiFiMulti WiFiMulti;

//DEFAULT SETTINGS
#define tierraSeca 0
#define tierraHum 75
#define tempAlta 26
#define tempBaja 22
#define aireSeco 85
#define aireHum 99
#define horaLampON 10
#define horaLampOFF 22
// DHT11
#include <DHT.h>
#define DHTPIN D1
#define DHTTYPE DHT11

long ventiON;
long ventiOFF;
// Soil Moisture Sensor
uint8_t sensorTierra = A0;
uint8_t sensorTierraVcc = D2;

// LEDS
uint8_t lamp = D8;
uint8_t vent = D7;
uint8_t extr = D6;
uint8_t intr = D5;
uint8_t builtin = LED_BUILTIN;
uint8_t pump = D4;

// IDS
#define LAMP_ID 0
#define VENT_ID 1
#define INTR_ID 2
#define EXTR_ID 3
#define FC28_ID 4
#define DHT11_ID 5
#define PUMP_ID 6

// TIMER
#include <SimpleTimer.h>
SimpleTimer timer;
const long utcOffset = -10800; 
char diaSemana[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

long send_payload = 1000  * 30;         //send payload to server every 30 minutes
long check_env = 1000 * 15;            //check environment every 15 minutes
long check_lamp = 60 *  1000;           //check lamp cycle every hour
long check_auto_pilot = 1000  * 2;      //check auto pilot cycle every 2 minutes
long check_water = 1000 * 60  * 3;      //check soil humidity every 3 hours
long check_settings = 1000 * 60 * 24;  //ask server for settings every 24 hours

WiFiUDP servidorReloj;
NTPClient clienteReloj(servidorReloj, "south-america.pool.ntp.org", utcOffset);

// CLASSES
Dispositivo lampara = Dispositivo(lamp, "Lampara", LAMP_ID);
Dispositivo ventilador = Dispositivo(vent, "Ventilador", VENT_ID);
Dispositivo intractor = Dispositivo(intr, "Intractor", INTR_ID);
Dispositivo extractor = Dispositivo(extr, "Extractor", EXTR_ID);
SensorTierra sensorMaceta(sensorTierraVcc, sensorTierra, FC28_ID, "Sensor Tierra");
SensorAmbiente sensorAire = SensorAmbiente(DHTPIN, DHTTYPE, DHT11_ID, "Sensor Ambiente");
Dispositivo waterPump = Dispositivo(pump, "Bomba de agua", PUMP_ID);
Dispositivo built_in = Dispositivo(builtin, "built-in LED", 99);
AutoPilot autoVent(ventilador);
AutoPilot autoLamp(lampara, horaLampON, horaLampOFF);

//FUNCTION PROTOTYPES
void setupPeripherals();
void setupWifi();
void homeWelcome();                            
void handleNotFound();
void sendDhtPacket();
void setupTimerIntervals();
void sendPayloadToServer();
void checkEnvironment();
void checkSoilWatering();
void checkLamp();
void getSettings();
long getRandomTime();
void autoPilotVent();
void callback();
String getSensorsDataAsJSON();
String fechaYhora();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setupPeripherals();  
  setupWifi();
  setupTimerIntervals();
}

// WIFI SETUP

void setupWifi() {
  WiFi.begin(Constants::SSID, Constants::PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());


}

/**************************************
 * gets sensor data as JSON string and
 * sends to server via POST method
 * ************************************/
void sendPayloadToServer(){
    if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;
    // configure traged server and url
    http.begin(client, Constants::URL); //HTTP
    http.addHeader("Content-Type", "application/json");
    String data = getSensorsDataAsJSON();
    Serial.print("[HTTP] POST...\n" + data);
    // start connection and send HTTP header and body
    int httpCode = http.POST(data);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        built_in.blink();
        const String& payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

}

/**
 * @returns JsonDocument containing all sensor data
 * as a json serialized string
 * */

String getSensorsDataAsJSON() {
  Serial.println("Getting all sensors data...");
  SimpleMap<String, float> map = sensorAire.getData();
  int soilHum = sensorMaceta.getDataSuelo();

  const size_t capacity = JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(5);
  DynamicJsonDocument doc(capacity);
  JsonObject data = doc.createNestedObject("data");
  data[Constants::HUM_AIR] = map.get(Constants::HUM_AIR);
  data[Constants::TEMP] = map.get(Constants::TEMP);
  data[Constants::HUM_SOIL] = soilHum;

  String buffer = "";
  serializeJson(doc, buffer);
  return buffer;
}

// DEVICE INIT
void setupPeripherals() {
  lampara.begin();
  ventilador.begin();
  intractor.begin();
  extractor.begin();
  sensorAire.begin();
  sensorMaceta.begin();
  waterPump.begin();
  built_in.begin();
}

/********************************************************************
 * SERVIDOR DE RELOJ
 *******************************************************************/
String fechaYhora() {
  String fechayhora = "";
  clienteReloj.update();
  fechayhora = diaSemana[clienteReloj.getDay()] + (String) ", " + clienteReloj.getFormattedTime();
  Serial.println(fechayhora);
  return fechayhora;
}

long getRandomTime() {
  return (random(2, 15)) * 1000;
}

void checkEnvironment() {
  SimpleMap<String, float> currentData = sensorAire.getData();
  float humidity = currentData.get(Constants::HUM_AIR);
  
  if (humidity < aireSeco)   // DRY CONDITIONS
  {
    Serial.println("DRY CONDITIONS");
    autoVent.pause(true);
    extractor.off();
    intractor.off();
    ventilador.on();
    //TODO implement notification
  }
  else if (humidity > aireHum)  //WET CONDITIONS
  {
    Serial.println("WET CONDITIONS");
    autoVent.pause(true);
    intractor.on();
    extractor.on();
    ventilador.on();
    //TODO implement notification
  }
  else if ((humidity >= aireSeco) && (humidity < aireHum))    //NORMAL OPERATION
  {
    Serial.println("NORMAL OPERATION");
    autoVent.pause(false);
    extractor.on();
    intractor.off();
    //TODO implement notification
  }
}

void checkLamp() {
  if (!autoLamp.isWorking()) { autoLamp.setRunning(true); }
  autoLamp.startAP();
}

void autoPilotVent() {

    if (!autoVent.isPaused()){

      if (!autoVent.isWorking()) {

            autoVent.setStart();
            autoVent.setRunning(true);
            ventiON = getRandomTime();
            ventiOFF = getRandomTime();
            autoVent.setTime(ventiON, ventiOFF);
          }
        autoVent.runForTime(callback);
    }
}

void callback() {
  Serial.println("auto pilot cycle ended");
  autoVent.setRunning(false);
}

void checkSoilWatering() {
  int soilHumdity = sensorMaceta.getDataSuelo();
  if (soilHumdity <= tierraSeca) {
    //TODO implement START watering cycle and notification
  } else {
    //TODO implement STOP watering cycle and notification
  }
}


/**
 * SimpleTimer Interval setup*/
void setupTimerIntervals() {
  timer.setInterval(check_env, checkEnvironment);
  timer.setInterval(check_auto_pilot, autoPilotVent);
  timer.setInterval(check_lamp, checkLamp);
  // timer.setInterval(check_water, checkSoilWatering);
  timer.setInterval(send_payload, sendPayloadToServer);
  // timer.setInterval(check_settings, getSettings);
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.run();
}