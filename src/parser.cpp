#include "parser.hpp"
#include "logger.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

std::vector<std::shared_ptr<Statement>> Parser::parse()
{
  LOG(LogLevel::INFO, "Iniciando el metodo parse");
  std::vector<std::shared_ptr<Statement>> statements;
  LOG(LogLevel::INFO, "Iniciando vector de declaraciones");
  while (!check(TokenType::END_OF_FILE))
  {
    statements.push_back(parseStatement());
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
  tokens[current - 1].print();
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
  if (check(TokenType::NUMBER) || check(TokenType::FLOAT) || check(TokenType::BOOLEAN_LITERAL)) 
  {
    return parseDeclaration();
  } else if (check(TokenType::IDENTIFIER))
  {
    return parseAssigment();
  }

  return nullptr;
}

std::shared_ptr<Statement> Parser::parseDeclaration()
{
  const Token& typeToken = advance();
  std::string type = typeToken.tokenTypeToString(typeToken.type);
  std::string name = typeToken.value;
  match(TokenType::SEMICOLON);
  return std::make_shared<Declaration>(type, name);
}

std::shared_ptr<Statement> Parser::parseAssigment()
{
  std::string name = advance().value;
  LOG(LogLevel::INFO, "[parseAssigment] " + name);
  match(TokenType::ASSIGN);
  LOG(LogLevel::INFO, "[parseAssigment] enter to parseExpression");
  auto expr = parseExpression();
  match(TokenType::SEMICOLON);
  return std::make_shared<Assigment>(name, expr);
}

std::shared_ptr<Expression> Parser::parseExpression()
{  
  LOG(LogLevel::INFO, "[parseExpression] in");
  auto left = parseTerm();
  while (match(TokenType::PLUS) || match(TokenType::MINUS))
  {
    std::string op = tokens[current - 1].value;
    auto right = parseTerm();
    left = std::make_shared<BinaryExpr>(op, left, right);
  }
  LOG(LogLevel::INFO, "[parseExpression] out");
  return left;
}

std::shared_ptr<Expression> Parser::parseTerm()
{
  LOG(LogLevel::INFO, "[parseTerm] in");
  auto left = parseFactor();
  while (match(TokenType::STAR) || match(TokenType::SLASH))
  {
    std::string op = tokens[current - 1].value;
    auto right = parseFactor();
    left = std::make_shared<BinaryExpr>(op, left, right);
  }
  LOG(LogLevel::INFO, "[parseTerm] out");
  return left;
}

std::shared_ptr<Expression> Parser::parseFactor()
{
  if (match(TokenType::NUMBER))
  {
    return std::make_shared<NumberExpr>(tokens[current - 1].value);
  }
  if (match(TokenType::IDENTIFIER)) 
  {
    return std::make_shared<VariableExpr>(tokens[current - 1].value);
  }
  if (match(TokenType::LPAREN)) 
  {
    auto expr = parseExpression();
    match(TokenType::RPAREN);
    return expr;
  }
  return nullptr;
}
