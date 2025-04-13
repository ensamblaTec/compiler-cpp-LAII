#include "astprinter.hpp"

#include <iostream>

void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  "; // 2 espacios por nivel
    }
}

void printExpressionClassic(const std::shared_ptr<Expression>& expr, int indent)
{
    if (!expr) {
        printIndent(indent);
        std::cout << "❌ Expresión nula\n";
        return;
    }

    if (auto n = std::dynamic_pointer_cast<NumberExpr>(expr)) {
        printIndent(indent);
        std::cout << "NumberExpr(" << n->value << ")\n";
    }
    else if (auto s = std::dynamic_pointer_cast<StringExpr>(expr)) {
        printIndent(indent);
        std::cout << "StringExpr(\"" << s->text << "\")\n";
    }
    else if (auto v = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        printIndent(indent);
        std::cout << "VariableExpr(" << v->name << ")\n";
    }
    else if (auto b = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        printIndent(indent);
        std::cout << "BinaryExpr(op=" << b->op << ")\n";
        printExpressionClassic(b->left, indent + 1);
        printExpressionClassic(b->right, indent + 1);
    }
    else if (auto b = std::dynamic_pointer_cast<BooleanExpr>(expr)) {
      printIndent(indent);
      std::cout << "BooleanExpr(" << (b->value ? "true" : "false") << ")\n";
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
}

void printStatementTree(const std::shared_ptr<Statement>& stmt, const std::string& indent, bool last)
{
    std::cout << indent << (last ? "└── " : "├── ");

    if (!stmt) {
        std::cout << "NULL statement\n";
        return;
    }

    if (auto decl = std::dynamic_pointer_cast<Declaration>(stmt)) {
        std::cout << "Declaration(type=" << decl->type << ", name=" << decl->name << ")\n";
    }
    else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
        std::cout << "Assignment(name=" << assign->name << ")\n";
        printExpressionTree(assign->expr, indent + (last ? "    " : "│   "), true);
    }
    else {
        std::cout << "Unknown Statement\n";
    }
}

void printExpressionTree(const std::shared_ptr<Expression>& expr, const std::string& indent, bool last)
{
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
    else {
        std::cout << "Unknown Expression\n";
    }
}

