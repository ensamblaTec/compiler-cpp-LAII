package org.example.automatas2_rgg.utils;

import javafx.stage.FileChooser;
import javafx.stage.Stage;
import org.fxmisc.richtext.CodeArea;

import java.io.*;
import java.nio.charset.StandardCharsets;

public class FileUtils {

    private final Stage stage;
    private final CodeArea codeArea;

    public FileUtils(Stage stage, CodeArea codeArea) {
        this.stage = stage;
        this.codeArea = codeArea;
    }

    public void abrirArchivo() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Abrir archivo");
        File file = fileChooser.showOpenDialog(stage);

        if (file != null) {
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(
                    new FileInputStream(file), StandardCharsets.UTF_8))) {
                StringBuilder contenido = new StringBuilder();
                String linea;
                while ((linea = reader.readLine()) != null) {
                    contenido.append(linea).append("\n");
                }
                codeArea.replaceText(contenido.toString());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void guardarArchivo() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Guardar archivo");
        File file = fileChooser.showSaveDialog(stage);

        if (file != null) {
            try (BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
                    new FileOutputStream(file), StandardCharsets.UTF_8))) {
                writer.write(codeArea.getText());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void limpiarCodeArea() {
        codeArea.clear();
    }


}

