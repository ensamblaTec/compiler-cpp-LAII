package org.example.automatas2_rgg.models;

import javafx.beans.property.SimpleStringProperty;

public class SymbolTableModel {
    private final SimpleStringProperty identificador;
    private final SimpleStringProperty tipo;
    private final SimpleStringProperty valor;
    private final SimpleStringProperty columna;
    private final SimpleStringProperty linea;


    public SymbolTableModel(String identificador, String tipo, String valor, String columna, String linea) {
        this.identificador = new SimpleStringProperty(identificador);
        this.tipo = new SimpleStringProperty(tipo);
        this.valor = new SimpleStringProperty(valor);
        this.columna = new SimpleStringProperty(columna);
        this.linea = new SimpleStringProperty(linea);
    }

    public String getIdentificador() { return identificador.get(); }
    public String getTipo() { return tipo.get(); }
    public String getValor() { return valor.get(); }
    public String getColumna() { return columna.get(); }
    public String getLinea() { return linea.get(); }
}
