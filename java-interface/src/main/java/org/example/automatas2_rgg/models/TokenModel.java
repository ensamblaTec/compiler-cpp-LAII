package org.example.automatas2_rgg.models;

import javafx.beans.property.SimpleStringProperty;

public class TokenModel {
    private final SimpleStringProperty tipo;
    private final SimpleStringProperty valor;
    private final SimpleStringProperty columna;
    private final SimpleStringProperty linea;

    public TokenModel(String tipo, String valor, String columna, String linea) {
        this.tipo = new SimpleStringProperty(tipo);
        this.valor = new SimpleStringProperty(valor);
        this.columna = new SimpleStringProperty(columna);
        this.linea = new SimpleStringProperty(linea);
    }

    public String getTipo() { return tipo.get(); }
    public String getValor() { return valor.get(); }
    public String getColumna() { return columna.get(); }
    public String getLinea() { return linea.get(); }

    public void setTipo(String tipo) { this.tipo.set(tipo); }
    public void setValor(String valor) { this.valor.set(valor); }
    public void setColumna(String columna) { this.columna.set(columna); }
    public void setLinea(String linea) { this.linea.set(linea); }


}