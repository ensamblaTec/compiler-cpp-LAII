#pragma once

#include "ast.hpp"
#include <memory>
#include <vector>

class AstPrinter {
public:
    void print(const std::vector<std::shared_ptr<Statement>>& statements);

private:
    int indentLevel = 0;

    void printIndent() const;
    void printStatement(const std::shared_ptr<Statement>& stmt);
    void printExpression(const std::shared_ptr<Expression>& expr);

    void printVarDecl(const std::shared_ptr<VarDeclStatement>& stmt);
    void printPrint(const std::shared_ptr<PrintStatement>& stmt);
};

