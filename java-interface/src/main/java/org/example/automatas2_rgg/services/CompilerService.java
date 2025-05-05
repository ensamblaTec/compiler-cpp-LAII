package org.example.automatas2_rgg.services;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.stream.Collectors;

public class CompilerService {

    private final Path compiladorPath;

    public CompilerService(Path basePath) {
        this.compiladorPath = basePath.resolve("app");
    }

    public String compilarCodigo(String codigoFuente, File archivoGuardado) throws IOException {
        if (archivoGuardado == null) {
            throw new IllegalArgumentException("No se ha especificado ningún archivo fuente para guardar.");
        }

        Files.writeString(archivoGuardado.toPath(), codigoFuente, StandardCharsets.UTF_8);

        if (!Files.isExecutable(compiladorPath)) {
            throw new IOException("No se encontró o no tiene permisos de ejecución el compilador en: " + compiladorPath);
        }

        ProcessBuilder pb = new ProcessBuilder(
                compiladorPath.toAbsolutePath().toString(),
                archivoGuardado.getAbsolutePath()
        );

        pb.directory(compiladorPath.getParent().toFile());
        pb.redirectErrorStream(true);

        Process proceso = pb.start();

        return new BufferedReader(new InputStreamReader(proceso.getInputStream()))
                .lines().collect(Collectors.joining("\n"));
    }
}
