package num.edu.cellar.controller;

import lombok.RequiredArgsConstructor;
import num.edu.cellar.model.RelayStatus;
import num.edu.cellar.repository.RelayStatusRepository;
import num.edu.cellar.service.MqttListenerService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api/relay")
@RequiredArgsConstructor
public class RelayStatusController {

    private final RelayStatusRepository repository;
    private final MqttListenerService mqttService;

    @GetMapping("/latest")
    public ResponseEntity<RelayStatus> getLatestStatus() {
        RelayStatus latest = repository.findTopByOrderByTimestampDesc();
        return ResponseEntity.ok(latest);
    }

    // Endpoint to control the relays manually
    @PostMapping("/manual-control")
    public ResponseEntity<String> manualControl(@RequestBody RelayStatus relayControlDto) {
        try {
            // Publish relay control commands to MQTT broker
            mqttService.publishRelayControl(relayControlDto.isHeater(), relayControlDto.isCooler(), relayControlDto.isHumidifier(), relayControlDto.isVentilation());
            return ResponseEntity.ok("Manual relay control updated.");
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body("Error controlling relays.");
        }
    }

    // Endpoint to switch between auto and manual mode
    @PostMapping("/switch-mode")
    public ResponseEntity<String> switchMode(@RequestParam boolean isAutoMode) {
        try {
            // Publish mode switch to MQTT broker
            mqttService.publishModeSwitch(isAutoMode);
            return ResponseEntity.ok("Mode switched successfully.");
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body("Error switching mode.");
        }
    }
}
