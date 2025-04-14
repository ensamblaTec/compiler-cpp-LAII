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
    bool expect(TokenType type, const std::string& scopeMethod, const std::string& msgError);

    std::shared_ptr<Statement> parseStatement();
    std::shared_ptr<Statement> parseDeclaration();
    std::shared_ptr<Statement> parseAssignment();
    std::shared_ptr<Expression> parseExpression();
    std::shared_ptr<Expression> parseOr();
    std::shared_ptr<Expression> parseAnd();
    std::shared_ptr<Expression> parseEquality();
    std::shared_ptr<Expression> parseComparison();
    std::shared_ptr<Expression> parseAdditive();
    std::shared_ptr<Expression> parseTerm();
    std::shared_ptr<Expression> parseUnary();
    std::shared_ptr<Expression> parsePrimary();

    const Token& previous() const;
};
