#include "parser.hpp"
#include "logger.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

std::vector<std::shared_ptr<Statement>> Parser::parse()
{
  std::vector<std::shared_ptr<Statement>> statements;
  while (!check(TokenType::END_OF_FILE))
  {
    LOG(LogLevel::DEBUG, "[parser] el token actual a revisar es: " + tokens[current].getPrint());
    auto smtm = parseStatement();
    if (smtm)
    {
      statements.push_back(smtm);
    } else
    {
      LOG(LogLevel::ERROR, "[parse] statement no valido");
    }
  }
  return statements;
}

const Token& Parser::peek() const
{
  return tokens[current];
}

const Token& Parser::advance()
{
  if (!check(TokenType::END_OF_FILE)) current++;
  return tokens[current - 1];
}

bool Parser::check(TokenType type) const
{
  return peek().type == type;
}

bool Parser::match(TokenType type)
{
  if (check(type))
  {
    advance();
    return true;
  }
  return false;
}

std::shared_ptr<Statement> Parser::parseStatement()
{
  if (check(TokenType::KEYWORD_INT) || check(TokenType::KEYWORD_STR) || check(TokenType::KEYWORD_BOOL)) 
  {
    return parseDeclaration();
  } else if (check(TokenType::IDENTIFIER))
  {
    return parseAssignment();
  }

  LOG(LogLevel::ERROR, "[parseStatement] Token inesperado");
  return nullptr;
}

std::shared_ptr<Statement> Parser::parseDeclaration()
{
    std::string type;

    if (match(TokenType::KEYWORD_INT)) type = "entero";
    else if (match(TokenType::KEYWORD_FLOAT)) type = "decimal";
    else if (match(TokenType::KEYWORD_BOOL)) type = "bool";
    else if (match(TokenType::KEYWORD_STR)) type = "texto";
    else {
        LOG(LogLevel::ERROR, "[parseDeclaration] Tipo desconocido en declaración.");
        return nullptr;
    }

    Token nameToken = advance();
    std::string name = nameToken.value;

    if (match(TokenType::ASSIGN)) {
        auto expr = parseExpression();
        if (!expr) {
            LOG(LogLevel::ERROR, "[parseDeclaration] Expresión inválida en declaración con asignación.");
            return nullptr;
        }
        match(TokenType::SEMICOLON);
        return std::make_shared<Assignment>(name, expr);
    }

    match(TokenType::SEMICOLON);
    return std::make_shared<Declaration>(type, name);
}

std::shared_ptr<Statement> Parser::parseAssignment()
{
    std::string name = advance().value;
    if (!match(TokenType::ASSIGN)) {
        LOG(LogLevel::ERROR, "[parseAssigment] Se esperaba '=' en la asignación.");
        return nullptr;
    }

    auto expr = parseExpression();
    if (!expr) {
        LOG(LogLevel::ERROR, "[parseAssigment] Expresión nula en la asignación. Se esperaba un valor despues de '='");
        return nullptr;
    }

    match(TokenType::SEMICOLON);
    return std::make_shared<Assignment>(name, expr);
}

std::shared_ptr<Expression> Parser::parseExpression()
{
    return parseOr();
}

std::shared_ptr<Expression> Parser::parseOr() {
    auto expr = parseAnd();
    while (match(TokenType::OR)) {
        std::string op = previous().value;
        auto right = parseAnd();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::parseAnd() {
    auto expr = parseEquality();
    while (match(TokenType::AND)) {
        std::string op = previous().value;
        auto right = parseEquality();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::parseComparison() {
    auto expr = parseAdditive();
    while (match(TokenType::LT) || match(TokenType::LTE) ||
           match(TokenType::GT) || match(TokenType::GTE)) {
        std::string op = previous().value;
        auto right = parseTerm();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::EQ) || match(TokenType::NEQ)) {
        std::string op = previous().value;
        auto right = parseComparison();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::parseAdditive() {
    auto expr = parseTerm();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        std::string op = previous().value;
        auto right = parseTerm();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::parseTerm() {
  auto left = parseUnary();
  while (match(TokenType::MUL) || match(TokenType::SLASH)) {
    std::string op = previous().value;
    auto right = parseUnary();
    left = std::make_shared<BinaryExpr>(op, left, right);
  }
  return left;
}

std::shared_ptr<Expression> Parser::parsePrimary() {
  if (match(TokenType::NUMBER)) {
    return std::make_shared<NumberExpr>(previous().value);
  }
  if (match(TokenType::STRING_LITERAL)) {
    return std::make_shared<StringExpr>(previous().value);
  }
  if (match(TokenType::BOOLEAN_LITERAL)) {
    return std::make_shared<BooleanExpr>(previous().value);
  }
  if (match(TokenType::IDENTIFIER)) {
    return std::make_shared<VariableExpr>(previous().value);
  }
  if (match(TokenType::LPAREN)) {
    auto expr = parseExpression();
    match(TokenType::RPAREN);
    return expr;
  }

  LOG(LogLevel::ERROR, "[parsePrimary] expresión desconocida: " + peek().value);
  return nullptr;
}

std::shared_ptr<Expression> Parser::parseUnary() {
    if (match(TokenType::NOT) || match(TokenType::MINUS)) {
        std::string op = previous().value;
        auto right = parseUnary();
        return std::make_shared<UnaryExpr>(op, right);
    }
    return parsePrimary();
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

