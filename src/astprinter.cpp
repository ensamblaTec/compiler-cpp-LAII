
#include "astprinter.hpp"
#include <iostream>

void printIndent(int indent) {
    for (int i = 0; i < indent; ++i)
        std::cout << "  ";
}

void printInitializerClassic(const std::shared_ptr<Statement>& init, int indent) {
    if (!init) {
        printIndent(indent);
        std::cout << "NULL initializer\n";
        return;
    }

    if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStatement>(init)) {
        printExpressionClassic(exprStmt->expression, indent);
    } else if (auto assignStmt = std::dynamic_pointer_cast<Assignment>(init)) {
        printStatementClassic(assignStmt, indent);
    } else {
        printIndent(indent);
        std::cout << "❓ Inicializador desconocido\n";
    }
}

void printInitializerTree(const std::shared_ptr<Statement>& init, const std::string& indent, bool last) {
    std::cout << indent << (last ? "└── " : "├── ");
    if (!init) {
        std::cout << "NULL initializer\n";
        return;
    }

    if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStatement>(init)) {
        printExpressionTree(exprStmt->expression, indent + (last ? "    " : "│   "), true);
    } else if (auto assignStmt = std::dynamic_pointer_cast<Assignment>(init)) {
        std::cout << "Assignment(name=" << assignStmt->name << ")\n";
        printExpressionTree(assignStmt->expr, indent + (last ? "    " : "│   "), true);
    } else {
        std::cout << "❓ Inicializador desconocido\n";
    }
}

// ===========================
// CLASSIC STYLE PRINTER
// ===========================

void printExpressionClassic(const std::shared_ptr<Expression>& expr, int indent)
{
    if (!expr) {
        printIndent(indent);
        std::cout << "❌ Expresión nula\n";
        return;
    }

    if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr)) {
        printIndent(indent);
        std::cout << "NumberExpr(" << num->value << ")\n";
    }
    else if (auto str = std::dynamic_pointer_cast<StringExpr>(expr)) {
        printIndent(indent);
        std::cout << "StringExpr(\"" << str->text << "\")\n";
    }
    else if (auto boolean = std::dynamic_pointer_cast<BooleanExpr>(expr)) {
        printIndent(indent);
        std::cout << "BooleanExpr(" << (boolean->value ? "true" : "false") << ")\n";
    }
    else if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        printIndent(indent);
        std::cout << "VariableExpr(" << var->name << ")\n";
    }
    else if (auto unary = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
        printIndent(indent);
        std::cout << "UnaryExpr(op='" << unary->op << "')\n";
        printExpressionClassic(unary->right, indent + 1);
    }
    else if (auto binary = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        printIndent(indent);
        std::cout << "BinaryExpr(op=" << binary->op << ")\n";
        printExpressionClassic(binary->left, indent + 1);
        printExpressionClassic(binary->right, indent + 1);
    }
    else if (auto assign = std::dynamic_pointer_cast<AssignmentExpr>(expr)) {
        printIndent(indent);
        std::cout << "AssignmentExpr(name=" << assign->name << ")\n";
        printExpressionClassic(assign->value, indent + 1);
    }
    else {
        printIndent(indent);
        std::cout << "❓ Expresión desconocida\n";
    }
}

void printStatementClassic(const std::shared_ptr<Statement>& stmt, int indent)
{
    if (!stmt) return;

    if (auto decl = std::dynamic_pointer_cast<Declaration>(stmt)) {
        printIndent(indent);
        std::cout << "Declaracion(type=" << decl->type << ", name=" << decl->name << ")\n";
    }
    else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
        printIndent(indent);
        std::cout << "Asignar(name=" << assign->name << ")\n";
        printExpressionClassic(assign->expr, indent + 1);
    }
    else if (auto block = std::dynamic_pointer_cast<BlockStatement>(stmt)) {
        printIndent(indent);
        std::cout << "Bloque\n";
        for (const auto& statement : block->statements)
            printStatementClassic(statement, indent + 1);
    }
    else if (auto ifStmt = std::dynamic_pointer_cast<IfStatement>(stmt)) {
        printIndent(indent);
        std::cout << "Si\n";
        printIndent(indent + 1);
        std::cout << "Condición:\n";
        printExpressionClassic(ifStmt->condition, indent + 2);
        printIndent(indent + 1);
        std::cout << "Entonces:\n";
        printStatementClassic(ifStmt->thenBranch, indent + 2);
        if (ifStmt->elseBranch) {
            printIndent(indent + 1);
            std::cout << "Sino:\n";
            printStatementClassic(ifStmt->elseBranch, indent + 2);
        }
    }
    else if (auto whileStmt = std::dynamic_pointer_cast<WhileStatement>(stmt)) {
        printIndent(indent);
        std::cout << "Mientras\n";
        printIndent(indent + 1);
        std::cout << "Condición:\n";
        printExpressionClassic(whileStmt->condition, indent + 2);
        printIndent(indent + 1);
        std::cout << "Bloque:\n";
        printStatementClassic(whileStmt->body, indent + 2);
    }
    else if (auto forStmt = std::dynamic_pointer_cast<ForStatement>(stmt)) {
        printIndent(indent);
        std::cout << "Para\n";
        if (forStmt->init) printStatementClassic(forStmt->init, indent + 2);
        else {
            printIndent(indent + 2); std::cout << "(sin inicialización)\n";
        }

        if (forStmt->condition) printExpressionClassic(forStmt->condition, indent + 2);
        else {
            printIndent(indent + 2); std::cout << "(sin condición)\n";
        }

        if (forStmt->increment) printStatementClassic(forStmt->increment, indent + 2);
        else {
            printIndent(indent + 2); std::cout << "(sin incremento)\n";
        }

        printStatementClassic(forStmt->body, indent + 2);
    }

    else if (auto printStmt = std::dynamic_pointer_cast<PrintStatement>(stmt)) {
        printIndent(indent);
        std::cout << "Mostrar\n";
        printExpressionClassic(printStmt->expr, indent + 1);
    }
    else if (auto inputStmt = std::dynamic_pointer_cast<InputStatement>(stmt)) {
        printIndent(indent);
        std::cout << "Entrada(variable=" << inputStmt->variable << ")\n";
    }
    else {
        printIndent(indent);
        std::cout << "❓ Sentencia desconocida\n";
    }
}

