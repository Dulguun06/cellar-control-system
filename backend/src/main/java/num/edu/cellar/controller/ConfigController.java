package num.edu.cellar.controller;

import lombok.RequiredArgsConstructor;
import num.edu.cellar.model.SensorConfig;
import num.edu.cellar.repository.SensorConfigRepository;
import num.edu.cellar.service.ConfigPublisherService;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/config")
@RequiredArgsConstructor
public class ConfigController {
    private final SensorConfigRepository repository;
    private final ConfigPublisherService publisher;


    @GetMapping
    public List<SensorConfig> getAll() {
        return repository.findAll();
    }

    @PostMapping
    public SensorConfig addConfig(@RequestBody SensorConfig newConfig) {
        return repository.save(newConfig);
    }

    @PostMapping("/apply/{id}")
    public void applyConfig(@PathVariable Integer id) {
        publisher.publishConfigById(id); // Sends config to ESP32
    }
}
