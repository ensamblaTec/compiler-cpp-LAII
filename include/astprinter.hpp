#pragma once

#include <memory>
#include "ast.hpp"

void printIndent(int indent);

void printStatementClassic(const std::shared_ptr<Statement>& stmt, int indent = 0);
void printExpressionClassic(const std::shared_ptr<Expression>& expr, int indent = 0);

void printStatementTree(const std::shared_ptr<Statement>& stmt, const std::string& indent = "", bool last = true);
void printExpressionTree(const std::shared_ptr<Expression>& expr, const std::string& indent = "", bool last = true);

