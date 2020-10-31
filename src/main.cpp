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
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
const char *nombreRed = "FUMANCHU";
const char *pwdRed = "heyholetsgo";
ESP8266WiFiMulti WiFiMulti;


//DEFAULT SETTINGS
#define tierraSeca 0
#define tierraHum 75
#define tempAlta 26
#define tempBaja 22
#define aireSeco 40
#define aireHum 99
#define horaLampON 21
#define horaLampOFF 9




// DHT11
#include <DHT.h>
#define DHTPIN D1
#define DHTTYPE DHT11
float temp;
float hum;

// Soil Moisture Sensor
uint8_t sensorTierra = A0;
uint8_t sensorTierraVcc = D2;

// LEDS
uint8_t lamp = D8;
uint8_t vent = D7;
uint8_t extr = D6;
uint8_t intr = D5;
uint8_t builtin = BUILTIN_LED;


// IDS
#define LAMP_ID 0
#define VENT_ID 1
#define INTR_ID 2
#define EXTR_ID 3
#define FC28_ID 4
#define DHT11_ID 5

// TIMER
#include <SimpleTimer.h>
SimpleTimer timer;
const long utcOffset = -10800; 
char diaSemana[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

WiFiUDP servidorReloj;
NTPClient clienteReloj(servidorReloj, "south-america.pool.ntp.org", utcOffset);

// CLASSES
Dispositivo lampara = Dispositivo(lamp, "Lampara", LAMP_ID);
Dispositivo ventilador = Dispositivo(vent, "Ventilador", VENT_ID);
Dispositivo intractor = Dispositivo(intr, "Intractor", INTR_ID);
Dispositivo extractor = Dispositivo(extr, "Extractor", EXTR_ID);
SensorTierra sensorMaceta(sensorTierraVcc, sensorTierra, FC28_ID, "Sensor Tierra");
SensorAmbiente sensorAire = SensorAmbiente(DHTPIN, DHTTYPE, DHT11_ID, "Sensor Ambiente");
Dispositivo built_in = Dispositivo(builtin, "built-in LED", 99);

//FUNCTION PROTOTYPES
void setupPeripherals();
void setupWifi();
void homeWelcome();                            
void handleNotFound();
void sendDhtPacket();
void setupTimerIntervals();
void sendPayloadToServer();
String getSensorsDataAsJSON();

String fechaYhora();
long timestamp();




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setupPeripherals();  
  setupWifi();
  setupTimerIntervals();

}

// WIFI SETUP

void setupWifi() {
  WiFi.begin(nombreRed, pwdRed);

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

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://charr0max.pythonanywhere.com/measures"); //HTTP
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
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
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
  doc[Constants::TIMESTAMP] = timestamp();
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
  built_in.begin();
}

// /DHT PAGE
void sendDhtPacket() {
  DynamicJsonDocument doc = sensorAire.getJsonData();
  doc[Constants::TIMESTAMP] = timestamp();
  String buffer;
  serializeJson(doc, buffer);
}

/********************************************************************
 * SERVIDOR DE RELOJ
 *******************************************************************/
String fechaYhora() {
  String fechayhora = "";
  clienteReloj.update();
  fechayhora = diaSemana[clienteReloj.getDay()] + (String) ", " + clienteReloj.getHours() + ":" + clienteReloj.getMinutes();
  Serial.println(fechayhora);
  return fechayhora;
}

long timestamp() {
  long timestamp = 0;
  clienteReloj.update();
  timestamp = clienteReloj.getEpochTime();
  Serial.println(timestamp);
  return timestamp;
}

/**
 * SimpleTimer Interval setup*/
void setupTimerIntervals() {
  timer.setInterval(10000, sendPayloadToServer);
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.run();
}