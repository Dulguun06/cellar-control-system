package num.edu.cellar.controller;

import lombok.RequiredArgsConstructor;
import num.edu.cellar.model.SensorData;
import num.edu.cellar.repository.SensorDataRepository;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/api/sensor")
@RequiredArgsConstructor
public class SensorDataController {

    private final SensorDataRepository repository;

    @GetMapping("/all")
    public List<SensorData> getAllData() {
        return repository.findAll();
    }

    @GetMapping("/latest")
    public SensorData getLatestData() {
        return repository.findTopByOrderByTimestampDesc();
    }
}
