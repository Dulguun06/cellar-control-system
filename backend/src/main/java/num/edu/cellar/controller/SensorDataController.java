package num.edu.cellar.controller;

import lombok.RequiredArgsConstructor;
import num.edu.cellar.model.SensorData;
import num.edu.cellar.repository.SensorDataRepository;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
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
    public List<SensorData> getLatestData() {
        return repository.findTop2ByOrderByTimestampDesc();
    }

    @GetMapping("/latest/{sensorId}")
    public SensorData getLatestBySensorId(@PathVariable int sensorId) {
        return repository.findTopBySensorIdOrderByTimestampDesc(sensorId);
    }

    @GetMapping("/sensor/{id}")
    public List<SensorData> getDataBySensor(@PathVariable int id) {
        return repository.findBySensorId(id);
    }
}
