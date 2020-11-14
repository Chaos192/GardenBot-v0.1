#include "Constants.h"

const String Constants::HUM_AIR = "rel_humidity";
const String Constants::HUM_SOIL = "soil_humidity";
const String Constants::TEMP = "temperature";
const String Constants::DEVICE_ID = "device_id";
const String Constants::DEVICE_NAME = "dev_name";
const String Constants::TIMESTAMP = "timestamp";
const String Constants::STATUS = "status";
const String Constants::URL = "http://charr0max.pythonanywhere.com/measures";
const String Constants::DATA = "data";

const String Constants::SSID = "FUMANCHU";
const String Constants::PASS = "heyholetsgo";

const String Constants::MQTT_BROKER = "maqiatto.com";
const int Constants::MQTT_BROKER_PORT = 1883;
const String Constants::MQTT_USERNAME = "manuelrg88@gmail.com";
const String Constants::MQTT_KEY = "Mg412115";

const String Constants::BASE_TOPIC = Constants::MQTT_USERNAME + "/gardenbot";
const String Constants::NOTIFICATIONS = Constants::BASE_TOPIC + "/notifications";
const String Constants::DEVICES = Constants::BASE_TOPIC + "/devices";
const String Constants::ENVIRONMENT = Constants::BASE_TOPIC + "/notifications";
