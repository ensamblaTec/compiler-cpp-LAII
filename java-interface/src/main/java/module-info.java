module org.example.automatas2_rgg {
    requires javafx.controls;
    requires javafx.fxml;

    requires org.controlsfx.controls;
    requires com.dlsc.formsfx;
    requires org.kordamp.bootstrapfx.core;
    requires org.fxmisc.richtext;
    requires com.fasterxml.jackson.databind;

    opens org.example.automatas2_rgg to javafx.fxml;
    exports org.example.automatas2_rgg;
    exports org.example.automatas2_rgg.controllers;
    opens org.example.automatas2_rgg.controllers to javafx.fxml;
}