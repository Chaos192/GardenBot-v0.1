#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Arduino.h>

class Constants
{
public:
    static const String HUM_AIR;
    static const String HUM_SOIL;
    static const String TEMP;
    static const String DEVICE_ID;
    static const String DEVICE_NAME;
    static const String TIMESTAMP;
    static const String STATUS;
    static const String URL;
    static const String DATA;

    static const String SSID;
    static const String PASS;

    static const String MQTT_BROKER;
    static const int MQTT_BROKER_PORT;
    static const String MQTT_USERNAME;
    static const String MQTT_KEY;

    static const String BASE_TOPIC;
    static const String NOTIFICATIONS;
    static const String DEVICES;
    static const String ENVIRONMENT;
};

#endif
