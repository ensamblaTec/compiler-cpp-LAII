#include "lexer.hpp"
#include "parser.hpp"
#include "error_reporter.hpp"
#include "symbol_table.hpp"
#include "astprinter.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    std::ifstream inputFile("codigo.txt"); // tu archivo de entrada
    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return 1;
    }

    Lexer lexer(inputFile);
    ErrorReporter reporter;
    SymbolTable symbols;

    Parser parser(lexer, reporter, symbols);

    auto statements = parser.parse();

    if (reporter.hasErrors()) {
        std::cout << "Errores encontrados durante el parseo." << std::endl;
    } else {
        std::cout << "Parseo exitoso!" << std::endl;
        // AstPrinter printer;
        // printer.print(statements);
    }

    return 0;
}

