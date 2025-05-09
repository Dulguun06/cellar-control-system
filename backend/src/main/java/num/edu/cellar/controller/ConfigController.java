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

    @PutMapping("/{id}")
    public SensorConfig updateConfig(@PathVariable Integer id, @RequestBody SensorConfig updated) {
        return repository.findById(id)
                .map(config -> {
                    config.setName(updated.getName());
                    config.setMinTemp(updated.getMinTemp());
                    config.setMaxTemp(updated.getMaxTemp());
                    config.setMinHum(updated.getMinHum());
                    config.setMaxHum(updated.getMaxHum());
                    return repository.save(config);
                })
                .orElseThrow(() -> new RuntimeException("Config not found"));
    }

    @DeleteMapping("/{id}")
    public void deleteConfig(@PathVariable Integer id) {
        repository.deleteById(id);
    }
    @GetMapping("/active")
    public SensorConfig getActiveConfig() {
        Integer id = publisher.getActiveConfigId();
        return id != null ? repository.findById(id).orElse(null) : null;
    }
}