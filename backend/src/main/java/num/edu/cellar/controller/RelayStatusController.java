package num.edu.cellar.controller;

import lombok.RequiredArgsConstructor;
import num.edu.cellar.model.RelayStatus;
import num.edu.cellar.repository.RelayStatusRepository;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api/relay")
@RequiredArgsConstructor
public class RelayStatusController {

    private final RelayStatusRepository repository;
    @GetMapping("/latest")
    public ResponseEntity<RelayStatus> getLatestStatus() {
        RelayStatus latest = repository.findTopByOrderByTimestampDesc();
        return ResponseEntity.ok(latest);
    }
}
