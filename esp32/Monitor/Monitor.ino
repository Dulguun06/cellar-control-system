#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define EEPROM_SIZE 4
#define CONFIG_ID_ADDR 0  // EEPROM address to store config ID


// WiFi credentials
const char *ssid = "Dudu_huurhun";
const char *password = "95552298";

// MQTT broker
const char *mqttServer = "192.168.1.11";
const int mqttPort = 1883;

// MQTT topics
const char *dataTopic = "esp32/data";
const char *configTopic = "esp32/config";
const char *relayStatusTopic = "esp32/relay/status";
const char *alertTopic = "esp32/alert";

// DHT settings
#define DHTPIN 4
#define DHTTYPE DHT11

// Relay pins
#define HEATER 25
#define COOLER 26
#define HUMIDIFIER 32
#define VENTILATION 33

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

float minTemp = 4.0, maxTemp = 15.0;
float minHum = 80.0, maxHum = 95.0;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1000;

// Store previous relay states
bool lastHeater = false;
bool lastCooler = false;
bool lastHumidifier = false;
bool lastVentilation = false;

// Alert state tracking
bool tempHighAlertSent = false;
bool tempLowAlertSent = false;
bool humHighAlertSent = false;
bool humLowAlertSent = false;

//Sensor Data
float lastTemp = NAN;
float lastHum = NAN;
const float changeThreshold = 0.1;  // Adjust based on how sensitive you want it

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  dht.begin();

  pinMode(HEATER, OUTPUT);
  pinMode(COOLER, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(VENTILATION, OUTPUT);

  // Turn everything off initially
  digitalWrite(HEATER, LOW);
  digitalWrite(COOLER, LOW);
  digitalWrite(HUMIDIFIER, LOW);
  digitalWrite(VENTILATION, LOW);

  connectToWiFi();

  configTime(0, 0, "pool.ntp.org");
  while (time(nullptr) < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Time synchronized.");

  client.setServer(mqttServer, mqttPort);
  client.setCallback([](char *topic, byte *payload, unsigned int length) {
    if (String(topic) == configTopic) {
      payload[length] = '\0';
      handleConfigUpdate((char *)payload);
    }
  });

  connectToMQTT();
  client.subscribe(configTopic);
  int storedConfigId = loadActiveConfigId();
  if (storedConfigId != 0) {
    publishActiveConfigId(storedConfigId);
  }
}

void loop() {
  if (!client.connected()) connectToMQTT();
  client.loop();

  unsigned long now = millis();
  if (now - lastSendTime >= sendInterval) {
    sendSensorData();
    lastSendTime = now;
  }
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println(" connected!");
      client.subscribe(configTopic);
    } else {
      Serial.print(" failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}
void storeActiveConfigId(int configId) {
  EEPROM.write(CONFIG_ID_ADDR, configId);
  EEPROM.commit();
  Serial.printf("Config ID %d stored in EEPROM\n", configId);
}

int loadActiveConfigId() {
  int configId = EEPROM.read(CONFIG_ID_ADDR);
  Serial.printf("Loaded config ID %d from EEPROM\n", configId);
  return configId;
}

void publishActiveConfigId(int configId) {
  StaticJsonDocument<64> doc;
  doc["activeConfigId"] = configId;

  char buffer[64];
  serializeJson(doc, buffer);
  client.publish("esp32/status/activeConfig", buffer);
  Serial.printf("Published activeConfigId: %d\n", configId);
}

void handleConfigUpdate(char *payload) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.println("Failed to parse config JSON");
    return;
  }

  minTemp = doc["minTemp"] | minTemp;
  maxTemp = doc["maxTemp"] | maxTemp;
  minHum = doc["minHum"] | minHum;
  maxHum = doc["maxHum"] | maxHum;

  int configId = doc["configId"] | 0;
  if (configId != 0) {
    storeActiveConfigId(configId);
    publishActiveConfigId(configId);
  }

  Serial.println("Updated config:");
  Serial.printf("Temp: %.2f - %.2f | Hum: %.2f - %.2f\n", minTemp, maxTemp, minHum, maxHum);
}


void applyControl(float temp, float hum) {
  bool isCold = temp < minTemp;
  bool isHot = temp > maxTemp;
  bool isHumid = hum > maxHum;
  bool isDry = hum < minHum;

  bool heater = isCold;
  bool cooler = isHot;
  bool humidifier = isDry;
  bool ventilation = isHumid;

  digitalWrite(HEATER, heater ? HIGH : LOW);
  digitalWrite(COOLER, cooler ? HIGH : LOW);
  digitalWrite(HUMIDIFIER, humidifier ? HIGH : LOW);
  digitalWrite(VENTILATION, ventilation ? HIGH : LOW);

  if (heater != lastHeater || cooler != lastCooler || humidifier != lastHumidifier || ventilation != lastVentilation) {
    sendRelayStatus(heater, cooler, humidifier, ventilation);
    lastHeater = heater;
    lastCooler = cooler;
    lastHumidifier = humidifier;
    lastVentilation = ventilation;
  }
}

void sendSensorData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  Serial.printf("Sensor: %.2f°C, %.2f%%\n", temp, hum);

  applyControl(temp, hum);
  checkAndSendAlerts(temp, hum);

  // Only send if temp or hum changed more than the threshold
  if (isnan(lastTemp) || isnan(lastHum) || abs(temp - lastTemp) > changeThreshold || abs(hum - lastHum) > changeThreshold) {

    StaticJsonDocument<256> doc;
    JsonObject sensor = doc.createNestedObject("sensor");
    sensor["temp"] = temp;
    sensor["hum"] = hum;

    char buffer[256];
    size_t len = serializeJson(doc, buffer);
    client.publish(dataTopic, buffer, len);

    Serial.println("Sensor data changed → sent to MQTT.");

    // Update previous values
    lastTemp = temp;
    lastHum = hum;
  } else {
    Serial.println("No change in sensor data → MQTT not sent.");
  }
}


