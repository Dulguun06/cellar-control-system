package num.edu.cellar.repository;

import num.edu.cellar.model.SensorData;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface SensorDataRepository extends JpaRepository<SensorData, Long> {
    List<SensorData> findBySensorId(int sensorId);
    List<SensorData> findTop2ByOrderByTimestampDesc();

}
