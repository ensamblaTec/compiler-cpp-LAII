#include "parser.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "error_reporter.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

std::vector<std::shared_ptr<Statement>> Parser::parse()
{
  std::vector<std::shared_ptr<Statement>> statements;
  while (!check(TokenType::END_OF_FILE))
  {
    LOG(LogLevel::DEBUG, "[parser] el token actual a revisar es: " + tokens[current].getPrint());
    if (check(TokenType::RBRACE)) {
      LOG(LogLevel::ERROR, "[parse] Se encontró una llave de cierre '}' fuera de un bloque");
      advance();
      continue;
    }
    auto smtm = parseStatement();
    if (smtm)
    {
      statements.push_back(smtm);
    } else
    {
      LOG(LogLevel::ERROR, "[parse] statement no valido");
      synchronize();
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

bool Parser::expect(TokenType type, const std::string& scopeMethod, const std::string& msgError)
{
  if (match(type)) return true;
  
  const Token& token = peek();
  LOG(LogLevel::ERROR, "[" + scopeMethod + "] " + msgError +
      " se ha encontrado el token " + token.getPrint() +
      " (linea: " + std::to_string(token.row + 1) + ", columna: " + std::to_string(token.column) + ")");
  return false;
}

bool Parser::isBinaryOperator(TokenType type) const {
    return type == TokenType::PLUS || type == TokenType::MINUS ||
           type == TokenType::MUL  || type == TokenType::DIV   || type == TokenType::MOD ||
           type == TokenType::EQ   || type == TokenType::NEQ   ||
           type == TokenType::LT   || type == TokenType::LTE   ||
           type == TokenType::GT   || type == TokenType::GTE   ||
           type == TokenType::AND  || type == TokenType::OR;
}

void Parser::synchronize() {
    while (!check(TokenType::END_OF_FILE)) {
        if (previous().type == TokenType::SEMICOLON) return;

        TokenType t = peek().type;

        if (t == TokenType::KEYWORD_INT || t == TokenType::KEYWORD_BOOL ||
            t == TokenType::KEYWORD_STR || t == TokenType::KEYWORD_FLOAT ||
            t == TokenType::IDENTIFIER)
        {
            return;
        }

        advance();
    }
}

std::shared_ptr<Statement> Parser::parseStatement()
{
    if (check(TokenType::RBRACE)) {
        return nullptr;
    }

    if (match(TokenType::KEYWORD_INT) || match(TokenType::KEYWORD_STR) || match(TokenType::KEYWORD_BOOL)) 
        return parseDeclaration();

    if (check(TokenType::IDENTIFIER))
        return parseAssignment();

    if (match(TokenType::KEYWORD_IF))
        return parseIf();

    if (match(TokenType::KEYWORD_WHILE))
        return parseWhile();

    if (match(TokenType::KEYWORD_FOR))
        return parseFor();

    if (match(TokenType::KEYWORD_PRINT))
        return parsePrint();

    if (match(TokenType::KEYWORD_INPUT))
        return parseInput();

    LOG(LogLevel::ERROR, "[parseStatement] Token inesperado: " + peek().getPrint() +
                         " (línea: " + std::to_string(peek().row + 1) +
                         ", columna: " + std::to_string(peek().column) + ")");
    return nullptr;
}

std::shared_ptr<Statement> Parser::parseDeclaration()
{
    std::string type;

    if (match(TokenType::KEYWORD_INT))       type = "entero";
    else if (match(TokenType::KEYWORD_FLOAT)) type = "decimal";
    else if (match(TokenType::KEYWORD_BOOL))  type = "bool";
    else if (match(TokenType::KEYWORD_STR))   type = "texto";
    else {
        LOG(LogLevel::ERROR, "[parseDeclaration] Tipo desconocido en declaración. Se encontró: " + peek().getPrint());
        return nullptr;
    }

    if (!match(TokenType::IDENTIFIER)) {
      TokenType found = peek().type;

      if (found == TokenType::ASSIGN)
      {
        LOG(LogLevel::ERROR, "[parseDeclaration] Se encontró un '=' sin identificador. Se esperaba un nombre de variable antes del operador de asignación.");
      } else if (check(TokenType::SEMICOLON)) {
        LOG(LogLevel::ERROR, "[parseDeclaration] Declaración incompleta: se esperaba un identificador antes del ';'");
      } else {
        LOG(LogLevel::ERROR, "[parseDeclaration] Se esperaba un identificador después del tipo. Se encontró: " + peek().getPrint());
      }
      return nullptr;
    }

    std::string name = previous().value;

    if (check(TokenType::KEYWORD_INT) || check(TokenType::KEYWORD_FLOAT) ||
      check(TokenType::KEYWORD_BOOL) || check(TokenType::KEYWORD_STR))
    {
      LOG(LogLevel::ERROR, "[parseDeclaration] Se encontró otro tipo después del identificador '" + name +
          "'. ¿Quizás olvidaste un punto y coma ';' entre declaraciones?");
      return nullptr;
    }

    if (check(TokenType::IDENTIFIER)) {
      LOG(LogLevel::ERROR, "[parseDeclaration] Se encontró un identificador inesperado después del nombre '" + name + "'. Quizás olvidaste un punto y coma o estás redeclarando sin separar sentencias.");
      return nullptr;
    }

    if (match(TokenType::ASSIGN)) {
      if (check(TokenType::SEMICOLON) || check(TokenType::RPAREN) || check(TokenType::END_OF_FILE)) {
        LOG(LogLevel::ERROR, "[parseDeclaration] Se esperaba una expresión después de '=' pero se encontró: " + peek().getPrint());
        return nullptr;
      }

      auto expr = parseExpression();

      if (!expr) {
        LOG(LogLevel::ERROR, "[parseDeclaration] Expresión inválida en declaración con asignación");
        return nullptr;
      }

      if (!expect(TokenType::SEMICOLON, "parseDeclaration", "Se esperaba ';' después de la asignación en la declaración"))
        return nullptr;

      return std::make_shared<Assignment>(name, expr);
    }

    if (!expect(TokenType::SEMICOLON, "parseDeclaration", "Se esperaba ';' después de la declaración"))
        return nullptr;

    return std::make_shared<Declaration>(type, name);
}

std::shared_ptr<Statement> Parser::parseIf() {
    if (!match(TokenType::LPAREN)) {
        LOG(LogLevel::ERROR, "[parseIf] Se esperaba '(' después de 'si'");
        return nullptr;
    }

    auto condition = parseExpression();

    if (!expect(TokenType::RPAREN, "parseIf", "Se esperaba ')' después de la condición"))
        return nullptr;

    auto thenBranch = parseBlock();

    std::shared_ptr<Statement> elseBranch = nullptr;
    if (match(TokenType::KEYWORD_ELSE)) {
        elseBranch = parseBlock(); 
    }

    return std::make_shared<IfStatement>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Statement> Parser::parseWhile() {
    if (!match(TokenType::LPAREN)) {
        LOG(LogLevel::ERROR, "[parseWhile] Se esperaba '(' después de 'mientras'");
        return nullptr;
    }

    auto condition = parseExpression();

    if (!expect(TokenType::RPAREN, "parseWhile", "Se esperaba ')' después de la condición"))
        return nullptr;

    auto body = parseBlock();

    return std::make_shared<WhileStatement>(condition, body);
}

std::shared_ptr<Statement> Parser::parseFor() {
    if (!match(TokenType::LPAREN)) {
        LOG(LogLevel::ERROR, "[parseFor] Se esperaba '(' después de 'para'");
        return nullptr;
    }

    auto initializer = parseExpression();
    if (!expect(TokenType::SEMICOLON, "parseFor", "Se esperaba ';' después de la inicialización"))
        return nullptr;

    auto condition = parseExpression();

    if (!expect(TokenType::SEMICOLON, "parseFor", "Se esperaba ';' después de la condición"))
        return nullptr;

    auto increment = parseExpression();

    if (!expect(TokenType::RPAREN, "parseFor", "Se esperaba ')' después de los parámetros"))
        return nullptr;

    auto body = parseBlock();

    return std::make_shared<ForStatement>(initializer, condition, increment, body);
}

std::shared_ptr<Statement> Parser::parsePrint() {
    auto expr = parseExpression();

    if (!expr) {
        LOG(LogLevel::ERROR, "[parsePrint] Expresión inválida después de 'mostrar'");
        return nullptr;
    }

    match(TokenType::SEMICOLON);
    return std::make_shared<PrintStatement>(expr);
}

std::shared_ptr<Statement> Parser::parseInput() {
    Token varToken = advance();

    if (!match(TokenType::SEMICOLON)) {
        LOG(LogLevel::ERROR, "[parseEntrada] Se esperaba ';' después de 'entrada'");
        return nullptr;
    }

    return std::make_shared<InputStatement>(varToken.value);
}

std::shared_ptr<Statement> Parser::parseBlock() {
    if (!match(TokenType::LBRACE)) {
        LOG(LogLevel::ERROR, "[parseBlock] Se esperaba '{' para iniciar un bloque");
        return nullptr;
    }

    std::vector<std::shared_ptr<Statement>> statements;

    while (!check(TokenType::RBRACE) && !check(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        } else {
            synchronize();
        }
    }

    if (!match(TokenType::RBRACE)) {
        LOG(LogLevel::ERROR, "[parseBlock] Se esperaba '}' para cerrar el bloque");
        return nullptr;
    }

    return std::make_shared<BlockStatement>(statements);
}

std::shared_ptr<Statement> Parser::parseAssignment()
{
    if (!match(TokenType::IDENTIFIER)) {
        LOG(LogLevel::ERROR, "[parseAssignment] Se esperaba un identificador al inicio de la asignación. Se encontró: " + peek().getPrint());
        return nullptr;
    }

    std::string name = previous().value;

    if (!expect(TokenType::ASSIGN, "parseAssignment", "Se esperaba '=' en la asignación"))
        return nullptr;

    if (check(TokenType::SEMICOLON) || check(TokenType::RPAREN) || check(TokenType::END_OF_FILE)) {
        LOG(LogLevel::ERROR, "[parseAssignment] Se esperaba una expresión después de '=' pero se encontró: " + peek().getPrint());
        return nullptr;
    }

    auto expr = parseExpression();

    if (!expr) {
        LOG(LogLevel::ERROR, "[parseAssignment] Expresión nula en la asignación. Se esperaba un valor después de '='");
        return nullptr;
    }

    if (!expect(TokenType::SEMICOLON, "parseAssignment", "Se esperaba ';' después de la asignación"))
        return nullptr;

    return std::make_shared<Assignment>(name, expr);
}

std::shared_ptr<Expression> Parser::parseExpression() {
    if (check(TokenType::PLUS) || check(TokenType::MINUS) ||
        check(TokenType::STAR) || check(TokenType::SLASH) ||
        check(TokenType::EQ) || check(TokenType::NEQ) ||
        check(TokenType::LT) || check(TokenType::LTE) ||
        check(TokenType::GT) || check(TokenType::GTE) ||
        check(TokenType::AND) || check(TokenType::OR) ||
        check(TokenType::MOD)) 
    {
        LOG(LogLevel::ERROR, "[parseExpression] Se encontró un operador binario sin operando izquierdo: " + peek().getPrint());
        return nullptr;
    }

    return parseOr();
}

std::shared_ptr<Expression> Parser::parseOr() {
    auto expr = parseAnd();
    if (!expr) {
      LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'.");
      return nullptr;
    }
    while (match(TokenType::OR)) {
        std::string op = previous().value;
        auto right = parseAnd();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::parseAnd() {
    auto expr = parseEquality();
    if (!expr) {
      LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'.");
      return nullptr;
    }
    while (match(TokenType::AND)) {
        std::string op = previous().value;
        auto right = parseEquality();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::parseComparison() {
  auto expr = parseAdditive();
  if (!expr) {
    LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'.");
    return nullptr;
  }
  while (match(TokenType::LT) || match(TokenType::LTE) ||
    match(TokenType::GT) || match(TokenType::GTE)) {
    std::string op = previous().value;
    auto right = parseTerm();
    
    if (!right) {
      LOG(LogLevel::ERROR, "[parseComparison] Operando derecho inválido para el operador: '" + op + "'");
      return nullptr;
    }

    expr = std::make_shared<BinaryExpr>(op, expr, right);
  }
  return expr;
}

std::shared_ptr<Expression> Parser::parseEquality() {
  auto expr = parseComparison();
  if (!expr) {
    LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'.");
    return nullptr;
  }
  while (match(TokenType::EQ) || match(TokenType::NEQ)) {
    std::string op = previous().value;
    auto right = parseComparison();

    if (!right) {
      LOG(LogLevel::ERROR, "[parseEquality] Operando derecho inválido para el operador: '" + op + "'");
      return nullptr;

    }
    expr = std::make_shared<BinaryExpr>(op, expr, right);
  }
  return expr;
}

std::shared_ptr<Expression> Parser::parseAdditive() {
    auto expr = parseTerm();

    if (!expr) {
        LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'.");
        return nullptr;
    }

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        std::string op = previous().value;
        if (isBinaryOperator(peek().type))
        {
          LOG(LogLevel::ERROR, "[parseAdditive] Operadores binarios consecutivos inválidos: '" + op + "' seguido de '" + peek().value + "'");
          return nullptr;
        }

        auto right = parseTerm();
        if (!right)
        {
          LOG(LogLevel::ERROR, "[parseAdditive] Operando derecho inválido para el operador: '" + op + "'");
          return nullptr;
        }

        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::parseTerm() {
  auto expr = parseUnary();

  if (!expr) {
    LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'.");
    return nullptr;
  }

  while (match(TokenType::MUL) || match(TokenType::SLASH)) {
    std::string op = previous().value;
    if (isBinaryOperator(peek().type))
    {
      LOG(LogLevel::ERROR, "[parseAdditive] Operadores binarios consecutivos inválidos: '" + op + "' seguido de '" + peek().value + "'");
      return nullptr;
    }

    auto right = parseUnary();
    if (!right) {
      LOG(LogLevel::ERROR, "[parseTerm] Operando derecho inválido para el operador: '" + op + "'");
      return nullptr;
    }

    expr = std::make_shared<BinaryExpr>(op, expr, right);
  }
  return expr;
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
    if (check(TokenType::RPAREN)) {
        LOG(LogLevel::ERROR, "[parsePrimary] Se encontró un paréntesis vacío '()'. Se esperaba una expresión dentro.");
        return nullptr;
    }

    auto expr = parseExpression();
    if (!expr) {
        LOG(LogLevel::ERROR, "[parsePrimary] Se esperaba una expresión válida dentro del paréntesis.");
        return nullptr;
    }

    if (!expect(TokenType::RPAREN, "parsePrimary", "Se esperaba ')' para cerrar la expresión"))
        return nullptr;

    return expr;
  }

  LOG(LogLevel::ERROR, "[parsePrimary] expresión desconocida: " + peek().value);
  return nullptr;
}

std::shared_ptr<Expression> Parser::parseUnary() {
    if (match(TokenType::NOT) || match(TokenType::MINUS)) {
        std::string op = previous().value;
        auto right = parseUnary();

        if (!right) {
          LOG(LogLevel::ERROR, "[parseTerm] Operando derecho inválido para el operador: '" + op + "'");
          return nullptr;
        }

        return std::make_shared<UnaryExpr>(op, right);
    }
    return parsePrimary();
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

