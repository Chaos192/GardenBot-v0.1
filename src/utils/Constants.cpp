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
const String Constants::AUTO_PILOT_MODE = "auto_pilot_mode";
const String Constants::MODE_AUTO = "auto";
const String Constants::MODE_MANUAL = "manual";

const String Constants::SSID = "FUMANCHU";
const String Constants::PASS = "heyholetsgo";

const String Constants::MQTT_USERNAME = "manuelrg88@gmail.com";
const String Constants::BASE_TOPIC = Constants::MQTT_USERNAME + "/gardenbot";
const String Constants::NOTIFICATIONS = Constants::BASE_TOPIC + "/notifications";
const String Constants::DEVICES = Constants::BASE_TOPIC + "/devices";
const String Constants::VENT = Constants::DEVICES + "/vent";
const String Constants::LAMP = Constants::DEVICES + "/lamp";
const String Constants::ENVIRONMENT = Constants::BASE_TOPIC + "/environment";

const String Constants::TYPE = "type";
const String Constants::MANUAL = "manual";
const String Constants::SETTINGS = "settings";
const String Constants::ORDER = "order";
const String Constants::ACTION = "action";

const String Constants::MIN_HUM = "min_hum";
const String Constants::MAX_HUM = "max_hum";
const String Constants::MIN_SOIL = "min_soil";
const String Constants::MAX_SOIL = "max_soil";
const String Constants::HOUR_ON = "hour_on";
const String Constants::HOUR_OFF = "hour_off";
const String Constants::CYCLE_ON = "cycle_on";
const String Constants::CYCLE_OFF = "cycle_off";

const String Constants::NOT_ID = "notification_id";
const String Constants::NOT_MSG = "message";
const String Constants::CODE_DEV = "DEV";
const String Constants::CODE_ENV = "ENV";
