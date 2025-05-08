package org.example.automatas2_rgg.controllers;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import javafx.animation.FadeTransition;
import javafx.fxml.FXML;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;
import javafx.util.Duration;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.List;

public class ArbolDerivacionController {

    @FXML
    private TreeView<String> treeViewDerivacion;

    private TreeItem<String> rootItem;

    @FXML
    public void initialize() {
        rootItem = new TreeItem<>("Árbol de Derivación");
        rootItem.setExpanded(true);
        treeViewDerivacion.setRoot(rootItem);

        // Intentamos cargar automáticamente el JSON si existe al iniciar
        Path rutaAst = Paths.get(System.getProperty("user.dir"))
                .resolve("../output/ast.json")
                .normalize();

        cargarArbolDesdeJson(rutaAst);
    }

    public void cargarArbolDesdeJson(Path rutaJson) {
        rootItem.getChildren().clear();

        if (!Files.exists(rutaJson)) {
            rootItem.getChildren().add(new TreeItem<>("Archivo JSON no encontrado: " + rutaJson.getFileName()));
            return;
        }

        try {
            ObjectMapper mapper = new ObjectMapper();
            JsonNode raiz = mapper.readTree(rutaJson.toFile());

            if (!raiz.isArray()) {
                rootItem.getChildren().add(new TreeItem<>("Formato JSON inválido."));
                return;
            }

            for (JsonNode nodo : raiz) {
                TreeItem<String> item = procesarNodoJson(nodo);
                rootItem.getChildren().add(item);
            }

            FadeTransition ft = new FadeTransition(Duration.millis(400), treeViewDerivacion);
            ft.setFromValue(0.0);
            ft.setToValue(1.0);
            ft.play();

            expandirRecursivamente(rootItem);

        } catch (IOException e) {
            rootItem.getChildren().add(new TreeItem<>("Error al leer el JSON:\n" + e.getMessage()));
        }
    }

    private TreeItem<String> procesarNodoJson(JsonNode nodo) {
        String tipo = nodo.path("type").asText("Desconocido");
        TreeItem<String> raiz = new TreeItem<>(tipo);

        switch (tipo) {
            case "Declaration" -> {
                raiz.getChildren().add(new TreeItem<>("Nombre: " + nodo.path("name").asText()));
                raiz.getChildren().add(new TreeItem<>("Tipo: " + nodo.path("varType").asText()));
            }
            case "IfStatement" -> {
                raiz.getChildren().add(new TreeItem<>("Condición:"));
                raiz.getChildren().add(procesarNodoJson(nodo.path("condition")));

                raiz.getChildren().add(new TreeItem<>("Entonces:"));
                for (JsonNode stmt : nodo.path("thenBranch").path("statements")) {
                    raiz.getChildren().add(procesarNodoJson(stmt));
                }

                raiz.getChildren().add(new TreeItem<>("Sino:"));
                for (JsonNode stmt : nodo.path("elseBranch").path("statements")) {
                    raiz.getChildren().add(procesarNodoJson(stmt));
                }
            }
            case "WhileStatement" -> {
                raiz.getChildren().add(new TreeItem<>("Condición:"));
                raiz.getChildren().add(procesarNodoJson(nodo.path("condition")));

                raiz.getChildren().add(new TreeItem<>("Bloque:"));
                for (JsonNode stmt : nodo.path("body").path("statements")) {
                    raiz.getChildren().add(procesarNodoJson(stmt));
                }
            }
            case "Assignment" -> {
                raiz.getChildren().add(new TreeItem<>("Variable: " + nodo.path("name").asText()));
                raiz.getChildren().add(new TreeItem<>("Valor:"));
                raiz.getChildren().add(procesarNodoJson(nodo.path("value")));
            }
            case "BinaryExpr" -> {
                TreeItem<String> opNode = new TreeItem<>("Operador: " + nodo.path("op").asText());
                opNode.getChildren().add(procesarNodoJson(nodo.path("left")));
                opNode.getChildren().add(procesarNodoJson(nodo.path("right")));
                raiz.getChildren().add(opNode);
            }
            case "PrintStatement" -> {
                raiz.getChildren().add(new TreeItem<>("Valor:"));
                raiz.getChildren().add(procesarNodoJson(nodo.path("value")));
            }
            case "InputStatement" -> {
                raiz.getChildren().add(new TreeItem<>("Variable: " + nodo.path("varName").asText()));
            }
            case "VariableExpr" -> {
                raiz.setValue("Variable(" + nodo.path("name").asText() + ")");
            }
            case "NumberExpr" -> {
                raiz.setValue("Número(" + nodo.path("value").asText() + ")");
            }
            default -> {
                raiz.setValue("Tipo desconocido o no manejado: " + tipo);
            }
        }

        return raiz;
    }

    private void expandirRecursivamente(TreeItem<String> nodo) {
        nodo.setExpanded(true);
        for (TreeItem<String> hijo : nodo.getChildren()) {
            expandirRecursivamente(hijo);
        }
    }

}
