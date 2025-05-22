package org.example.automatas2_rgg.controllers;

import java.awt.Desktop;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.MenuItem;

public class PDFController {
    @FXML
    private MenuItem sintacticoMenuItem;

    @FXML
    void abrirPDF(ActionEvent event) {
        try {
            URI uri = new URI("https://drive.google.com/file/d/1FoJsU1V1as1-ecBe0AxbOcn-mEu9sGaC/view?usp=sharing");
            Desktop.getDesktop().browse(uri);
        } catch (IOException | URISyntaxException e) {
            e.printStackTrace();
        }
    }

    @FXML
    void abrirPDFLexico(ActionEvent event) {
        try {
            URI uri = new URI("https://drive.google.com/file/d/1JG6MK4POZT3C9NWHFw3opaL8q66CYfbk/view?usp=sharing");
            Desktop.getDesktop().browse(uri);
        } catch (IOException | URISyntaxException e) {
            e.printStackTrace();
        }
    }

    @FXML
    void abrirPDFSemantico(ActionEvent event) {
        try {
            URI uri = new URI("https://drive.google.com/file/d/1_6XnQnb1FkawCrku1zUl8wJpn7Ci596W/view?usp=sharing");
            Desktop.getDesktop().browse(uri);
        } catch (IOException | URISyntaxException e) {
            e.printStackTrace();
        }
    }

}