package num.edu.cellar.model;

import jakarta.persistence.*;
import lombok.*;

import java.util.Date;

@Entity
@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
public class RelayStatus {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    private boolean heater;
    private boolean cooler;
    private boolean humidifier;
    private boolean ventilation;
    private Date timestamp;
}
