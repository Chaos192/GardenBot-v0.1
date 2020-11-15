#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <SimpleMap.h>
#include "./model/Dispositivo.h"
#include "./model/AutoPilot.h"
#include "./model/SensorTierra.h"
#include "./model/SensorAmbiente.h"
#include "./model/EnvironmentControl.h"
#include "./utils/Constants.h"
#include "./utils/MQTTConnector.h"

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
#define aireSeco 70
#define aireHum 99
#define horaLampON 10
#define horaLampOFF 22

// DHT11
#include <DHT.h>
#define DHTPIN D1
#define DHTTYPE DHT11

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

// OPERATION GLOBALS
long send_payload = 1000 * 30;        //send payload to server every 30 minutes
long check_env = 1000 * 15;           //check environment every 15 minutes
long check_lamp = 60 * 1000;          //check lamp cycle every hour
long check_auto_pilot = 1000 * 2;     //check auto pilot cycle every 2 minutes
long check_water = 1000 * 60 * 3;     //check soil humidity every 3 hours
long check_settings = 1000 * 60 * 24; //ask server for settings every 24 hours

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
EnvironmentControl control(sensorAire, ventilador, intractor, extractor, autoVent);

//FUNCTION PROTOTYPES
void setupPeripherals();
void setDefaultSettings();
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
void mqttCallback(char*, byte*, unsigned int);
void decodeMQTTPayload(char[]);
String getSensorsDataAsJSON();
String fechaYhora();

// WIFI SETUP

void setupWifi()
{
  WiFi.begin(Constants::SSID, Constants::PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
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
void sendPayloadToServer()
{
  if ((WiFi.status() == WL_CONNECTED))
  {

    WiFiClient client;
    HTTPClient http;
    // configure traged server and url
    http.begin(client, Constants::URL); //HTTP
    http.addHeader("Content-Type", "application/json");
    String data = getSensorsDataAsJSON();
    Serial.println("[HTTP] POST...\n" + data);
    if (MQTTPublish(Constants::ENVIRONMENT, data)) {
      Serial.printf("MQTTPublish was succeeded.\n");
    }
    // start connection and send HTTP header and body
    int httpCode = http.POST(data);

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK)
      {
        built_in.blink();
        const String &payload = http.getString();
        Serial.println(payload);
      }
    }
    else
    {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}

/**
 * @returns JsonDocument containing all sensor data
 * as a json serialized string
 * */

String getSensorsDataAsJSON()
{
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
void setupPeripherals()
{
  lampara.begin();
  ventilador.begin();
  intractor.begin();
  extractor.begin();
  sensorAire.begin();
  sensorMaceta.begin();
  waterPump.begin();
  built_in.begin();
  control.start();
  control.setParams(aireSeco, aireHum);  //set default parameters
}

/********************************************************************
 * SERVIDOR DE RELOJ
 *******************************************************************/
String fechaYhora()
{
  String fechayhora = "";
  clienteReloj.update();
  fechayhora = diaSemana[clienteReloj.getDay()] + (String) ", " + clienteReloj.getFormattedTime();
  Serial.println(fechayhora);
  return fechayhora;
}

long getRandomTime()
{
  return (random(2, 15)) * 1000;
}

/***********************************************
 * HUMIDITY CONTROL USING INLINE IN/OUT FANS AND
 * OSCILLATING SIDE FAN
************************************************/

void checkEnvironment()
{
  if(control.isWorking() && !control.isPaused()) {
    String notification = control.checkEnvironment();
    Serial.println(notification);
    //TODO send notification to server
  }
}

void checkLamp()
{
  if (!autoLamp.isWorking())
  {
    autoLamp.setRunning(true);
  }
  autoLamp.startAP();
}

/***********************************************************
 * AUTOMATIC OPERATION OF OSCILLATING FAN
 * UNDER NORMAL CONDITIONS AS DEFINED BY PARAMS
************************************************************/

void autoPilotVent()
{

  if (!autoVent.isPaused())
  {

    if (!autoVent.isWorking())
    {

      autoVent.setStart();
      autoVent.setRunning(true);
      long ventiON = getRandomTime();
      long ventiOFF = getRandomTime();
      autoVent.setTime(ventiON, ventiOFF);
    }
    autoVent.runForTime(callback);
  }
}

void callback()
{
  Serial.println("auto pilot cycle ended");
  autoVent.setRunning(false);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (length > 0) {
    char payloadStr[length + 1];
    memset(payloadStr, 0, length + 1);
    strncpy(payloadStr, (char*)payload, length);
    Serial.printf("Data    : dataCallback. Topic : [%s]\n", topic);
    Serial.printf("Data    : dataCallback. Payload : %s\n", payloadStr);
    decodeMQTTPayload(payloadStr);
  }
}

void decodeMQTTPayload(char payload[]) {
  Serial.println("payload: " + (String) payload);
  //TODO deserialize payload and route accordingly
  //payload may be real time command
  //or auto pilot settings update 

  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 80;

  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, payload);
  const char* type = doc[Constants::TYPE]; // "settings" or "manual"
  String typeStr = String(type);
  JsonObject order = doc[Constants::ORDER];

  if(typeStr.equalsIgnoreCase(Constants::MANUAL)) {

    int device_id= order[Constants::DEVICE_ID];
    String action = order[Constants::ACTION];

    switch(device_id) {
      
      case 0: lampara.receiveOrder(action); break;
      
      case 1: ventilador.receiveOrder(action); break;

      case 2: intractor.receiveOrder(action); break;

      case 3: extractor.receiveOrder(action); break;

      default: Serial.println("ID no permitida o desconocida");
    }

  } else if (typeStr.equalsIgnoreCase(Constants::SETTINGS)) {

      int minHum = order[Constants::MIN_HUM];
      int maxHum = order[Constants::MAX_HUM];
      String notification = control.setParams(minHum, maxHum);
      Serial.println(notification);
      //TODO send notification to server
  }
}

void checkSoilWatering()
{
  int soilHumdity = sensorMaceta.getDataSuelo();
  if (soilHumdity <= tierraSeca)
  {
    //TODO implement START watering cycle and notification
  }
  else
  {
    //TODO implement STOP watering cycle and notification
  }
}

/*****************************
 * SimpleTimer Interval setup
 *****************************/

void setupTimerIntervals()
{
  timer.setInterval(check_env, checkEnvironment);
  timer.setInterval(check_auto_pilot, autoPilotVent);
  timer.setInterval(check_lamp, checkLamp);
  // timer.setInterval(check_water, checkSoilWatering);
  // timer.setInterval(send_payload, sendPayloadToServer);
  // timer.setInterval(check_settings, getSettings);
}

/***************************************************************************************************************
 ***************************************************************************************************************/

void setup()
{
  Serial.begin(9600);
  setupPeripherals();
  setupWifi();
  setupTimerIntervals();
  MQTTBegin();
  MQTTSetCallback(mqttCallback);
  // TODO get settings from server implementation
}

void loop()
{
  timer.run();
  MQTTLoop();
  MQTTSubscribe();
}