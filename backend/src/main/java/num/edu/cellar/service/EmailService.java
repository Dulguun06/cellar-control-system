package num.edu.cellar.service;

import lombok.RequiredArgsConstructor;
import org.springframework.mail.SimpleMailMessage;
import org.springframework.mail.javamail.JavaMailSender;
import org.springframework.stereotype.Service;

@Service
@RequiredArgsConstructor
public class EmailService {
    private final JavaMailSender mailSender;
    private String recipientEmail = "dulguunbatbold1@outlook.com"; // Default email

    public void sendAlert(String subject, String message) {
        SimpleMailMessage mail = new SimpleMailMessage();
        mail.setTo(recipientEmail);
        mail.setSubject(subject);
        mail.setText(message);
        try {
            mailSender.send(mail);
            System.out.println("✅ Email sent successfully to " + recipientEmail);
        } catch (Exception e) {
            System.err.println("❌ Failed to send email: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public String getRecipientEmail() {
        return recipientEmail;
    }

    public void updateRecipientEmail(String newEmail) {
        this.recipientEmail = newEmail;
        System.out.println("📬 Notification email updated to: " + newEmail);
    }
}
