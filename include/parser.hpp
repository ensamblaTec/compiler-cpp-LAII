#pragma once

#include "token.hpp"
#include "ast.hpp"
#include "symbol_table.hpp"

#include <vector>
#include <memory>

class Parser
{
  public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::shared_ptr<Statement>> parse();
    SymbolTable symbols;

  private:

    bool semanticErrorOcurred = false;

    const std::vector<Token>& tokens;
    int current;

    const Token& peek() const;
    Token peekNext() const;
    const Token& advance();

    bool match(TokenType type);
    bool check(TokenType type) const;
    bool expect(TokenType type, const std::string& scopeMethod, const std::string& msgError);
    bool isBinaryOperator(TokenType type) const;
    bool isAtEnd() const;

    void synchronize();
    void logTokenContext(const std::string& header) const;

    std::string inferType(const std::shared_ptr<Expression>& expr);

    std::shared_ptr<Statement> parseStatement();
    std::shared_ptr<Statement> parseDeclaration();
    std::shared_ptr<Statement> parseIf();
    std::shared_ptr<Statement> parseWhile();

    std::shared_ptr<Statement> parseForInitializer();
    std::shared_ptr<Statement> parseFor();
    std::shared_ptr<Statement> parseIncrement();

    std::shared_ptr<Statement> parsePrint();
    std::shared_ptr<Statement> parseInput();
    std::shared_ptr<Statement> parseAssignment(bool consumeSemicolon = true);
    std::shared_ptr<Statement> parseBlock(const std::string& context);

    std::shared_ptr<Expression> parseExpression();
    std::shared_ptr<Expression> parseAssignmentExpression();

    std::shared_ptr<Expression> parseOr();
    std::shared_ptr<Expression> parseAnd();
    std::shared_ptr<Expression> parseEquality();
    std::shared_ptr<Expression> parseComparison();
    std::shared_ptr<Expression> parseAdditive();
    std::shared_ptr<Expression> parseTerm();
    std::shared_ptr<Expression> parseUnary();
    std::shared_ptr<Expression> parsePrimary();

    const Token& previous() const;

    void validateVarDeclared(const std::shared_ptr<Expression>& expr);
};
