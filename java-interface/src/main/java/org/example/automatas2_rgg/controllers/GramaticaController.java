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

public class GramaticaController {
    @FXML
    private TextArea gramaticatexto;

    @FXML
    void initialize() {
        cargarGramatica();
    }

    private void cargarGramatica() {
        try (InputStream is = getClass().getResourceAsStream("/data/gramatica.txt")) {
            if (is == null) {
                System.out.println("Error: Archivo no encontrado en /data/gramatica.txt");
                gramaticatexto.setText("Error: No se pudo encontrar el archivo.");
                return;
            }

            BufferedReader reader = new BufferedReader(new InputStreamReader(is));
            StringBuilder contenido = new StringBuilder();
            String linea;
            while ((linea = reader.readLine()) != null) {
                contenido.append(linea).append("\n");
            }
            gramaticatexto.setText(contenido.toString());
        } catch (IOException e) {
            e.printStackTrace();
            gramaticatexto.setText("Error al cargar el archivo.");
        }
    }

    @FXML
    public void onRegresar(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
            Parent root = loader.load();

            Stage stage = (Stage) ((MenuItem) event.getSource()).getParentPopup().getOwnerWindow();

            // Crear una nueva escena con las dimensiones predefinidas
            Scene scene = new Scene(root, 800, 600);
            stage.setScene(scene);

            // Asegurar que el tamaño se mantenga
            stage.setWidth(800);
            stage.setHeight(600);

            stage.show();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
