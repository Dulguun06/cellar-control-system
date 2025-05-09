package num.edu.cellar.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.Getter;
import lombok.RequiredArgsConstructor;
import num.edu.cellar.model.SensorConfig;
import num.edu.cellar.repository.SensorConfigRepository;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.springframework.stereotype.Service;

@Service
@RequiredArgsConstructor
public class ConfigPublisherService {
    private final MqttClient mqttClient;
    private final SensorConfigRepository configRepository;
    private final ObjectMapper objectMapper = new ObjectMapper();

    @Getter
    private Integer activeConfigId = null;

    public void publishConfigById(Integer id) {
        try {
            SensorConfig config = configRepository.findById(id).orElse(null);
            if (config == null) {
                System.err.println("Config not found with id " + id);
                return;
            }

            String json = objectMapper.writeValueAsString(config);
            mqttClient.publish("esp32/config", new MqttMessage(json.getBytes()));
            System.out.println("Published config to esp32/config: " + json);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
