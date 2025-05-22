package org.example.automatas2_rgg.models;

import javafx.beans.property.SimpleStringProperty;

public class SymbolTableModel {
    private final SimpleStringProperty identificador;
    private final SimpleStringProperty tipo;
    private final SimpleStringProperty valor;
    private final SimpleStringProperty columna;
    private final SimpleStringProperty linea;
    private final SimpleStringProperty categoria;
    private final SimpleStringProperty scope;


    public SymbolTableModel(String identificador, String tipo, String valor, String columna, String linea, String categoria, String scope) {
        this.identificador = new SimpleStringProperty(identificador);
        this.tipo = new SimpleStringProperty(tipo);
        this.valor = new SimpleStringProperty(valor);
        this.columna = new SimpleStringProperty(columna);
        this.linea = new SimpleStringProperty(linea);
        this.categoria = new SimpleStringProperty(categoria);
        this.scope = new SimpleStringProperty(scope);
    }

    public String getIdentificador() { return identificador.get(); }
    public String getTipo() { return tipo.get(); }
    public String getValor() { return valor.get(); }
    public String getColumna() { return columna.get(); }
    public String getLinea() { return linea.get(); }
    public String getCategoria() { return categoria.get(); }
    public String getScope() { return scope.get(); }
}
