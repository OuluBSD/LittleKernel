
// 1. Hello World GUI (JavaFX)
// 2. Simple button with click -> Alert
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.Pane;
import javafx.stage.Stage;

public class JFX01_Hello extends Application {
    @Override
    public void start(Stage stage) {
        stage.setTitle("Hello World program");
        Label label = new Label("Hello world!");
        StackPane root = new StackPane(label);
        stage.setScene(new Scene(root, 320, 240));
        stage.show();
    }
}

class JFX01_Button extends Application {
    @Override
    public void start(Stage stage) {
        stage.setTitle("Button program");
        Pane root = new Pane();
        Button btn = new Button("Hello world!");
        btn.setLayoutX(30);
        btn.setLayoutY(30);
        btn.setPrefSize(100, 30);
        btn.setOnAction(e -> new Alert(AlertType.INFORMATION, "Popup message").showAndWait());
        root.getChildren().add(btn);
        stage.setScene(new Scene(root, 320, 240));
        stage.show();
    }
}
