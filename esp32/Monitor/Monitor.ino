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
const char *modeTopic = "esp32/mode";
const char *relayStatusTopic = "esp32/relay/status";
const char *manualControlTopic = "esp32/relay/control";
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

bool heaterStatus = false;
bool coolerStatus = false;
bool humidifierStatus = false;
bool ventilationStatus = false;

// Alert state tracking
bool tempHighAlertSent = false;
bool tempLowAlertSent = false;
bool humHighAlertSent = false;
bool humLowAlertSent = false;

//Sensor Data
float lastTemp = NAN;
float lastHum = NAN;
const float changeThreshold = 0.1;  // Adjust based on how sensitive you want it

// Control mode
bool isManualMode = false;  // Flag to track if in manual mode

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
    } else if (String(topic) == modeTopic) {
      payload[length] = '\0';
      handleModeSwitch((char *)payload);
    } else if (String(topic) == manualControlTopic) {
      payload[length] = '\0';
      handleManualControl((char *)payload);
    }
  });

  connectToMQTT();
  client.subscribe(configTopic);
  client.subscribe(modeTopic);
  client.subscribe(manualControlTopic);


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
      client.subscribe(manualControlTopic);  // Subscribe to manual control topic
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
  client.publish("esp32/config/activeConfig", buffer);
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

  int configId = doc["id"] | 0;
  if (configId != 0) {
    storeActiveConfigId(configId);
    publishActiveConfigId(configId);
  }

  Serial.println("Updated config:");
  Serial.printf("Temp: %.2f - %.2f | Hum: %.2f - %.2f\n", minTemp, maxTemp, minHum, maxHum);
}
void handleModeSwitch(char *payload) {
  StaticJsonDocument<64> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.println("Failed to parse mode switch JSON");
    return;
  }

  String mode = doc["mode"];
  if (mode == "false") {
    isManualMode = true;
    Serial.println("Switched to MANUAL mode");
  } else if (mode == "true") {
    isManualMode = false;
    Serial.println("Switched to AUTO mode");
  }
  sendRelayStatus();
}

void handleManualControl(char *payload) {
  if (!isManualMode) {
    Serial.println("Ignored manual control - not in MANUAL mode");
    return;
  }

  StaticJsonDocument<128> doc;
  if (deserializeJson(doc, payload)) {
    Serial.println("Failed to parse manual control JSON");
    return;
  }

  bool newHeater      = doc["heater"];
  bool newCooler      = doc["cooler"];
  bool newHumidifier  = doc["humidifier"];
  bool newVentilation = doc["ventilation"];

  bool changed = false;

  if (newHeater != heaterStatus) {
    digitalWrite(HEATER, newHeater ? HIGH : LOW);
    heaterStatus = newHeater;
    changed = true;
  }

  if (newCooler != coolerStatus) {
    digitalWrite(COOLER, newCooler ? HIGH : LOW);
    coolerStatus = newCooler;
    changed = true;
  }

  if (newHumidifier != humidifierStatus) {
    digitalWrite(HUMIDIFIER, newHumidifier ? HIGH : LOW);
    humidifierStatus = newHumidifier;
    changed = true;
  }

  if (newVentilation != ventilationStatus) {
    digitalWrite(VENTILATION, newVentilation ? HIGH : LOW);
    ventilationStatus = newVentilation;
    changed = true;
  }

  if (changed) {
    sendRelayStatus();
    Serial.println("Manual control applied.");
  } else {
    Serial.println("Manual control received, but no changes.");
  }
}

void applyControl(float temp, float hum) {
  if (isManualMode) return;

  bool newHeater      = temp < minTemp;
  bool newCooler      = temp > maxTemp;
  bool newHumidifier  = hum  < minHum;
  bool newVentilation = hum  > maxHum;

  bool changed = false;

  if (newHeater != heaterStatus) {
    digitalWrite(HEATER, newHeater ? HIGH : LOW);
    heaterStatus = newHeater;
    changed = true;
  }

  if (newCooler != coolerStatus) {
    digitalWrite(COOLER, newCooler ? HIGH : LOW);
    coolerStatus = newCooler;
    changed = true;
  }

  if (newHumidifier != humidifierStatus) {
    digitalWrite(HUMIDIFIER, newHumidifier ? HIGH : LOW);
    humidifierStatus = newHumidifier;
    changed = true;
  }

  if (newVentilation != ventilationStatus) {
    digitalWrite(VENTILATION, newVentilation ? HIGH : LOW);
    ventilationStatus = newVentilation;
    changed = true;
  }

  if (changed) {
    sendRelayStatus();  // Only send if anything changed
  }
}


void sendSensorData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  Serial.printf("Sensor: %.2f°C, %.2f%%\n", temp, hum);

  applyControl(temp, hum);
  checkAndSendAlerts(temp, hum);

  if (isnan(lastTemp) || isnan(lastHum) || abs(temp - lastTemp) > changeThreshold || abs(hum - lastHum) > changeThreshold) {
    StaticJsonDocument<256> doc;
    JsonObject sensor = doc.createNestedObject("sensor");
    sensor["temp"] = temp;
    sensor["hum"] = hum;

    char buffer[256];
    size_t len = serializeJson(doc, buffer);
    client.publish(dataTopic, buffer, len);

    Serial.println("Sensor data changed → sent to MQTT.");

    lastTemp = temp;
    lastHum = hum;
  }
}

void sendRelayStatus() {
  StaticJsonDocument<128> doc;
  doc["heater"] = heaterStatus;
  doc["cooler"] = coolerStatus;
  doc["humidifier"] = humidifierStatus;
  doc["ventilation"] = ventilationStatus;
  char buffer[128];
  serializeJson(doc, buffer);
  client.publish(relayStatusTopic, buffer);
}

void checkAndSendAlerts(float temp, float hum) {
  StaticJsonDocument<128> alertDoc;
  bool alertTriggered = false;

  if (temp > (maxTemp + 10) && !tempHighAlertSent) {
    alertDoc["type"] = "TEMP_HIGH";
    alertDoc["message"] = "Temperature is extremely high!";
    alertDoc["value"] = temp;
    tempHighAlertSent = true;
    alertTriggered = true;
  } else if (temp <= (maxTemp + 10)) {
    tempHighAlertSent = false;
  }

  if (temp < (minTemp - 10) && !tempLowAlertSent) {
    alertDoc["type"] = "TEMP_LOW";
    alertDoc["message"] = "Temperature is extremely low!";
    alertDoc["value"] = temp;
    tempLowAlertSent = true;
    alertTriggered = true;
  } else if (temp >= (minTemp - 10)) {
    tempLowAlertSent = false;
  }

  if (hum > (maxHum + 10) && !humHighAlertSent) {
    alertDoc["type"] = "HUM_HIGH";
    alertDoc["message"] = "Humidity is extremely high!";
    alertDoc["value"] = hum;
    humHighAlertSent = true;
    alertTriggered = true;
  } else if (hum <= (maxHum + 10)) {
    humHighAlertSent = false;
  }

  if (hum < (minHum - 10) && !humLowAlertSent) {
    alertDoc["type"] = "HUM_LOW";
    alertDoc["message"] = "Humidity is extremely low!";
    alertDoc["value"] = hum;
    humLowAlertSent = true;
    alertTriggered = true;
  } else if (hum >= (minHum - 10)) {
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
