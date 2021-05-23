#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Arduino.h>

class Constants
{
public:
    //OPERATION PARAMS
    static const String AIR_HUM;
    static const String SOIL_HUM;
    static const String AIR_TEMP;
    static const String DEVICE_ID;
    static const String DEVICE_NAME;
    static const String TIMESTAMP;
    static const String STATUS;
    static const String URL;
    static const String DATA;
    static const String AUTO_PILOT_MODE;
    static const String MODE_AUTO;
    static const String MODE_MANUAL;
    static const String DEVICE_ID_ADD;

    // WIFI
    static const String SSID;
    static const String PASS;

    //MQTT
    static const String MQTT_USERNAME;
    static const String BASE_TOPIC;
    static const String NOTIFICATIONS;
    static const String DEVICES;
    static const String VENT;
    static const String LAMP;
    static const String ENVIRONMENT;

    //ORDER
    static const String TYPE;
    static const String MANUAL;
    static const String SETTINGS;
    static const String ORDER;
    static const String ACTION;
    static const String DEVICE_PIN;

    static const String MIN_TEMP;
    static const String MAX_TEMP;
    static const String MIN_HUM;
    static const String MAX_HUM;
    static const String MIN_SOIL;
    static const String MAX_SOIL;
    static const String HOUR_ON;
    static const String HOUR_OFF;
    static const String CYCLE_ON;
    static const String CYCLE_OFF;

    //NOTIFICATION

    static const String CODE;
    static const String NOT_MSG;
    static const int CODE_ENV_NORMAL;
    static const int CODE_DEV;
    static const int CODE_ENV_HIGH;
    static const int CODE_DEV_LINKED;
};

#endif
