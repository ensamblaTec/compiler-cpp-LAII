#include "parser.hpp"
#include "logger.hpp"
#include "utils.hpp"

void Parser::logTokenContext() const {
  LOG(LogLevel::DEBUG, "[Contexto] Token anterior: " + previousToken.getPrint());
  LOG(LogLevel::DEBUG, "[Contexto] Token actual:   " + currentToken.getPrint());
}

Parser::Parser(Lexer& lexer, ErrorReporter& reporter, SymbolTable& symbols)
  : lexer(lexer), reporter(reporter), symbols(symbols) {
    advance();
}

std::vector<std::shared_ptr<Statement>> Parser::parse() {
    std::vector<std::shared_ptr<Statement>> statements;

    while (!check(TokenType::END_OF_FILE)) {
        LOG(LogLevel::DEBUG, "[parser] el token actual a revisar es: " + peek().getPrint());

        if (check(TokenType::RBRACE)) {
            LOG(LogLevel::ERROR, "[parse] Se encontró una llave de cierre '}' fuera de un bloque");
            ErrorReporter::getInstance().report("Se encontró una llave de cierre '}' fuera de un bloque", peek().row, peek().column);
            advance();
            continue;
        }

        if (check(TokenType::SEMICOLON)) {
            advance();
            if (check(TokenType::SEMICOLON)) {
                LOG(LogLevel::ERROR, "[parse] Se encontraron múltiples ';' vacíos seguidos");
                ErrorReporter::getInstance().report("Uso redundante de ';' sin instrucción entre ellos", peek().row, peek().column);
                advance();
                continue;
            }

            continue;
        }

        if (check(TokenType::KEYWORD_ELSE)) {
            LOG(LogLevel::ERROR, "[parse] Se encontró 'sino' sin un bloque 'si' anterior válido");
            ErrorReporter::getInstance().report("Se encontró 'sino' sin un bloque 'si' anterior válido", peek().row, peek().column);
            advance();
            continue;
        }

        if (check(TokenType::NUMBER) || check(TokenType::STRING_LITERAL) || check(TokenType::BOOLEAN_LITERAL) || check(TokenType::LPAREN)) {
            LOG(LogLevel::ERROR, "[parse] Se encontró una expresión suelta sin asignación ni impresión");
            ErrorReporter::getInstance().report("Expresión suelta no permitida. Debe asignarse o mostrarse explícitamente.", peek().row, peek().column);
            synchronize();
            continue;
        }

        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        } else {
            LOG(LogLevel::ERROR, "[parse] statement no valido");
            ErrorReporter::getInstance().report("Statement no válido", peek().row, peek().column);
            synchronize();
        }
    }

    ErrorReporter::getInstance().printSummary();
    ErrorReporter::getInstance().clear();

    return statements;
}


const Token& Parser::peek() const
{
  return currentToken;
}

void Parser::advance()
{
  previousToken = currentToken;
  currentToken = lexer.nextToken();
}

bool Parser::check(TokenType type) const
{
  return currentToken.type == type;
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

  ErrorReporter::getInstance().report("[" + scopeMethod + "] " + msgError, token.row, token.column);

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
    advance();

    while (currentToken.type != TokenType::END_OF_FILE) {
        if (previousToken.type == TokenType::SEMICOLON) return;

        switch (currentToken.type) {
            case TokenType::KEYWORD_INT:
            case TokenType::KEYWORD_STR:
            case TokenType::KEYWORD_BOOL:
            case TokenType::KEYWORD_IF:
            case TokenType::KEYWORD_WHILE:
            case TokenType::KEYWORD_FOR:
            case TokenType::KEYWORD_PRINT:
            case TokenType::KEYWORD_INPUT:
            case TokenType::LBRACE:
            case TokenType::RBRACE:
                return; // Puntos seguros donde puedes retomar el parsing
            default:
                break;
        }

        advance();
    }
}

std::shared_ptr<Statement> Parser::parseStatement()
{
    if (check(TokenType::RBRACE)) {
        return nullptr;
    }

    if (check(TokenType::KEYWORD_INT) || check(TokenType::KEYWORD_STR) || check(TokenType::KEYWORD_BOOL)) 
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

    ErrorReporter::getInstance().report("Statement no válido", peek().row, peek().column);
    return nullptr;
}

VarType Parser::tokenTypeToVarType(const Token& token) {
    switch (token.type) {
        case TokenType::KEYWORD_INT: return VarType::INT;
        case TokenType::KEYWORD_STR: return VarType::STR;
        case TokenType::KEYWORD_BOOL: return VarType::BOOL;
        default: return VarType::INVALID;
    }
}


std::shared_ptr<Statement> Parser::parseVarDeclaration() {
    Token typeToken = previousToken;
    VarType varType = tokenTypeToVarType(typeToken);

    if (!expect(TokenType::IDENTIFIER, "parseVarDeclaration", "Se esperaba el nombre de la variable.")) {
        return nullptr;
    }
    Token name = previousToken;

    std::shared_ptr<Expression> initializer = nullptr;
    if (match(TokenType::EQ)) {
        initializer = parseExpression();
    }

    if (!expect(TokenType::SEMICOLON, "parseVarDeclaration", "Se esperaba ';' después de la declaración de variable.")) {
        return nullptr;
    }

    if (!symbols.declare(name.value, varType)) {
        reporter.report("Variable '" + name.value + "' ya fue declarada en este ámbito.", name.row, name.column);
    }

    return std::make_shared<VarDeclStatement>(varType, name.value, initializer);
}

