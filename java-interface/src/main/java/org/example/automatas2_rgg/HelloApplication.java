package org.example.automatas2_rgg;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import org.example.automatas2_rgg.controllers.HelloController;

import java.io.IOException;

public class HelloApplication extends Application {
    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        Parent root = fxmlLoader.load();

        HelloController controller = fxmlLoader.getController();
        controller.setStage(stage);

       Scene scene = new Scene(root, 800, 600);

       //scene.getStylesheets().add(HelloApplication.class.getResource("src/main/resources/CSS/estilos.css").toExternalForm());

        stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
    }



    public static void main(String[] args) {
        launch();
    }
}