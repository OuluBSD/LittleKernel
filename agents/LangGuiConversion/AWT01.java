
// 1. Hello World GUI (Swing/AWT)
// 2. Simple button with click -> JOptionPane
import javax.swing.*;
import java.awt.*;

public class AWT01_Hello {
    public static void main(String[] args) {
        EventQueue.invokeLater(() -> {
            JFrame f = new JFrame("Hello World program");
            f.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
            f.setSize(320, 240);
            JLabel lbl = new JLabel("Hello world!", SwingConstants.CENTER);
            f.add(lbl, BorderLayout.CENTER);
            f.setLocationRelativeTo(null);
            f.setVisible(true);
        });
    }
}

class AWT01_Button {
    public static void main(String[] args) {
        EventQueue.invokeLater(() -> {
            JFrame f = new JFrame("Button program");
            f.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
            f.setSize(320, 240);
            JButton btn = new JButton("Hello world!");
            btn.setBounds(30, 30, 100, 30);
            btn.addActionListener(e -> JOptionPane.showMessageDialog(f, "Popup message"));
            f.setLayout(null);
            f.add(btn);
            f.setLocationRelativeTo(null);
            f.setVisible(true);
        });
    }
}
