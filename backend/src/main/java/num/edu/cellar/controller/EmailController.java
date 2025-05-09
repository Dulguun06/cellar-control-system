package num.edu.cellar.controller;

import lombok.Getter;
import lombok.RequiredArgsConstructor;
import lombok.Setter;
import num.edu.cellar.service.EmailService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api")
@RequiredArgsConstructor
public class EmailController {
    private final EmailService emailService;

    @PostMapping("/update-notification-email")
    public ResponseEntity<String> updateEmail(@RequestBody EmailUpdateRequest request) {
        if (request.getEmail() == null || request.getEmail().isBlank()) {
            return ResponseEntity.badRequest().body("Email must not be empty.");
        }

        emailService.updateRecipientEmail(request.getEmail());
        return ResponseEntity.ok("Notification email updated successfully.");
    }

    @GetMapping("/notification-email")
    public ResponseEntity<String> getEmail() {
        return ResponseEntity.ok(emailService.getRecipientEmail());
    }

    @Getter
    @Setter
    public static class EmailUpdateRequest {
        private String email;

        public EmailUpdateRequest(String email) {
            this.email = email;
        }
    }
}
