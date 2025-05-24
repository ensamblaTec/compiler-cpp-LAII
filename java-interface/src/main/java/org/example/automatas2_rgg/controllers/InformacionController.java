package org.example.automatas2_rgg.controllers;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.MenuItem;
import javafx.scene.control.TextArea;
import javafx.stage.Stage;
import org.example.automatas2_rgg.HelloApplication;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class InformacionController {

    @FXML
    private TextArea gramaticatexto;

    @FXML
    void onRegresar(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
            Parent root = loader.load();

            // Recuperar el Stage actual desde el menú
            Stage stage = (Stage) ((MenuItem) event.getSource()).getParentPopup().getOwnerWindow();

            // Crear la escena con dimensiones fijas
            Scene scene = new Scene(root, 800, 600);

            // Si deseas aplicar un CSS, descomenta esto y actualiza la ruta:
            // scene.getStylesheets().add(HelloApplication.class.getResource("css/estilos.css").toExternalForm());

            // Establecer la escena y mantener el título y tamaño fijo
            stage.setScene(scene);
            stage.setTitle("Hello!");
            stage.setResizable(false); // Esto evita que el usuario pueda cambiar el tamaño
            stage.show();

            // Opcional: Obtener el controlador, si necesitas pasarle datos
            HelloController controller = loader.getController();
            controller.setStage(stage);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}