#pragma once

#include "ast.hpp"

#include <iostream>
#include <memory>

void printExpression(const std::shared_ptr<Expression>& expr, int indent = 0);
void printStatement(const std::shared_ptr<Statement>& stmt, int indent = 0);

inline void printIndent(int indent) {
    for (int i = 0; i < indent; ++i)
        std::cout << "  ";
}

inline void printExpression(const std::shared_ptr<Expression>& expr, int indent) {
    if (!expr) return;

    if (auto n = std::dynamic_pointer_cast<NumberExpr>(expr)) {
        printIndent(indent);
        std::cout << "NumberExpr(" << n->value << ")\n";
    }
    else if (auto v = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        printIndent(indent);
        std::cout << "VariableExpr(" << v->name << ")\n";
    }
    else if (auto b = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        printIndent(indent);
        std::cout << "BinaryExpr(" << b->op << ")\n";
        printExpression(b->left, indent + 1);
        printExpression(b->right, indent + 1);
    }
    else {
        printIndent(indent);
        std::cout << "Unknown Expression\n";
    }
}

inline void printStatement(const std::shared_ptr<Statement>& stmt, int indent) {
    if (!stmt) return;

    if (auto d = std::dynamic_pointer_cast<Declaration>(stmt)) {
        printIndent(indent);
        std::cout << "Declaracion(type=" << d->type << ", name=" << d->name << ")\n";
    }
    else if (auto a = std::dynamic_pointer_cast<Assigment>(stmt)) {
        printIndent(indent);
        std::cout << "Declaracion(name=" << a->name << ")\n";
        printExpression(a->expr, indent + 1);
    }
    else {
        printIndent(indent);
        std::cout << "Unknown Statement\n";
    }
}