std::shared_ptr<Statement> Parser::parseDeclaration() {
    if (match(TokenType::KEYWORD_INT) || match(TokenType::KEYWORD_STR) || match(TokenType::KEYWORD_BOOL)) {
        return parseVarDeclaration();
    }
    return parseStatement();
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
      ErrorReporter::getInstance().report("Se esperaba una expresión válida después de 'mostrar'", peek().row, peek().column);
      return nullptr;
    }

    if (!match(TokenType::SEMICOLON))
    {
      LOG(LogLevel::ERROR, "[parsePrint] Se esperaba ';' después de 'mostrar'");
      ErrorReporter::getInstance().report("Se esperaba ';' al final de la instrucción 'mostrar'", peek().row, peek().column);
      return nullptr;
    }

    return std::make_shared<PrintStatement>(expr);
}

std::shared_ptr<Statement> Parser::parseInput() {
  if (!match(TokenType::IDENTIFIER)) {
    LOG(LogLevel::ERROR, "[parseInput] Se esperaba un identificador después de 'entrada'.");
    ErrorReporter::getInstance().report("Se esperaba un nombre de variable después de 'entrada'", peek().row, peek().column);
    return nullptr;
  }

  std::string name = previous().value;

  if (!symbols.lookup(name)) {
    std::string msg = "La variable '" + name + "' no ha sido declarada previamente para entrada.";
    LOG(LogLevel::ERROR, "[parseInput] " + msg);
    ErrorReporter::getInstance().report(msg, peek().row, peek().column);
    return nullptr;
  }

  if (!match(TokenType::SEMICOLON)) {
    LOG(LogLevel::ERROR, "[parseInput] Se esperaba ';' después de 'entrada'");
    ErrorReporter::getInstance().report("Se esperaba ';' después de la entrada", peek().row, peek().column);
    return nullptr;
  }

  return std::make_shared<InputStatement>(name);
}

std::shared_ptr<Statement> Parser::parseBlock() {
    if (!match(TokenType::LBRACE)) {
        LOG(LogLevel::ERROR, "[parseBlock] Se esperaba '{' para iniciar un bloque");
        return nullptr;
    }

    symbols.enterScope();
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
      ErrorReporter::getInstance().report("Se esperaba '}' para cerrar el bloque", peek().row, peek().column);
      symbols.exitScope();
      return nullptr;
    }

    if (statements.empty()) {
      LOG(LogLevel::ERROR, "[parseBlock] El bloque está vacío y no contiene ninguna instrucción ejecutable");
      ErrorReporter::getInstance().report("El bloque está vacío y no contiene ninguna instrucción", peek().row, peek().column);
      symbols.exitScope();
      return nullptr;
    }

    symbols.exitScope();
    return std::make_shared<BlockStatement>(statements);
}

std::shared_ptr<Statement> Parser::parseAssignment()
{
  if (!match(TokenType::IDENTIFIER)) {
    LOG(LogLevel::ERROR, "[parseAssignment] Se esperaba un identificador al inicio de la asignación. Se encontró: " + peek().getPrint());
    ErrorReporter::getInstance().report("Se esperaba un identificador para la asignación", peek().row, peek().column);
    return nullptr;
  }

  std::string name = previous().value;

  if (!symbols.lookup(name)) {
    std::string msg = "La variable '" + name + "' no ha sido declarada previamente.";
    LOG(LogLevel::ERROR, "[parseAssignment] " + msg);
    ErrorReporter::getInstance().report(msg, peek().row, peek().column);
    return nullptr;
  }

  if (!match(TokenType::ASSIGN)) {
    LOG(LogLevel::ERROR, "[parseAssignment] Se esperaba '=' en la asignación después de '" + name + "'");
    ErrorReporter::getInstance().report("Se esperaba '=' después del identificador", peek().row, peek().column);
    return nullptr;
  }

  if (check(TokenType::SEMICOLON) || check(TokenType::RPAREN) || check(TokenType::END_OF_FILE)) {
    LOG(LogLevel::ERROR, "[parseAssignment] Se esperaba una expresión después de '=' pero se encontró: " + peek().getPrint());
    ErrorReporter::getInstance().report("Se esperaba una expresión después de '='", peek().row, peek().column);
    return nullptr;
  }

  auto expr = parseExpression();
  if (!expr) {
    LOG(LogLevel::ERROR, "[parseAssignment] Expresión nula en la asignación. Se esperaba un valor después de '='");
    ErrorReporter::getInstance().report("Expresión inválida en la asignación", peek().row, peek().column);
    return nullptr;
  }

  if (!match(TokenType::SEMICOLON)) {
    LOG(LogLevel::ERROR, "[parseAssignment] Se esperaba ';' después de la asignación");
    ErrorReporter::getInstance().report("Falta ';' después de la asignación", peek().row, peek().column);
    return nullptr;
  }

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
        logTokenContext();
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
    std::string name = previous().value;

    if (!symbols.lookup(name)) {
      std::string msg = "La variable '" + name + "' no ha sido declarada.";
      LOG(LogLevel::ERROR, "[parsePrimary] " + msg);
      ErrorReporter::getInstance().report(msg, peek().row, peek().column);
      return nullptr;
    }

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
    return previousToken;
}

