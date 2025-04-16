#include "error_reporter.hpp"
#include <iostream>

ErrorReporter& ErrorReporter::getInstance() {
    static ErrorReporter instance;
    return instance;
}

void ErrorReporter::report(const std::string& message, int line, int column) {
    errors.push_back({message, line, column});
}

bool ErrorReporter::hasErrors() const {
    return !errors.empty();
}

void ErrorReporter::clear() {
    errors.clear();
}

void ErrorReporter::printSummary() const {
    if (errors.empty()) return;

    std::cout << "\n[RESUMEN] Se encontraron " << errors.size() << " errores gramaticales:\n";
    for (const auto& err : errors) {
        std::cout << "  - Línea " << (err.line + 1) << ", columna " << err.column << ": " << err.message << "\n";
    }
}

