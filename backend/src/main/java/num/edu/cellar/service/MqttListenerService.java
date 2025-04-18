package num.edu.cellar.service;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import jakarta.annotation.PostConstruct;
import lombok.RequiredArgsConstructor;
import num.edu.cellar.controller.WebSocketController;
import num.edu.cellar.model.RelayStatus;
import num.edu.cellar.model.SensorData;
import num.edu.cellar.repository.RelayStatusRepository;
import num.edu.cellar.repository.SensorDataRepository;
import org.eclipse.paho.client.mqttv3.*;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;

@Service
@RequiredArgsConstructor
public class MqttListenerService {

    private final MqttClient mqttClient;
    private final WebSocketController webSocketController;
    private final SensorDataRepository repository;
    private final RelayStatusRepository relayStatusRepository;
    private final ObjectMapper objectMapper = new ObjectMapper();

    @PostConstruct
    public void subscribeToTopic() throws MqttException {
        mqttClient.subscribe("esp32/data", this::handleSensorData);
        mqttClient.subscribe("esp32/relay/status", this::handleRelayStatus);
    }

    private void handleSensorData(String topic, MqttMessage message) {
        {
            String payload = new String(message.getPayload());
            System.out.println("Received: " + payload);

            try {
                JsonNode root = objectMapper.readTree(payload);
                JsonNode sensor = root.get("sensor");

                if (sensor != null && sensor.has("temp") && sensor.has("hum")) {
                    SensorData data = SensorData.builder()
                            .sensorId(1) // fixed since only one sensor is used
                            .temperature((float) sensor.get("temp").asDouble())
                            .humidity((float) sensor.get("hum").asDouble())
                            .timestamp(LocalDateTime.now())
                            .build();

                    repository.save(data);
                    webSocketController.sendSensorDataUpdate(data);
                    System.out.println("Sensor data saved to DB.");
                } else {
                    System.err.println("Invalid sensor data format.");
                }
            } catch (Exception e) {
                System.err.println("Failed to parse or save: " + e.getMessage());
            }
        }
    }
    private void handleRelayStatus(String topic, MqttMessage message) {
        String payload = new String(message.getPayload());
        System.out.println("Relay Status Received: " + payload);

        try {
            JsonNode root = objectMapper.readTree(payload);

            RelayStatus status = RelayStatus.builder()
                    .heater(root.get("heater").asBoolean())
                    .cooler(root.get("cooler").asBoolean())
                    .humidifier(root.get("humidifier").asBoolean())
                    .ventilation(root.get("ventilation").asBoolean())
                    .timestamp(LocalDateTime.now())
                    .build();

            relayStatusRepository.save(status);
            webSocketController.sendRelayStatusUpdate(status);
            System.out.println("Relay status saved.");
        } catch (Exception e) {
            System.err.println("Failed to parse relay status: " + e.getMessage());
        }
    }
}
