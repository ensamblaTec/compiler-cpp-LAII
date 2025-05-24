package org.example.automatas2_rgg.controllers;

import javafx.application.Platform;
import javafx.beans.property.SimpleStringProperty;
import javafx.concurrent.Task;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import org.example.automatas2_rgg.HelloApplication;
import org.example.automatas2_rgg.models.SymbolTableModel;
import org.example.automatas2_rgg.models.TokenModel;
import org.example.automatas2_rgg.services.CompilerService;
import org.example.automatas2_rgg.utils.FileUtils;
import org.fxmisc.richtext.CodeArea;
import org.fxmisc.richtext.LineNumberFactory;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class HelloController {
    public Label consolePrompt;
    @FXML private MenuItem nuevo;
    @FXML private MenuItem cargar;
    @FXML private MenuItem guardar;

    @FXML private TableView<SymbolTableModel> tablaSimbolos;
    @FXML private TableColumn<SymbolTableModel, String> colIdentificador;
    @FXML private TableColumn<SymbolTableModel, String> colTipo;
    @FXML private TableColumn<SymbolTableModel, String> colValor;
    @FXML private TableColumn<SymbolTableModel, String> colColumna;
    @FXML private TableColumn<SymbolTableModel, String> colLinea;
    @FXML private TableColumn<SymbolTableModel, String> colCategoria;
    @FXML private TableColumn<SymbolTableModel, String> colScope;

    @FXML private TableView<TokenModel> tablaTokens;
    @FXML private TableColumn<TokenModel, String> colTipoToken;
    @FXML private TableColumn<TokenModel, String> colValorToken;
    @FXML private TableColumn<TokenModel, String> colColumnaToken;
    @FXML private TableColumn<TokenModel, String> colLineaToken;


    @FXML private Label welcomeText;
    @FXML private CodeArea codigoFuente;
    @FXML private TextArea consoleOutput;
    @FXML private TextArea codigoIntermedio;
    @FXML private TextArea codigoEnsamblador;

    @FXML private Button runButton;

    @FXML private AnchorPane arbolDerivacionWrapper; // este es el fx:id del fx:include

    private ArbolDerivacionController arbolDerivacionController;

    private File archivoActual = null;
    private String contenidoGuardado = "";
    private long ultimaModificacionArchivo = 0;
    private boolean modificado = false;

    private CompilerService compilerService;

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

        consolePrompt.visibleProperty().bind(consoleOutput.textProperty().isEmpty());
        if (stage != null && fileUtils != null) {
            fileUtils = new FileUtils(stage, codigoFuente);
        }
        codigoFuente.setParagraphGraphicFactory(LineNumberFactory.get(codigoFuente));
        Path basePath = Paths.get(System.getProperty("user.dir")).getParent();
        compilerService = new CompilerService(basePath);
        codigoFuente.textProperty().addListener((observable, oldValue, newValue) -> {
            modificado = !newValue.equals(contenidoGuardado);
            actualizarTituloVentana();
        });

        inicializarColumnasTabla();
        inicializarColumnasTablaTokens();
        FXMLLoader loader = new FXMLLoader(getClass().getResource("/org/example/automatas2_rgg/ArbolDerivacion.fxml"));
        try {
            AnchorPane root = loader.load();
            VBox.setVgrow(root, Priority.ALWAYS);

            arbolDerivacionController = loader.getController();
            arbolDerivacionWrapper.getChildren().setAll(root);

            AnchorPane.setTopAnchor(root, 0.0);
            AnchorPane.setBottomAnchor(root, 0.0);
            AnchorPane.setLeftAnchor(root, 0.0);
            AnchorPane.setRightAnchor(root, 0.0);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void inicializarColumnasTabla() {
        colIdentificador.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getIdentificador()));
        colTipo.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getTipo()));
        colValor.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getValor()));
        colColumna.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getColumna()));
        colLinea.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getLinea()));

        colCategoria.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getCategoria()));
        colScope.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getScope()));

    }

    public void inicializarColumnasTablaTokens() {
        colTipoToken.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getTipo()));
        colValorToken.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getValor()));
        colColumnaToken.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getColumna()));
        colLineaToken.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getLinea()));
    }


    private void actualizarTituloVentana() {
        if (stage != null) {
            String nombre = (archivoActual != null) ? archivoActual.getName() : "Archivo no guardado";
            if (modificado) {
                nombre += " *";
            }
            stage.setTitle(nombre + " - Compilador");
        }
    }

    @FXML
    private void onAbrir() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Abrir archivo");
        fileChooser.getExtensionFilters().addAll(
                new FileChooser.ExtensionFilter("Código fuente (*.txt, *.cpp)", "*.txt", "*.cpp"),
                new FileChooser.ExtensionFilter("Todos los archivos", "*.*")
        );

        File file = fileChooser.showOpenDialog(runButton.getScene().getWindow());

        if (file != null) {
            try {
                String contenido = Files.readString(file.toPath(), StandardCharsets.UTF_8);

                // Cargar en el editor
                codigoFuente.replaceText(contenido);

                // Actualizar estado
                archivoActual = file;
                contenidoGuardado = contenido;
                modificado = false;
                ultimaModificacionArchivo = file.lastModified();

                actualizarTituloVentana();

            } catch (IOException e) {
                mostrarAlerta("Error", "No se pudo abrir el archivo:\n" + e.getMessage(), Alert.AlertType.ERROR);
            }
        }
    }

    @FXML
    private void onGuardar() {
        if (archivoActual == null) {
            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Guardar archivo");
            archivoActual = fileChooser.showSaveDialog(runButton.getScene().getWindow());
            if (archivoActual == null) return;
        }

        if (archivoActual.exists()) {
            long actualMod = archivoActual.lastModified();
            if (actualMod > ultimaModificacionArchivo) {
                Alert alerta = new Alert(Alert.AlertType.CONFIRMATION);
                alerta.setTitle("Archivo externo modificado");
                alerta.setHeaderText("El archivo fue modificado fuera del editor.");
                alerta.setContentText("¿Deseas sobrescribirlo de todas formas?");
                ButtonType resultado = alerta.showAndWait().orElse(ButtonType.CANCEL);

                if (resultado != ButtonType.OK) {
                    return;
                }
            }
        }

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(archivoActual))) {
            String contenido = codigoFuente.getText();
            writer.write(contenido);

            contenidoGuardado = contenido;
            modificado = false;
            ultimaModificacionArchivo = archivoActual.lastModified();

            actualizarTituloVentana();

        } catch (IOException e) {
            mostrarAlerta("Error", "No se pudo guardar el archivo:\n" + e.getMessage(), Alert.AlertType.ERROR);
        }
    }

    @FXML
    void onSalir(ActionEvent event) {
        Platform.exit();
        System.exit(0); // Opcional, para asegurarse de cerrar completamente la aplicación
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
            mostrarAlerta("Error", "El código está vacío.", Alert.AlertType.ERROR);
            return;
        }

        if (archivoActual == null) {
            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Guardar archivo antes de compilar");
            fileChooser.getExtensionFilters().addAll(
                    new FileChooser.ExtensionFilter("Código fuente (*.txt, *.cpp)", "*.txt", "*.cpp"),
                    new FileChooser.ExtensionFilter("Todos los archivos", "*.*")
            );
            archivoActual = fileChooser.showSaveDialog(runButton.getScene().getWindow());

            if (archivoActual == null) {
                mostrarAlerta("Cancelado", "No se seleccionó ningún archivo para guardar. Operación cancelada.", Alert.AlertType.INFORMATION);
                return;
            }
        }

        if (archivoActual.exists()) {
            long modEnDisco = archivoActual.lastModified();
            if (modEnDisco > ultimaModificacionArchivo) {
                Alert alerta = new Alert(Alert.AlertType.CONFIRMATION);
                alerta.setTitle("Archivo externo modificado");
                alerta.setHeaderText("El archivo fue modificado");
                alerta.setContentText("¿Deseas sobrescribirlo de todas formas?");
                ButtonType respuesta = alerta.showAndWait().orElse(ButtonType.CANCEL);

                if (respuesta != ButtonType.OK) {
                    return;
                }
            }
        }

        if (modificado) {
            onGuardar();
        }

        runButton.setText("Compilando...");
        runButton.setDisable(true);

        Task<String> tareaCompilacion = getStringTask(codigo);
        new Thread(tareaCompilacion).start();
    }

    private Task<String> getStringTask(String codigo) {
        Task<String> tareaCompilacion = new Task<>() {
            @Override
            protected String call() throws Exception {
                return compilerService.compilarCodigo(codigo, archivoActual);
            }
        };

        tareaCompilacion.setOnSucceeded(event -> {
            consoleOutput.setText(tareaCompilacion.getValue());
            runButton.setText("Run");
            runButton.setDisable(false);

            cargarSalidasGeneradas();
        });

        tareaCompilacion.setOnFailed(event -> {
            Throwable ex = tareaCompilacion.getException();
            consoleOutput.setText("Error:\n" + ex.getMessage());
            runButton.setText("Run");
            runButton.setDisable(false);
        });
        return tareaCompilacion;
    }

    public void cargarTablaSimbolosDesdeCSV(Path path) {
        try (BufferedReader reader = Files.newBufferedReader(path)) {
            tablaSimbolos.getItems().clear();
            reader.readLine(); // Saltar encabezado
            String linea;
            while ((linea = reader.readLine()) != null) {
                String[] datos = linea.split(",");
                if (datos.length == 7) {
                    tablaSimbolos.getItems().add(new SymbolTableModel(datos[0], datos[1], datos[2], datos[3], datos[4],datos[5],datos[6]));
                }
            }
        } catch (IOException e) {
            mostrarAlerta("Error", "No se pudo leer el archivo CSV: " + e.getMessage(), Alert.AlertType.ERROR);
        }
    }


    private void cargarSalidasGeneradas() {
        Path base = Paths.get(System.getProperty("user.dir")).resolve("../build/dist/output").normalize();

        cargarTablaSimbolosDesdeCSV(base.resolve("tabla_simbolos.txt"));
        cargarTablaSimbolosDesdeCSV(base.resolve("tabla_tokens.txt"));
        cargarArchivoEnCodeArea(codigoIntermedio,"ir.txt");
        cargarArchivoEnCodeArea(codigoEnsamblador,"program.txt");


        Path rutaAstJson = base.resolve("ast.json");

        if (arbolDerivacionController != null) {
            arbolDerivacionController.cargarArbolDesdeJson(rutaAstJson);
        } else {
            consoleOutput.appendText("\n⚠ No se pudo cargar el árbol de derivación (no se encontró el controlador).");
        }
    }

    private void mostrarAlerta(String titulo, String mensaje, Alert.AlertType type) {
        Alert alert = new Alert(type);
        alert.setTitle(titulo);
        alert.setHeaderText(null);
        alert.setContentText(mensaje);
        alert.showAndWait();
    }

    private void cargarArchivoEnCodeArea(TextArea codeArea, String nombreArchivo) {

        Path base = Paths.get(System.getProperty("user.dir")).resolve("../build/dist/output").normalize();

        try (InputStream is = getClass().getResourceAsStream("../build/dist/output" + nombreArchivo);
             BufferedReader reader = Files.newBufferedReader(base.resolve(nombreArchivo))) {

            StringBuilder contenido = new StringBuilder();
            String linea;
            while ((linea = reader.readLine()) != null) {
                contenido.append(linea).append("\n");
            }
            codeArea.setText(contenido.toString());
        } catch (IOException e) {
            codeArea.setText("Error cargando archivo: " + nombreArchivo);
            e.printStackTrace();
        }
    }

    @FXML
    private void abrirPDF(ActionEvent event) {
        new PDFController().abrirPDF(event);
    }

    @FXML
    private void abrirPDFLexico(ActionEvent event) {
        new PDFController().abrirPDFLexico(event);
    }

    @FXML
    private void abrirPDFSemantico(ActionEvent event) {
        new PDFController().abrirPDFSemantico(event);
    }



    @FXML
    void mostrarInformacion(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(HelloApplication.class.getResource("informacion-view.fxml"));

            Parent root = loader.load();

            // Obtener el escenario actual
            Stage stage = (Stage) ((MenuItem) event.getSource()).getParentPopup().getOwnerWindow();
            stage.setScene(new Scene(root));
            stage.show();


        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @FXML
    void mostrarGramatica(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(HelloApplication.class.getResource("gramatica-view.fxml"));

            Parent root = loader.load();

            // Obtener el escenario actual
            Stage stage = (Stage) ((MenuItem) event.getSource()).getParentPopup().getOwnerWindow();
            stage.setScene(new Scene(root));
            stage.show();


        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}

