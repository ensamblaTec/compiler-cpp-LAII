#include "ast_printer.hpp"
#include <iostream>
#include <memory>

void AstPrinter::print(const std::vector<std::shared_ptr<Statement>>& statements) {
    for (const auto& stmt : statements) {
        printStatement(stmt);
    }
}

void AstPrinter::printIndent() const {
    for (int i = 0; i < indentLevel; ++i) {
        std::cout << "  ";
    }
}

void AstPrinter::printStatement(const std::shared_ptr<Statement>& stmt) {
    if (auto varDecl = std::dynamic_pointer_cast<VarDeclStatement>(stmt)) {
        printVarDecl(varDecl);
    } else if (auto printStmt = std::dynamic_pointer_cast<PrintStatement>(stmt)) {
        printPrint(printStmt);
    } else {
        printIndent();
        std::cout << "[Nodo de tipo desconocido]" << std::endl;
    }
}

void AstPrinter::printVarDecl(const std::shared_ptr<VarDeclStatement>& stmt) {
    printIndent();
    std::cout << "VarDecl (" 
              << (stmt->type == VarType::INT ? "int" :
                  stmt->type == VarType::STR ? "str" :
                  stmt->type == VarType::BOOL ? "bool" : "unknown")
              << ") " << stmt->name;

    if (stmt->initializer) {
        std::cout << " = ";
        printExpression(stmt->initializer);
    }

    std::cout << std::endl;
}

void AstPrinter::printPrint(const std::shared_ptr<PrintStatement>& stmt) {
    printIndent();
    std::cout << "Print ";
    if (stmt->expression) {
        printExpression(stmt->expression);
    }
    std::cout << std::endl;
}

void AstPrinter::printExpression(const std::shared_ptr<Expression>& expr) {
    if (!expr) {
        std::cout << "(null expr)";
        return;
    }

    if (auto literal = std::dynamic_pointer_cast<LiteralExpression>(expr)) {
        std::cout << literal->value;
    } else if (auto variable = std::dynamic_pointer_cast<VariableExpression>(expr)) {
        std::cout << variable->name;
    } else if (auto binary = std::dynamic_pointer_cast<BinaryExpression>(expr)) {
        std::cout << "(";
        printExpression(binary->left);
        std::cout << " " << binary->op.lexeme << " ";
        printExpression(binary->right);
        std::cout << ")";
    } else {
        std::cout << "(expr desconocida)";
    }
}
