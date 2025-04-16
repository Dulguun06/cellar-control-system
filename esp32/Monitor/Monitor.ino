#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// WiFi credentials
const char *ssid = "Dudu_huurhun";
const char *password = "95552298";

// MQTT broker
const char *mqttServer = "192.168.1.7";
const int mqttPort = 1883;

// MQTT topics
const char *dataTopic = "esp32/data";
const char *configTopic = "esp32/config";

// DHT settings
#define DHTPIN1 4
#define DHTPIN2 5
#define DHTTYPE DHT11
DHT dht1(DHTPIN1, DHTTYPE);
// DHT dht2(DHTPIN2, DHTTYPE);

// Relay pins
#define HEATER1 25
// #define HEATER2 26
#define VENT1 32
// #define VENT2 33

// Default thresholds (can be updated by MQTT config)
float minTemp = 4.0, maxTemp = 15.0;
float minHum = 80.0, maxHum = 95.0;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 60000;

// Function declarations
void connectToWiFi();
void connectToMQTT();
void handleConfigUpdate(char *payload);
void applyControl(float temp, float hum, int heaterPin, int ventPin);
void sendSensorData();

void setup() {
  Serial.begin(115200);
  dht1.begin();
  // dht2.begin();

  pinMode(HEATER1, OUTPUT);
  // pinMode(HEATER2, OUTPUT);
  pinMode(VENT1, OUTPUT);
  // pinMode(VENT2, OUTPUT);
  digitalWrite(HEATER1, HIGH);
  // digitalWrite(HEATER2, HIGH);
  digitalWrite(VENT1, HIGH);
  // digitalWrite(VENT2, HIGH);

  connectToWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback([](char *topic, byte *payload, unsigned int length) {
    if (String(topic) == configTopic) {
      payload[length] = '\0';  // Make sure it's null-terminated
      handleConfigUpdate((char *)payload);
    }
  });

  connectToMQTT();
  client.subscribe(configTopic);
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

  Serial.println("Updated config:");
  Serial.printf("Temp: %.2f - %.2f | Hum: %.2f - %.2f\n", minTemp, maxTemp, minHum, maxHum);
}

void applyControl(float temp, float hum, int heaterPin, int ventPin) {
  bool cold = temp < minTemp;
  bool warm = temp > maxTemp;
  bool humid = hum > maxHum;
  bool dry = hum < minHum;

  if (cold && humid) {
    digitalWrite(heaterPin, HIGH);
    digitalWrite(ventPin, HIGH);
  } else if (cold) {
    digitalWrite(heaterPin, HIGH);
    digitalWrite(ventPin, LOW);
  } else if (warm) {
    digitalWrite(heaterPin, LOW);
    digitalWrite(ventPin, HIGH);
  } else {
    // In between, no control
    digitalWrite(heaterPin, LOW);
    digitalWrite(ventPin, LOW);
  }
}

void sendSensorData() {
  float temp1 = dht1.readTemperature();
  float hum1 = dht1.readHumidity();
  // float temp2 = dht2.readTemperature();
  // float hum2 = dht2.readHumidity();

  Serial.printf("Sensor 1: %.2f°C, %.2f%%\n", temp1, hum1);
  // Serial.printf("Sensor 2: %.2f°C, %.2f%%\n", temp2, hum2);

  applyControl(temp1, hum1, HEATER1, VENT1);
  // applyControl(temp2, hum2, HEATER2, VENT2);

  StaticJsonDocument<256> doc;
  JsonObject s1 = doc.createNestedObject("sensor1");
  // JsonObject s2 = doc.createNestedObject("sensor2");

  s1["temp"] = temp1;
  s1["hum"] = hum1;
  // s2["temp"] = temp2;
  // s2["hum"] = hum2;

  char buffer[256];
  size_t len = serializeJson(doc, buffer);
  client.publish(dataTopic, buffer, len);
}