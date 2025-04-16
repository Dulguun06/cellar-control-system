package num.edu.cellar.service;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import jakarta.annotation.PostConstruct;
import lombok.RequiredArgsConstructor;
import num.edu.cellar.model.SensorData;
import num.edu.cellar.repository.SensorDataRepository;
import org.eclipse.paho.client.mqttv3.*;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;

@Service
@RequiredArgsConstructor
public class MqttListenerService {

    private final MqttClient mqttClient;
    private final SensorDataRepository repository;
    private final ObjectMapper objectMapper = new ObjectMapper();

    @PostConstruct
    public void subscribeToTopic() throws MqttException {
        mqttClient.subscribe("esp32/data", (topic, message) -> {
            String payload = new String(message.getPayload());
            System.out.println("Received: " + payload);

            try {
                JsonNode root = objectMapper.readTree(payload);

                JsonNode s1 = root.get("sensor1");
//                JsonNode s2 = root.get("sensor2");

                SensorData data1 = SensorData.builder()
                        .sensorId(1)
                        .temperature((float) s1.get("temp").asDouble())
                        .humidity((float) s1.get("hum").asDouble())
                        .timestamp(LocalDateTime.now())
                        .build();

//                SensorData data2 = SensorData.builder()
//                        .sensorId(2)
//                        .temperature((float) s2.get("temp").asDouble())
//                        .humidity((float) s2.get("hum").asDouble())
//                        .timestamp(LocalDateTime.now())
//                        .build();

                repository.save(data1);
//                repository.save(data2);

                System.out.println("Saved both sensors to DB.");
            } catch (Exception e) {
                System.err.println("Failed to parse or save: " + e.getMessage());
            }
        });
    }
}
