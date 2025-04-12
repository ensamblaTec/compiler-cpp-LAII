#pragma once

#include "token.hpp"
#include "ast.hpp"

#include <vector>
#include <memory>

class Parser
{
  public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::shared_ptr<Statement>> parse();

  private:
    const std::vector<Token>& tokens;
    int current;

    const Token& peek() const;
    const Token& advance();
    bool match(TokenType type);
    bool check(TokenType type) const;

    std::shared_ptr<Statement> parseStatement();
    std::shared_ptr<Statement> parseDeclaration();
    std::shared_ptr<Statement> parseAssigment();
    std::shared_ptr<Expression> parseExpression();
    std::shared_ptr<Expression> parseTerm();
    std::shared_ptr<Expression> parseFactor();
};
