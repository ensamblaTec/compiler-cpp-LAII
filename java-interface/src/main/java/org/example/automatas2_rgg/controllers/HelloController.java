package org.example.automatas2_rgg;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.stage.Stage;
import org.example.automatas2_rgg.utils.FileUtils;
import org.fxmisc.richtext.CodeArea;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.stream.Collectors;

public class HelloController {
    @FXML private MenuItem nuevo;
    @FXML private MenuItem cargar;
    @FXML private MenuItem guardar;

    @FXML private Label welcomeText;
    @FXML private CodeArea codigoFuente;
    @FXML private TextArea codigoIntermedio;
    @FXML private TextArea codigoEnsamblador;
    @FXML private TextArea tablaSimbolos;
    @FXML private ArbolDerivacionController arbolDerivacionController;

    private FileUtils fileUtils;
    private Stage stage;
    public void setStage(Stage stage) {
        this.stage = stage;
        if (codigoFuente != null) {
            fileUtils = new FileUtils(stage, codigoFuente);
        }
    }

    @FXML
    public void initialize() {
        if (stage != null && fileUtils != null) {
            fileUtils = new FileUtils(stage, codigoFuente);
        }
//        cargarArchivoEnCodeArea(codigoFuente, "codigo_fuente.txt");
//        cargarArchivoEnTextArea(codigoIntermedio, "codigo_intermedio.txt");
//        cargarArchivoEnTextArea(codigoEnsamblador, "codigo_ensamblador.txt");
//        cargarArchivoEnTextArea(tablaSimbolos, "tabla_simbolos.txt");
        // cargarArbolDeDerivacion();
    }

    @FXML
    private void onAbrir() {
        fileUtils.abrirArchivo();
    }

    @FXML
    private void onGuardar() {
        fileUtils.guardarArchivo();
    }

    @FXML
    private void onLimpiar() {
        fileUtils.limpiarCodeArea();
    }

    @FXML
    protected void onHelloButtonClick() {

        welcomeText.setText("Welcome to JavaFX Application!");
    }

    @FXML
    private void onCompilar() {
        String codigo = codigoFuente.getText().trim();

        if (codigo.isEmpty()) {
            mostrarAlerta("Error", "El área de código está vacía. Escribe algo antes de compilar.", Alert.AlertType.WARNING);
            return;
        }

        try {
            String resultado = compilarCodigo(codigo);
            mostrarAlerta("Success", "Compilacion Correcta", Alert.AlertType.INFORMATION);
            mostrarResultado("Compilación exitosa:\n" + resultado);
            codigoFuente.replaceText(resultado);
        } catch (Exception e) {
            mostrarResultado("Error durante la compilación:\n" + e.getMessage());
        }
    }

    private String compilarCodigo(String codigo) throws IOException {
        // 1. Guardar el código en un archivo fuente
        Path inputPath = Paths.get("/home/ensamblatec/develop/cpp/compilator_rgg/entrada.txt");
        Files.writeString(inputPath, codigo, StandardCharsets.UTF_8);

        // 2. Ejecutar compilador C++
        ProcessBuilder pb = new ProcessBuilder("./compilador", "entrada.txt");
        pb.directory(new File("/home/ensamblatec/develop/cpp/compilator_rgg"));
        pb.redirectErrorStream(true);

        Process proceso = pb.start();

        // 3. Leer y devolver salida
        return new BufferedReader(new InputStreamReader(proceso.getInputStream()))
                .lines().collect(Collectors.joining("\n"));
    }

    private void mostrarAlerta(String titulo, String mensaje, Alert.AlertType type) {
        Alert alert = new Alert(type);
        alert.setTitle(titulo);
        alert.setHeaderText(null);
        alert.setContentText(mensaje);
        alert.showAndWait();
    }

    private void mostrarResultado(String mensaje) {
        // Puedes usar un Label o TextArea para mostrarlo en la UI
        System.out.println(mensaje); // por ahora, consola
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

