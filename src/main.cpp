#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <SimpleMap.h>
#include "./model/Dispositivo.h"
#include "./model/AutoPilot.h"
#include "./model/SensorTierra.h"
#include "./model/SensorAmbiente.h"


// WIFI setup
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
const char *nombreRed = "FUMANCHU";
const char *pwdRed = "heyholetsgo";
ESP8266WebServer server(80);
String header;

//DEFAULT SETTINGS
#define tierraSeca 0
#define tierraHum 75
#define tempAlta 26
#define tempBaja 22
#define aireSeco 40
#define aireHum 99
#define horaLampON 13
#define horaLampOFF 7

//JSON LABELS
const char *HUM= "Humedad";
const char *TEMP= "Temperatura";
const char *DEVICE_ID= "Device ID";
const char *TIMESTAMP= "Time";


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

//FUNCTION PROTOTYPES
void setupPeripherals();
void setupWifi(String, String);
void homeWelcome();                            
void handleNotFound();
void sendDhtPacket();
void setupTimerIntervals();
void checkAllSensorsData();
String fechaYhora();
DynamicJsonDocument getJsonData(SimpleMap<String, float>);




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setupPeripherals();
  setupWifi(nombreRed, pwdRed);
  setupTimerIntervals();

}

/**
 * SimpleTimer Interval setup*/
void setupTimerIntervals() {
  timer.setInterval(10000, checkAllSensorsData);
}

// WIFI SETUP

void setupWifi(String ssid, String pwd) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", homeWelcome);

  server.on("/dht", sendDhtPacket);                        //endpoints

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void checkAllSensorsData() {
  //TODO get fresh measurements from sensors
  // maybe add json document upload to DB?
}

// DEVICE INIT
void setupPeripherals() {
  lampara.begin();
  ventilador.begin();
  intractor.begin();
  extractor.begin();
  sensorAire.begin();
  sensorMaceta.begin();
}


// HOME PAGE
void homeWelcome() {
  server.send(200, "text/plain", "Bienvenido a GardenBot API");
  Serial.print("home");
}

// 404 NOT FOUND
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println("404");
  
}

// /DHT PAGE
void sendDhtPacket() {
  DynamicJsonDocument doc = sensorAire.getJsonData();
  doc[TIMESTAMP] = fechaYhora();
  String buffer;
  serializeJson(doc, buffer);
  server.send(200, F("application/json"), buffer);
  Serial.print(F("dht data sent"));
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


void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  timer.run();
}