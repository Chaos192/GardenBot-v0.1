#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MQTTConnector.h"
#include "./utils/Constants.h"

#define MQTT_BROKER       "maqiatto.com"
#define MQTT_BROKER_PORT  1883
#define MQTT_USERNAME     "manuelrg88@gmail.com"
#define MQTT_KEY          "Mg412115"
#define BASE_TOPIC        "manuelrg88@gmail.com/gardenbot"

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

boolean mqttInitCompleted = false;
String clientId = "gardenBot" + String(ESP.getChipId());

/* Incoming data callback. */
void dataCallback(char* topic, byte* payload, unsigned int length)
{
  char payloadStr[length + 1];
  memset(payloadStr, 0, length + 1);
  strncpy(payloadStr, (char*)payload, length);
  Serial.printf("Data    : dataCallback. Topic : [%s]\n", topic);
  Serial.printf("Data    : dataCallback. Payload : %s\n", payloadStr);
}

void performConnect()
{
  uint16_t connectionDelay = 5000;
  while (!mqtt.connected())
  {
    Serial.printf("Trace   : Attempting MQTT connection...\n");
    if (mqtt.connect(clientId.c_str(), MQTT_USERNAME, MQTT_KEY))
    {
      Serial.printf("Trace   : Connected to Broker.\n");

      /* Subscription to your topic after connection was succeeded.*/
      MQTTSubscribe(BASE_TOPIC);
    }
    else
    {
      Serial.printf("Error!  : MQTT Connect failed, rc = %d\n", mqtt.state());
      Serial.printf("Trace   : Trying again in %d msec.\n", connectionDelay);
      delay(connectionDelay);
    }
  }
}

boolean MQTTPublish(const char* topic, char* payload)
{
  boolean retval = false;
  if (mqtt.connected())
  {
    retval = mqtt.publish(topic, payload);
  }
  return retval;
}

boolean MQTTSubscribe(const char* topicToSubscribe)
{
  boolean retval = false;
  if (mqtt.connected())
  {
    retval = mqtt.subscribe(topicToSubscribe);
  }
  return retval;
}

boolean MQTTIsConnected()
{
  return mqtt.connected();
}

void MQTTBegin()
{
  mqtt.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  mqtt.setCallback(dataCallback);
  mqttInitCompleted = true;
}

void MQTTLoop()
{
  if(mqttInitCompleted)
  {
    if (!MQTTIsConnected())
    {
      performConnect();
    }
    mqtt.loop();
  }
}