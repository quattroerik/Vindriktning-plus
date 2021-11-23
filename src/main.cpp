//parts of the code are orginal or modified from Hypfer Sören Beye  https://github.com/Hypfer/esp8266-vindriktning-particle-sensor 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <secret.h>
#include <types.h>
#include <PubSubClient.h>
#include "aht21Sensor.h"
#include "ir_com.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <SerialCom.h>

char identifier[24];
#define mqtt_PREFIX "tele"
const char* Name = SECRET_Name;
#define AVAILABILITY_ONLINE "online"
#define AVAILABILITY_OFFLINE "offline"
char MQTT_TOPIC_AVAILABILITY[128];
char MQTT_TOPIC_STATE[128];
char MQTT_TOPIC_COMMAND[128];


char MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_PM25_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_TEMPERATURE_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_HUMIDITY_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_MOTION_SENSOR[128];

const char* swVersion = "beta.1";
const char* ssid = SECRET_SSID;
const char* wifi_password = SECRET_WIFI_PASSWORD;
const char* mqtt_server = SECRET_MQTT_SERVER;
const int mqtt_port = SECRET_MQTT_PORT;
const char* mqtt_user = SECRET_MQTT_USER;
const char* mqtt_password = SECRET_MQTT_PASSWORD;
const char* mqtt_client_id = SECRET_MQTT_CLIENT_ID;

uint32_t lastSensorCheckInterval = 0;
const uint16_t SensorCheckInterval = 10000; //30000 = 30 second

uint32_t lastSensorCheckInterval_small = 0;
const uint16_t SensorCheckInterval_small = 1000; //1000 = 1 second

uint32_t lastSensorCheckInterval_mid = 0;
const uint16_t SensorCheckInterval_mid = 10000; //10000 = 10 second

ahtSensorState stateAHT;
irSensorState stateIR;
mqttDataSet dataSet;
particleSensorState_t statePM25;

//Var for Wifi client
WiFiClient wifiClient;
//Var for MQTT client
PubSubClient client(wifiClient);

// Connect to WIFI with credentials specified in SECRET
void connect_wifi() {
  delay(15);
  // Connect to WIFI
  WiFi.begin(ssid, wifi_password);
  // Ensure wifi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void setupAutoConfig() {
    snprintf(identifier, sizeof(identifier), "ESP-%X", ESP.getChipId());
    snprintf(MQTT_TOPIC_AVAILABILITY, 127, "%s/%s/LWT", mqtt_PREFIX, identifier);
    snprintf(MQTT_TOPIC_STATE, 127, "%s/%s/STATE", mqtt_PREFIX, identifier);
    snprintf(MQTT_TOPIC_COMMAND, 127, "%s/%s/command", mqtt_PREFIX, identifier);

    snprintf(MQTT_TOPIC_AUTOCONF_PM25_SENSOR, 127, "homeassistant/sensor/%s_pm25/config", identifier);
    snprintf(MQTT_TOPIC_AUTOCONF_WIFI_SENSOR, 127, "homeassistant/sensor/%s_wifi/config", identifier);
    snprintf(MQTT_TOPIC_AUTOCONF_HUMIDITY_SENSOR, 127, "homeassistant/sensor/%s_humidity/config", identifier);
    snprintf(MQTT_TOPIC_AUTOCONF_TEMPERATURE_SENSOR, 127, "homeassistant/sensor/%s_temperature/config", identifier);
    snprintf(MQTT_TOPIC_AUTOCONF_MOTION_SENSOR, 127, "homeassistant/sensor/%s_motion/config", identifier);
}

void autoConfigMqtt() {

    char mqttPayload[2048];
    DynamicJsonDocument device(256);
    DynamicJsonDocument autoconfPayload(1024);
    StaticJsonDocument<64> identifiersDoc;
    JsonArray identifiers = identifiersDoc.to<JsonArray>();

    identifiers.add(identifier);

    device["identifiers"] = identifiers;
    device["manufacturer"] = "quattroerik";
    device["model"] = "Vindriktning-plus";
    device["name"] = identifier;
    device["sw_version"] = swVersion;

    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["name"] = "WiFi";
    autoconfPayload["value_template"] = "{{value_json.wifi.rssi}}";
    autoconfPayload["unique_id"] = identifier + String("_wifi");
    autoconfPayload["unit_of_measurement"] = "dBm";
    autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["json_attributes_template"] = "{\"ssid\": \"{{value_json.wifi.ssid}}\", \"ip\": \"{{value_json.wifi.ip}}\"}";
    autoconfPayload["icon"] = "mdi:wifi";

    serializeJson(autoconfPayload, mqttPayload);
    client.publish(&MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[0], &mqttPayload[0], true);
    autoconfPayload.clear();


    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["name"] = "PM25";
    autoconfPayload["value_template"] = "{{value_json.pm25}}";
    autoconfPayload["unique_id"] = identifier + String("_pm25");
    autoconfPayload["unit_of_measurement"] = "μg/m³";
    autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["icon"] = "mdi:air-filter";

    serializeJson(autoconfPayload, mqttPayload);
    client.publish(&MQTT_TOPIC_AUTOCONF_PM25_SENSOR[0], &mqttPayload[0], true);
    autoconfPayload.clear();


    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["name"] = "Temperature";
    autoconfPayload["value_template"] = "{{value_json.temperature}}";
    autoconfPayload["unique_id"] = identifier + String("_temperature");
    autoconfPayload["unit_of_measurement"] = "°C";
    autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["icon"] = "mdi:thermometer";

    serializeJson(autoconfPayload, mqttPayload);
    client.publish(&MQTT_TOPIC_AUTOCONF_TEMPERATURE_SENSOR[0], &mqttPayload[0], true);
    autoconfPayload.clear();


    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["name"] = "Humidity";
    autoconfPayload["value_template"] = "{{value_json.humidity}}";
    autoconfPayload["unique_id"] = identifier + String("_humidity");
    autoconfPayload["unit_of_measurement"] = "rH%";
    autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["icon"] = "mdi:water-percent";
    
    serializeJson(autoconfPayload, mqttPayload);
    client.publish(&MQTT_TOPIC_AUTOCONF_HUMIDITY_SENSOR[0], &mqttPayload[0], true);
    autoconfPayload.clear();


    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["name"] = "Motion";
    autoconfPayload["value_template"] = "{{value_json.motion}}";
    autoconfPayload["unique_id"] = identifier + String("_motion");
    autoconfPayload["unit_of_measurement"] = "ON/OFF";
    autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["icon"] = "mdi:motion-sensor";
    
    serializeJson(autoconfPayload, mqttPayload);
    client.publish(&MQTT_TOPIC_AUTOCONF_MOTION_SENSOR[0], &mqttPayload[0], true);
    autoconfPayload.clear();


}

