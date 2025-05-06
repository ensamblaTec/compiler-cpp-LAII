package org.example.automatas2_rgg.controllers;

import javafx.beans.property.SimpleStringProperty;
import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import org.example.automatas2_rgg.models.SymbolTableModel;
import org.example.automatas2_rgg.services.CompilerService;
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

    @FXML private TableView<SymbolTableModel> tablaSimbolos;
    @FXML private TableColumn<SymbolTableModel, String> colIdentificador;
    @FXML private TableColumn<SymbolTableModel, String> colTipo;
    @FXML private TableColumn<SymbolTableModel, String> colValor;
    @FXML private TableColumn<SymbolTableModel, String> colColumna;
    @FXML private TableColumn<SymbolTableModel, String> colLinea;

    @FXML private Label welcomeText;
    @FXML private CodeArea codigoFuente;
    @FXML private TextArea consoleOutput;
    @FXML private TextArea codigoIntermedio;
    @FXML private TextArea codigoEnsamblador;
    @FXML private ArbolDerivacionController arbolDerivacionController;

    @FXML private Button runButton;

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
        if (stage != null && fileUtils != null) {
            fileUtils = new FileUtils(stage, codigoFuente);
        }
        Path basePath = Paths.get(System.getProperty("user.dir")).getParent();
        compilerService = new CompilerService(basePath);
        codigoFuente.textProperty().addListener((observable, oldValue, newValue) -> {
            modificado = !newValue.equals(contenidoGuardado);
            actualizarTituloVentana();
        });

        inicializarColumnasTabla();
//        cargarArchivoEnCodeArea(codigoFuente, "codigo_fuente.txt");
//        cargarArchivoEnTextArea(codigoIntermedio, "codigo_intermedio.txt");
//        cargarArchivoEnTextArea(codigoEnsamblador, "codigo_ensamblador.txt");
//        cargarArchivoEnTextArea(tablaSimbolos, "tabla_simbolos.txt");
        // cargarArbolDeDerivacion();
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
                if (datos.length == 5) {
                    tablaSimbolos.getItems().add(new SymbolTableModel(datos[0], datos[1], datos[2], datos[3], datos[4]));
                }
            }
        } catch (IOException e) {
            mostrarAlerta("Error", "No se pudo leer el archivo CSV: " + e.getMessage(), Alert.AlertType.ERROR);
        }
    }


    private void cargarArchivoEn(TextInputControl destino, Path ruta) {
        try {
            if (Files.exists(ruta)) {
                String contenido = Files.readString(ruta, StandardCharsets.UTF_8);
                destino.setText(contenido);
            } else {
                String mensaje = "No se encontró el archivo: " + ruta.getFileName();
                destino.setText(mensaje);
                consoleOutput.appendText(mensaje + "\n");
            }
        } catch (IOException e) {
            String mensaje = "❌ Error al leer " + ruta.getFileName() + ":\n" + e.getMessage();
            destino.setText(mensaje);
            consoleOutput.appendText(mensaje + "\n");
        }
    }

    private void cargarSalidasGeneradas() {
        Path base = Paths.get(System.getProperty("user.dir")).resolve("../build/dist/output").normalize();

        cargarTablaSimbolosDesdeCSV(base.resolve("tabla_simbolos.txt"));

        Path rutaAstJson = base.resolve("ast.json");

        if (arbolDerivacionController != null) {
            arbolDerivacionController.cargarArbolDesdeJson(rutaAstJson);
        } else {
            consoleOutput.appendText("\n⚠ No se pudo cargar el árbol de derivación (no se encontró el controlador).");
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


}

