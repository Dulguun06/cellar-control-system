package num.edu.cellar.service;

import lombok.RequiredArgsConstructor;
import org.springframework.mail.SimpleMailMessage;
import org.springframework.mail.javamail.JavaMailSender;
import org.springframework.stereotype.Service;

@Service
@RequiredArgsConstructor
public class EmailService {
    private final JavaMailSender mailSender;
    private static final String TO = "dulguunbatbold1@outlook.com";

    public void sendAlert(String subject, String message) {
        SimpleMailMessage mail = new SimpleMailMessage();
        mail.setTo(TO);
        mail.setSubject(subject);
        mail.setText(message);
        mailSender.send(mail);
        try {
            mailSender.send(mail);
            System.out.println("✅ Email sent successfully.");
        } catch (Exception e) {
            System.err.println("❌ Failed to send email: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