void publishState() {
    DynamicJsonDocument wifiJson(192);
    DynamicJsonDocument stateJson(604);
    char payload[256];

    wifiJson["ssid"] = WiFi.SSID();
    wifiJson["ip"] = WiFi.localIP().toString();
    wifiJson["rssi"] = WiFi.RSSI();
    stateJson["temperature"] = stateAHT.temperature;
    stateJson["humidity"] = stateAHT.humidity;
    stateJson["pm25"] = statePM25.avgPM25;
    stateJson["motion"] = stateIR.enabled;
    stateJson["sw_version"] = swVersion;

    stateJson["wifi"] = wifiJson.as<JsonObject>();
    serializeJson(stateJson, payload);
    client.publish(&MQTT_TOPIC_STATE[0], &payload[0], true);
}

// This function is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* payload, unsigned int length) {}

//Connect to MQTT broker and subscribe topics
void connect_MQTT() {
  // Ensure connection to MQTT broker
  while (!client.connected()) {
    //Connect to MQTT broker with credientials specified in SECRET
    if(client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      
      autoConfigMqtt();
      // Make sure to subscribe after polling the status so that we never execute commands with the default data
      client.subscribe(MQTT_TOPIC_COMMAND);
      break;
    } 
    else {
      delay(5000);
    }
  }
}
void setup() {
  //pinMode(LED_BUILTIN, OUTPUT); // The built-in LED is initialized as an output

  Serial.begin(115200);
  //initialize the serial port for reading the vindriktning pm25 sensor values
  SerialCom::setup();

  connect_wifi();
  
  //setup OTA

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
      } else { // U_FS
        type = "filesystem";
      }

    });
    ArduinoOTA.onEnd([]() {
      //Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      //Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        //Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        //Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        //Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        //Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        //Serial.println("End Failed");
      }
    });
  ArduinoOTA.begin();

  ahtRead::setup();
  IRread::setup();
  
  //initalize timestamp for movement
  stateIR.timestamp = millis();

  IRread::parseIR(stateIR);
  ahtRead::parseAHT(stateAHT);
  
  //mqtt client
  setupAutoConfig();

  client.setServer(mqtt_server, mqtt_port); //set mqtt server
  client.setKeepAlive(10);
  client.setBufferSize(2048);
  client.setCallback(callback); //set callback method for incoming mqtt topics
  client.connect(mqtt_client_id, mqtt_user, mqtt_password);
  client.publish(MQTT_TOPIC_AVAILABILITY, AVAILABILITY_ONLINE, true);
  autoConfigMqtt();
  //Serial.begin(9600);
  //Serial.print("start publish");
}

void loop() {
  
  //check wifi status, if connection lost -> reconnect
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  //check for update
  ArduinoOTA.handle();

  //check mqtt connection, if connection lost -> reconnect
  //toDo: looping in case MQTT broker is not available (on other hand, display will not do anything...)
  connect_MQTT();

  //every 30 seconds publish data
  const uint32_t currentMillis = millis();
  if (currentMillis - lastSensorCheckInterval >= SensorCheckInterval) {
    lastSensorCheckInterval = currentMillis;
    ahtRead::parseAHT(stateAHT);

    publishState();
  }

  //every 1 seconds check IR
  if (currentMillis - lastSensorCheckInterval_small >= SensorCheckInterval_small) {
    lastSensorCheckInterval_small = currentMillis;
    IRread::parseIR(stateIR);
    
    //only publish here in case motion is detected
    if(stateIR.lastState != stateIR.enabled) {
      publishState();
    }
  }

  //every 10 seconds check IR
  if (currentMillis - lastSensorCheckInterval_mid >= SensorCheckInterval_mid) {
    lastSensorCheckInterval_mid = currentMillis;
    SerialCom::handleUart(statePM25);
  }
}