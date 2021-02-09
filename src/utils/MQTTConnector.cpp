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
String dev_id;

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
      MQTTSubscribe();
    }
    else
    {
      Serial.printf("Error!  : MQTT Connect failed, rc = %d\n", mqtt.state());
      Serial.printf("Trace   : Trying again in %d msec.\n", connectionDelay);
      delay(connectionDelay);
    }
  }
}

boolean MQTTPublish(String topic, String payload)
{
  String topic_char = dev_id + "/" + topic;
  char * payload_char = new char[payload.length() + 1];
  strcpy(payload_char, payload.c_str());
  boolean retval = false;
  if (mqtt.connected())
  {
    retval = mqtt.publish(topic_char.c_str(), payload_char);
  }
  return retval;
}

boolean MQTTSubscribe()
{
  boolean retval = false;
  if (mqtt.connected())
  {
    retval = mqtt.subscribe(Constants::DEVICES.c_str());
  }
  return retval;
}

boolean MQTTIsConnected()
{
  return mqtt.connected();
}

void MQTTBegin(String deviceId)
{
  mqtt.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  dev_id = deviceId;
  mqttInitCompleted = true;
}

void MQTTSetCallback(void (*callback)(char* topic, byte* payload, unsigned int length)) {
  mqtt.setCallback(*callback);
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