package num.edu.cellar.controller;

import num.edu.cellar.model.SensorData;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class WebSocketController {

    private final SimpMessagingTemplate messagingTemplate;

    public WebSocketController(SimpMessagingTemplate messagingTemplate) {
        this.messagingTemplate = messagingTemplate;
    }

    // Send updates to frontend via WebSocket
    public void sendSensorDataUpdate(SensorData sensorData) {
        messagingTemplate.convertAndSend("/topic/sensorUpdates", sensorData);
    }

    // Send relay status updates
    public void sendRelayStatusUpdate(Object relayStatus) {
        messagingTemplate.convertAndSend("/topic/relayUpdates", relayStatus);
    }

}