void sendRelayStatus(bool heater, bool cooler, bool humidifier, bool ventilation) {
  StaticJsonDocument<128> doc;
  doc["heater"] = heater;
  doc["cooler"] = cooler;
  doc["humidifier"] = humidifier;
  doc["ventilation"] = ventilation;

  char buffer[128];
  size_t len = serializeJson(doc, buffer);
  client.publish(relayStatusTopic, buffer, len);

  Serial.println("Relay status updated:");
  Serial.println(buffer);
}

void checkAndSendAlerts(float temp, float hum) {
  StaticJsonDocument<128> alertDoc;
  bool alertTriggered = false;

  if (temp > maxTemp && !tempHighAlertSent) {
    alertDoc["type"] = "TEMP_HIGH";
    alertDoc["message"] = "Temperature is too high!";
    alertDoc["value"] = temp;
    tempHighAlertSent = true;
    alertTriggered = true;
  } else if (temp <= maxTemp) {
    tempHighAlertSent = false;
  }

  if (temp < minTemp && !tempLowAlertSent) {
    alertDoc["type"] = "TEMP_LOW";
    alertDoc["message"] = "Temperature is too low!";
    alertDoc["value"] = temp;
    tempLowAlertSent = true;
    alertTriggered = true;
  } else if (temp >= minTemp) {
    tempLowAlertSent = false;
  }

  if (hum > maxHum && !humHighAlertSent) {
    alertDoc["type"] = "HUM_HIGH";
    alertDoc["message"] = "Humidity is too high!";
    alertDoc["value"] = hum;
    humHighAlertSent = true;
    alertTriggered = true;
  } else if (hum <= maxHum) {
    humHighAlertSent = false;
  }

  if (hum < minHum && !humLowAlertSent) {
    alertDoc["type"] = "HUM_LOW";
    alertDoc["message"] = "Humidity is too low!";
    alertDoc["value"] = hum;
    humLowAlertSent = true;
    alertTriggered = true;
  } else if (hum >= minHum) {
    humLowAlertSent = false;
  }

  if (alertTriggered) {
    char alertBuffer[128];
    size_t len = serializeJson(alertDoc, alertBuffer);
    client.publish(alertTopic, alertBuffer, len);
    Serial.println("🚨 Alert sent:");
    Serial.println(alertBuffer);
  }
}
