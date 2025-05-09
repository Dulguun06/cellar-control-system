package num.edu.cellar.service;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import jakarta.annotation.PostConstruct;
import lombok.RequiredArgsConstructor;
import num.edu.cellar.model.RelayStatus;
import num.edu.cellar.model.SensorData;
import num.edu.cellar.repository.RelayStatusRepository;
import num.edu.cellar.repository.SensorDataRepository;
import org.eclipse.paho.client.mqttv3.*;
import org.springframework.stereotype.Service;

import java.sql.Date;

@Service
@RequiredArgsConstructor
public class MqttListenerService {

    private final MqttClient mqttClient;
    private final SensorDataRepository repository;
    private final RelayStatusRepository relayStatusRepository;
    private final EmailService emailService;
    private final ObjectMapper objectMapper = new ObjectMapper();

    @PostConstruct
    public void subscribeToTopic() throws MqttException {
        mqttClient.subscribe("esp32/data", this::handleSensorData);
        mqttClient.subscribe("esp32/relay/status", this::handleRelayStatus);
        mqttClient.subscribe("esp32/alert", this::handleAlertMessage);
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
                            .temperature((float) sensor.get("temp").asDouble())
                            .humidity((float) sensor.get("hum").asDouble())
                            .timestamp(new Date(System.currentTimeMillis()))
                            .build();

                    repository.save(data);
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
                    .timestamp(new Date(System.currentTimeMillis()))
                    .build();
            relayStatusRepository.save(status);
            System.out.println("Relay status saved.");
        } catch (Exception e) {
            System.err.println("Failed to parse relay status: " + e.getMessage());
        }
    }

    public void handleAlertMessage(String topic, MqttMessage mqttMessage) {
        try {
            JsonNode node = objectMapper.readTree(mqttMessage.getPayload());
            String type = node.get("type").asText(); // TEMP_HIGH, HUM_LOW, etc.
            String message = node.get("message").asText(); // e.g. "Temperature is too high!"
            double value = node.get("value").asDouble();

            // Set units based on alert type
            String unit;
            if (type.startsWith("TEMP")) {
                unit = "°C";
            } else if (type.startsWith("HUM")) {
                unit = "%";
            } else {
                unit = "";
            }

            String subject = "[Cellar Alert] " + message;
            String body = String.format("""
                    🚨 ALERT DETECTED!
                    
                    Reason: %s
                    Current Value: %.2f %s
                    Time: %s
                    """, message, value, unit, new java.util.Date());

            emailService.sendAlert(subject, body);
            System.out.println("✅ Email alert sent: " + subject);
        } catch (Exception e) {
            System.err.println("❌ Failed to handle alert message: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
