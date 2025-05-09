package num.edu.cellar.controller;

import lombok.RequiredArgsConstructor;
import num.edu.cellar.service.EmailService;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequiredArgsConstructor
public class TestController {
    private final EmailService emailService;

    @GetMapping("/test-email")
    public String testEmail() {
        emailService.sendAlert("Test Subject", "This is a test email.");
        return "Email sent!";
    }
}
