package num.edu.cellar.DTO;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class RelayStatusDTO {
    private boolean heater;
    private boolean cooler;
    private boolean humidifier;
    private boolean ventilation;
}
