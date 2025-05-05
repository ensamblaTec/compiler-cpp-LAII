package org.example.automatas2_rgg;

import javafx.fxml.FXML;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;

public class ArbolDerivacionController {

    @FXML
    private TreeView<String> treeViewDerivacion;

    private TreeItem<String> rootItem;

    @FXML
    public void initialize() {
        rootItem = new TreeItem<>("Árbol de Derivación");
        rootItem.setExpanded(true);
        treeViewDerivacion.setRoot(rootItem);

        // Puedes agregar datos de prueba aquí si quieres ver algo al inicio
        agregarEjemploBasico();
    }

    public void agregarEjemploBasico() {
        TreeItem<String> webItem = new TreeItem<>("Web Tutorials");
        webItem.getChildren().addAll(
                new TreeItem<>("HTML Tutorial"),
                new TreeItem<>("HTML5 Tutorial"),
                new TreeItem<>("CSS Tutorial"),
                new TreeItem<>("SVG Tutorial")
        );

        TreeItem<String> javaItem = new TreeItem<>("Java Tutorials");
        javaItem.getChildren().addAll(
                new TreeItem<>("Java Language"),
                new TreeItem<>("Java Collections"),
                new TreeItem<>("Java Concurrency")
        );

        rootItem.getChildren().addAll(webItem, javaItem);
    }

    // Método público para agregar nodos desde otras clases
    public void agregarNodo(String padreTexto, String hijoTexto) {
        TreeItem<String> nodoPadre = buscarNodo(rootItem, padreTexto);
        if (nodoPadre != null) {
            nodoPadre.getChildren().add(new TreeItem<>(hijoTexto));
        }
    }

    private TreeItem<String> buscarNodo(TreeItem<String> actual, String texto) {
        if (actual.getValue().equals(texto)) {
            return actual;
        }
        for (TreeItem<String> hijo : actual.getChildren()) {
            TreeItem<String> encontrado = buscarNodo(hijo, texto);
            if (encontrado != null) {
                return encontrado;
            }
        }
        return null;
    }
}
