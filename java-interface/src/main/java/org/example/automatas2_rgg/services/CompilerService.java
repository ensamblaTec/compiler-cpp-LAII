package org.example.automatas2_rgg.services;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.stream.Collectors;

public class CompilerService {

    private final Path compiladorPath;

    public CompilerService(Path basePath) {
        this.compiladorPath = basePath;
    }

    public String compilarCodigo(String codigoFuente, File archivoGuardado) throws IOException {
        if (archivoGuardado == null) {
            throw new IllegalArgumentException("No se ha especificado ningún archivo fuente para guardar.");
        }

        Files.writeString(archivoGuardado.toPath(), codigoFuente, StandardCharsets.UTF_8);

        boolean isWindows = System.getProperty("os.name").toLowerCase().contains("win");
        String ejecutable = isWindows ? "app.exe" : "app";

        Path compilador = compiladorPath.resolve("build/dist").resolve(ejecutable).normalize();

        if (!Files.exists(compilador) || (!isWindows && !Files.isExecutable(compilador))) {
            throw new IOException("No se encontró o no tiene permisos de ejecución el compilador en: " + compilador);
        }

        ProcessBuilder pb = new ProcessBuilder(
                compilador.toAbsolutePath().toString(),
                archivoGuardado.getAbsolutePath()
        );

        pb.directory(compilador.getParent().toFile());
        pb.redirectErrorStream(true);

        Process proceso = pb.start();

        try (BufferedReader reader = new BufferedReader(new InputStreamReader(proceso.getInputStream()))) {
            return reader.lines().collect(Collectors.joining("\n"));
        }
    }

}
