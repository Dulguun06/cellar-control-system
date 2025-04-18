package num.edu.cellar.controller;

import lombok.RequiredArgsConstructor;
import num.edu.cellar.DTO.RelayStatusDTO;
import num.edu.cellar.model.RelayStatus;
import num.edu.cellar.repository.RelayStatusRepository;
import org.springframework.http.ResponseEntity;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/api/relay")
@RequiredArgsConstructor
public class RelayStatusController {

    private final RelayStatusRepository repository;
    private final SimpMessagingTemplate messagingTemplate;
    @GetMapping("/latest")
    public ResponseEntity<RelayStatus> getLatestStatus() {
        RelayStatus latest = repository.findTopByOrderByTimestampDesc();
        return ResponseEntity.ok(latest);
    }
    @PostMapping("/test-relay")
    public void testRelay(@RequestBody RelayStatusDTO status) {
        System.out.println("Received DTO: " + status);
        messagingTemplate.convertAndSend("/topic/relayUpdates", status);
    }
    @PostMapping("/test-sensor")
    public void testSensor(@RequestBody Map<String, Object> payload) {
        messagingTemplate.convertAndSend("/topic/sensorUpdates", payload);
    }
}
