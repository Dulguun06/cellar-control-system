package num.edu.cellar.repository;

import num.edu.cellar.model.RelayStatus;
import org.springframework.data.jpa.repository.JpaRepository;

public interface RelayStatusRepository extends JpaRepository<RelayStatus, Long> {
    RelayStatus findTopByOrderByTimestampDesc();
}