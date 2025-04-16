#include <Arduino.h>
#include <DHT.h>

// Pin definitions
#define DHTPIN1 4
#define DHTPIN2 5
#define DHTTYPE DHT11

#define RELAY_HEATER1 25
#define RELAY_HEATER2 26
#define RELAY_VALVE1 32
#define RELAY_VALVE2 33

// DHT sensor objects
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

void setupPins() {
  pinMode(RELAY_HEATER1, OUTPUT);
  pinMode(RELAY_HEATER2, OUTPUT);
  pinMode(RELAY_VALVE1, OUTPUT);
  pinMode(RELAY_VALVE2, OUTPUT);
  digitalWrite(RELAY_HEATER1, LOW);
  digitalWrite(RELAY_HEATER2, LOW);
  digitalWrite(RELAY_VALVE1, LOW);
  digitalWrite(RELAY_VALVE2, LOW);
}

void readSensors(float &temp1, float &hum1, float &temp2, float &hum2) {
  temp1 = dht1.readTemperature();
  hum1 = dht1.readHumidity();
  temp2 = dht2.readTemperature();
  hum2 = dht2.readHumidity();
}

void controlRelay(int relayPin, bool state) {
  digitalWrite(relayPin, state ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  setupPins();
}

void loop() {
  float temp1, hum1, temp2, hum2;
  readSensors(temp1, hum1, temp2, hum2);

  Serial.printf("Sensor 1: Temp: %.2f°C, Humidity: %.2f%%\n", temp1, hum1);
  Serial.printf("Sensor 2: Temp: %.2f°C, Humidity: %.2f%%\n", temp2, hum2);

  controlRelay(RELAY_HEATER1, true);
  delay(500);
  controlRelay(RELAY_HEATER2, true);
  delay(500);
  controlRelay(RELAY_VALVE1, true);
  delay(500);
  controlRelay(RELAY_VALVE2, true);
  delay(500);
  delay(2000);
  controlRelay(RELAY_HEATER1, false);
  delay(500);
  controlRelay(RELAY_HEATER2, false);
  delay(500);
  controlRelay(RELAY_VALVE1, false);
  delay(500);
  controlRelay(RELAY_VALVE2, false);
  delay(500);
  delay(2000);
}