// ===========================
// TREE STYLE PRINTER
// ===========================

void printStatementTree(const std::shared_ptr<Statement>& stmt, const std::string& indent, bool last) {
    std::cout << indent << (last ? "└── " : "├── ");
    if (!stmt) {
        std::cout << "NULL statement\n";
        return;
    }
    if (auto decl = std::dynamic_pointer_cast<Declaration>(stmt)) {
        std::cout << "Declaracion(type=" << decl->type << ", name=" << decl->name << ")\n";
    }
    else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
        std::cout << "Asignar(name=" << assign->name << ")\n";
        printExpressionTree(assign->expr, indent + (last ? "    " : "│   "), true);
    }
    else if (auto block = std::dynamic_pointer_cast<BlockStatement>(stmt)) {
        std::cout << "Bloque\n";
        for (size_t i = 0; i < block->statements.size(); ++i) {
            printStatementTree(block->statements[i], indent + (last ? "    " : "│   "), i == block->statements.size() - 1);
        }
    }
    else if (auto ifStmt = std::dynamic_pointer_cast<IfStatement>(stmt)) {
        std::cout << "Si\n";
        printExpressionTree(ifStmt->condition, indent + "│   ", false);
        std::cout << indent << "│   ├── Entonces\n";
        printStatementTree(ifStmt->thenBranch, indent + "│   │   ", ifStmt->elseBranch == nullptr);
        if (ifStmt->elseBranch) {
            std::cout << indent << "│   └── Sino\n";
            printStatementTree(ifStmt->elseBranch, indent + "│       ", true);
        }
    }
    else if (auto whileStmt = std::dynamic_pointer_cast<WhileStatement>(stmt)) {
        std::cout << "Mientras\n";
        printExpressionTree(whileStmt->condition, indent + "│   ", false);
        printStatementTree(whileStmt->body, indent + "│   ", true);
    }
    else if (auto forStmt = std::dynamic_pointer_cast<ForStatement>(stmt)) {
        std::cout << indent << (last ? "└── " : "├── ") << "Para\n";

        std::string nextIndent = indent + (last ? "    " : "│   ");

        std::cout << nextIndent << "├── Inicializador\n";
        if (forStmt->init) printStatementTree(forStmt->init, nextIndent + "│   ", false);
        else std::cout << nextIndent << "│   └── (sin inicialización)\n";

        std::cout << nextIndent << "├── Condición\n";
        if (forStmt->condition) printExpressionTree(forStmt->condition, nextIndent + "│   ", false);
        else std::cout << nextIndent << "│   └── (sin condición)\n";

        std::cout << nextIndent << "├── Incremento\n";
        if (forStmt->increment) printStatementTree(forStmt->increment, nextIndent + "│   ", false);
        else std::cout << nextIndent << "│   └── (sin incremento)\n";

        std::cout << nextIndent << "└── Cuerpo\n";
        printStatementTree(forStmt->body, nextIndent + "    ", true);
    }
    else if (auto printStmt = std::dynamic_pointer_cast<PrintStatement>(stmt)) {
        std::cout << "Mostrar\n";
        printExpressionTree(printStmt->expr, indent + "│   ", true);
    }
    else if (auto inputStmt = std::dynamic_pointer_cast<InputStatement>(stmt)) {
        std::cout << "Entrada(variable=" << inputStmt->variable << ")\n";
    }
    else {
        std::cout << "Unknown Statement\n";
    }
}

void printExpressionTree(const std::shared_ptr<Expression>& expr, const std::string& indent, bool last) {
    std::cout << indent << (last ? "└── " : "├── ");
    if (!expr) {
        std::cout << "NULL expression\n";
        return;
    }
    if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr)) {
        std::cout << "Number(" << num->value << ")\n";
    }
    else if (auto str = std::dynamic_pointer_cast<StringExpr>(expr)) {
        std::cout << "String(\"" << str->text << "\")\n";
    }
    else if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        std::cout << "Variable(" << var->name << ")\n";
    }
    else if (auto bin = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        std::cout << "BinaryExpr(op=" << bin->op << ")\n";
        printExpressionTree(bin->left, indent + (last ? "    " : "│   "), false);
        printExpressionTree(bin->right, indent + (last ? "    " : "│   "), true);
    }
    else if (auto b = std::dynamic_pointer_cast<BooleanExpr>(expr)) {
        std::cout << "Boolean(" << (b->value ? "true" : "false") << ")\n";
    }
    else if (auto assign = std::dynamic_pointer_cast<AssignmentExpr>(expr)) {
        std::cout << "AssignmentExpr(name=" << assign->name << ")\n";
        printExpressionTree(assign->value, indent + (last ? "    " : "│   "), true);
    }
    else {
        std::cout << "Unknown Expression\n";
    }
}

