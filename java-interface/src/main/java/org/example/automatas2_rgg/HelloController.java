package org.example.automatas2_rgg;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.control.TreeItem;
import javafx.stage.Stage;
import org.fxmisc.richtext.CodeArea;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class HelloController {
    @FXML private Label welcomeText;
    @FXML private CodeArea codigoFuente;
    @FXML private TextArea codigoIntermedio;
    @FXML private TextArea codigoEnsamblador;
    @FXML private TextArea tablaSimbolos;
    @FXML private ArbolDerivacionController arbolDerivacionController;



    @FXML
    public void initialize() {
        cargarArchivoEnCodeArea(codigoFuente, "codigo_fuente.txt");
        cargarArchivoEnTextArea(codigoIntermedio, "codigo_intermedio.txt");
        cargarArchivoEnTextArea(codigoEnsamblador, "codigo_ensamblador.txt");
        cargarArchivoEnTextArea(tablaSimbolos, "tabla_simbolos.txt");
        // cargarArbolDeDerivacion();

    }



    @FXML
    protected void onHelloButtonClick() {

        welcomeText.setText("Welcome to JavaFX Application!");
    }

    private void cargarArchivoEnCodeArea(CodeArea codeArea, String nombreArchivo) {
        try (InputStream is = getClass().getResourceAsStream("/data/" + nombreArchivo);
             BufferedReader reader = new BufferedReader(new InputStreamReader(is))) {

            StringBuilder contenido = new StringBuilder();
            String linea;
            while ((linea = reader.readLine()) != null) {
                contenido.append(linea).append("\n");
            }
            codeArea.replaceText(contenido.toString());
        } catch (IOException e) {
            codeArea.replaceText("Error cargando archivo: " + nombreArchivo);
            e.printStackTrace();
        }
    }

    private void cargarArchivoEnTextArea(TextArea textArea, String nombreArchivo) {
        try (InputStream is = getClass().getResourceAsStream("/data/" + nombreArchivo);
             BufferedReader reader = new BufferedReader(new InputStreamReader(is))) {

            StringBuilder contenido = new StringBuilder();
            String linea;
            while ((linea = reader.readLine()) != null) {
                contenido.append(linea).append("\n");
            }
            textArea.setText(contenido.toString());

        } catch (IOException | NullPointerException e) {
            textArea.setText("Error cargando archivo: " + nombreArchivo);
            e.printStackTrace();
        }
    }

    @FXML
    private void mostrarArbolDeDerivacion() {
        try {
            FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("/org/example/automatas2_rgg/ArbolDerivacion.fxml"));
            Parent root = fxmlLoader.load();

            Stage stage = new Stage();
            stage.setTitle("Árbol de Derivación");
            stage.setScene(new Scene(root));
            stage.show();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void cargarArbolDeDerivacion() {
        TreeItem<String> root = new TreeItem<>("Programa");

        TreeItem<String> decls = new TreeItem<>("Declaraciones");
        decls.getChildren().addAll(
                new TreeItem<>("int x"),
                new TreeItem<>("float y")
        );

        TreeItem<String> instrucciones = new TreeItem<>("Instrucciones");
        instrucciones.getChildren().addAll(
                new TreeItem<>("x = 10"),
                new TreeItem<>("y = x + 2.5")
        );

        root.getChildren().addAll(decls, instrucciones);
        root.setExpanded(true);
    }

    public void ejemploAgregarNodo() {
        arbolDerivacionController.agregarNodo("Java Tutorials", "Java Streams");
    }

}

