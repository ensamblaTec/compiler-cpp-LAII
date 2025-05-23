#include <unordered_set>

#include "parser.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "error_reporter.hpp"
#include "symbol_table.hpp"

void Parser::logTokenContext(const std::string& header) const {
  const Token& prev = current > 0 ? tokens[current - 1] : tokens[current];
  const Token& curr = peek();
  const Token& next = (current + 1 < tokens.size()) ? tokens[current + 1] : curr;

  LOG(LogLevel::ERROR, header + "\n  Token anterior: " + prev.getPrint() +
                      "\n  Token actual:   " + curr.getPrint() +
                      "\n  Token siguiente:" + next.getPrint());
}

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0), symbols() {}

std::vector<std::shared_ptr<Statement>> Parser::parse()
{
  semanticErrorOcurred = false;
  std::vector<std::shared_ptr<Statement>> statements;
  while (!check(TokenType::END_OF_FILE))
  {
    LOG(LogLevel::DEBUG, "[parser] el token actual a revisar es: " + tokens[current].getPrint());
    if (check(TokenType::RBRACE)) {
      LOG(LogLevel::ERROR, "[parse] Se encontró una llave de cierre '}' fuera de un bloque");
      ErrorReporter::getInstance().report("Se encontró una llave de cierre '}' fuera de un bloque", peek().row, peek().column);
      advance();
      continue;
    }

    if (check(TokenType::SEMICOLON) && tokens[current + 1].type == TokenType::SEMICOLON) {
      LOG(LogLevel::ERROR, "[parse] Se encontraron múltiples ';' vacíos seguidos");
      ErrorReporter::getInstance().report("Uso redundante de ';' sin instrucción entre ellos", peek().row, peek().column);
      advance(); 
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

    auto smtm = parseStatement();
    if (smtm)
    {
      statements.push_back(smtm);
    } else
    {
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parse] Statement no válido: " + tokens[current].getPrint());
        ErrorReporter::getInstance().report("Statement no válido 11111", peek().row, peek().column);
      }
      synchronize();
    }
  }

  ErrorReporter::getInstance().printSummary();
  ErrorReporter::getInstance().clear();

  symbols.printTable();
  symbols.printHistory();
  symbols.exportToCSV("output/tabla_simbolos.txt");

  return statements;
}

const Token& Parser::peek() const
{
  return tokens[current];
}

Token Parser::peekNext() const {
  return current + 1 >= tokens.size() ? tokens.back() : tokens[current+1];
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

bool Parser::isAtEnd() const {
  return peek().type == TokenType::END_OF_FILE;
}

void Parser::synchronize() {
  advance();
  while (!isAtEnd()) {
    if (previous().type == TokenType::SEMICOLON) {
      return;
    }

    switch (peek().type) {
      case TokenType::RBRACE:
      case TokenType::RPAREN:
      case TokenType::KEYWORD_INT:
      case TokenType::KEYWORD_STR:
      case TokenType::KEYWORD_BOOL:
      case TokenType::KEYWORD_IF:
      case TokenType::KEYWORD_WHILE:
      case TokenType::KEYWORD_FOR:
      case TokenType::KEYWORD_PRINT:
      case TokenType::KEYWORD_INPUT:
      case TokenType::IDENTIFIER:
        return; 
      default:
        advance();
    }
  }
}

std::shared_ptr<Statement> Parser::parseStatement()
{
  LOG(LogLevel::DEBUG, "[parseStatement] Token a revisar actual: " + peek().getPrint());

  if (check(TokenType::RBRACE)) {
      return nullptr;
  }

  if (check(TokenType::KEYWORD_INT) || check(TokenType::KEYWORD_STR) || check(TokenType::KEYWORD_BOOL)) 
      return parseDeclaration();

  if (check(TokenType::IDENTIFIER)) {
    std::string palabra = peek().value;
    static const std::unordered_set<std::string> tiposInvalidos = {
      "int", "str", "float", "double", "char", "var"
    };

    Token prev = previous();
    Token next = peekNext();

    if ((prev.type == TokenType::SEMICOLON || prev.type == TokenType::END_OF_FILE) &&
        tiposInvalidos.count(palabra) &&
        next.type == TokenType::IDENTIFIER) {
        
        LOG(LogLevel::ERROR, "[parseStatement] Tipo no válido: '" + palabra + "'");
        ErrorReporter::getInstance().report("Tipo no válido: '" + palabra + "'. Usa 'entero', 'texto', bool", peek().row, peek().column);
        advance();
        synchronize();
        return nullptr;
    }

    return parseAssignment();
  }

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

  if (!semanticErrorOcurred) {
    LOG(LogLevel::ERROR, "[parseStatement] Token inesperado: " + peek().getPrint() +
                        " (línea: " + std::to_string(peek().row + 1) +
                        ", columna: " + std::to_string(peek().column) + ")");
    ErrorReporter::getInstance().report("Statement no válido 2222", peek().row, peek().column);
  }

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
      ErrorReporter::getInstance().report("Tipo desconocido en declaración. Se encontró: " + peek().getPrint(), peek().row, peek().column);
      semanticErrorOcurred = true;
      return nullptr;
    }

    if (!match(TokenType::IDENTIFIER)) {
      Token token = peek();
      LOG(LogLevel::ERROR, "[parseDeclaration] Se esperaba un identificador después del tipo. Se encontró: " + token.getPrint());
      ErrorReporter::getInstance().report("Se esperaba un identificador después del tipo. Se encontró: " + token.getPrint(), token.row, token.column);
      return nullptr;
    }

    std::string name = previous().value;
    int line = previous().row;
    int column = previous().column;
    Symbol symbol(name, type, SymbolCategory::Variable, symbols.getCurrentScope(), "", line, column);

    std::string msg = std::string("symbol:\n") + "\tname: " + symbol.name + ",\tvalue: " + symbol.value;
    LOG(LogLevel::WARNING, msg);

    LOG(LogLevel::DEBUG, " Declarando '" + name + "' en scope: " + symbols.getCurrentScope());

    if (!symbols.declare(symbol)) {
      std::string msg = "La variable '" + name + "' ya fue declarada en este bloque.";
      LOG(LogLevel::ERROR, "[parseDeclaration] " + msg);
      ErrorReporter::getInstance().report(msg, peek().row, peek().column);
      semanticErrorOcurred = true;
      return nullptr;
    }

    if (match(TokenType::ASSIGN)) {
      auto expr = parseExpression();

      if (!expr) {
        LOG(LogLevel::ERROR, "[parseDeclaration] Expresión inválida en declaración con asignación");
        ErrorReporter::getInstance().report("Expresión inválida en declaración con asignación", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
      }

      std::string exprType = inferType(expr);
      if (exprType == "error") {
        semanticErrorOcurred = true;
        return nullptr;
      }

      if (exprType != type) {
        std::string msg = "El tipo de la expresión ('" + exprType + "') no coincide con el tipo declarado ('" + type + "')";
        LOG(LogLevel::ERROR, "[parseDeclaration] " + msg);
        ErrorReporter::getInstance().report(msg, peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
      }

      if (!match(TokenType::SEMICOLON)) {
        LOG(LogLevel::ERROR, "[parseDeclaration] Se esperaba ';' después de la asignación en la declaración");
        ErrorReporter::getInstance().report("Se esperaba ';' después de la asignación en la declaración", peek().row, peek().column);
        return nullptr;
      }

      std::string value = "";
      if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr)) value = num->value;
      else if (auto str = std::dynamic_pointer_cast<StringExpr>(expr)) value = str->text;
      else if (auto boolean = std::dynamic_pointer_cast<BooleanExpr>(expr)) value = boolean->value ? "verdadero" : "falso";
      symbols.updateValue(name, value, line, column);
      LOG(LogLevel::WARNING, "[updateValue] " + msg);

      return std::make_shared<Assignment>(name, expr);
    }

    if (!match(TokenType::SEMICOLON)) {
      LOG(LogLevel::ERROR, "[parseDeclaration] Se esperaba ';' después de la declaración");
      ErrorReporter::getInstance().report("Se esperaba ';' después de la declaración", peek().row, peek().column);
      return nullptr;
    }
    return std::make_shared<Declaration>(type, name);
}

std::shared_ptr<Statement> Parser::parseIf() {
    if (!match(TokenType::LPAREN)) {
      LOG(LogLevel::ERROR, "[parseIf] Se esperaba '(' después de 'si'");
      ErrorReporter::getInstance().report("Se esperaba '(' después de 'si'", peek().row, peek().column);
      return nullptr;
    }

    auto condition = parseExpression();
    if (!condition) {
        LOG(LogLevel::ERROR, "[parseIf] Condición inválida en 'si'");
        ErrorReporter::getInstance().report("Condición inválida en 'si'", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
    }

    validateVarDeclared(condition);

    std::string exprType = inferType(condition);
    if (exprType == "error") {
      semanticErrorOcurred = true;
      return nullptr;
    }

    if (exprType != "bool") {
        LOG(LogLevel::ERROR, "[parseIf] La condición del 'si' no es booleana");
        ErrorReporter::getInstance().report("La condición del 'si' debe ser de tipo booleano", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
    }

    if (!expect(TokenType::RPAREN, "parseIf", "Se esperaba ')' después de la condición"))
        return nullptr;

    auto thenBranch = parseBlock("if");

    std::shared_ptr<Statement> elseBranch = nullptr;
    if (match(TokenType::KEYWORD_ELSE)) {
      elseBranch = parseBlock("else"); 
    }

    return std::make_shared<IfStatement>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Statement> Parser::parseWhile() {
    if (!match(TokenType::LPAREN)) {
        LOG(LogLevel::ERROR, "[parseWhile] Se esperaba '(' después de 'mientras'");
        ErrorReporter::getInstance().report("Se esperaba '(' después de 'mientras'", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
    }

    auto condition = parseExpression();
    if (!condition) {
        LOG(LogLevel::ERROR, "[parseWhile] Condición inválida en 'mientras'");
        ErrorReporter::getInstance().report("Condición inválida en 'mientras'", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
    }

    validateVarDeclared(condition);
    std::string exprType = inferType(condition);
    if (exprType == "error") {
      semanticErrorOcurred = true;
      return nullptr;
    }

    if (exprType != "bool") {
        LOG(LogLevel::ERROR, "[parseWhile] La condición del 'mientras' no es booleana");
        ErrorReporter::getInstance().report("La condición del 'mientras' debe ser de tipo booleano", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
    }

    if (!expect(TokenType::RPAREN, "parseWhile", "Se esperaba ')' después de la condición"))
        return nullptr;

    auto body = parseBlock("while");

    return std::make_shared<WhileStatement>(condition, body);
}

std::shared_ptr<Statement> Parser::parseForInitializer() {
  if (check(TokenType::IDENTIFIER)) {
    std::string name = advance().value;

    if (!expect(TokenType::ASSIGN, "parseForInitializer", "Se esperaba '=' en la asignación"))
      return nullptr;

    auto expr = parseExpression();
    if (!expr) {
        LOG(LogLevel::ERROR, "[parseForInitializer] Expresión nula en la asignación");
        ErrorReporter::getInstance().report("Expresión nula en la asignación", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
    }

    return std::make_shared<Assignment>(name, expr);
  } else {
    auto expr = parseExpression();
    if (expr) {
        return std::make_shared<ExpressionStatement>(expr);
    }
  }

  return nullptr;
}

std::shared_ptr<Statement> Parser::parseFor() {
    if (!match(TokenType::LPAREN)) {
      LOG(LogLevel::ERROR, "[parseFor] Se esperaba '(' después de 'para'");
      ErrorReporter::getInstance().report("Se esperaba '(' después de 'para'", peek().row, peek().column);
      return nullptr;
    }

    std::shared_ptr<Statement> initializer = parseForInitializer();

    if (!initializer) {
      LOG(LogLevel::ERROR, "[parseFor] Inicializador inválido");
      ErrorReporter::getInstance().report("Inicializador inválido", peek().row, peek().column);
      semanticErrorOcurred = true;
      synchronize(); 
      return nullptr;
    }

    if (!expect(TokenType::SEMICOLON, "parseFor", "Se esperaba ';' después de la inicialización")) {
      synchronize();
      return nullptr;
    }

    auto condition = parseExpression();
    if (!condition) {
      LOG(LogLevel::ERROR, "[parseFor] Condición inválida en 'para'");
      ErrorReporter::getInstance().report("Condición inválida en 'para'", peek().row, peek().column); 
      semanticErrorOcurred = true;
      synchronize();
      return nullptr;
    }

    validateVarDeclared(condition);
    std::string exprType = inferType(condition);
    if (exprType == "error") {
      semanticErrorOcurred = true;
      return nullptr;
    }

    if (exprType != "bool") {
      LOG(LogLevel::ERROR, "[parseFor] La condición del 'para' no es bool");
      ErrorReporter::getInstance().report("La condición del 'para' debe ser de tipo booleano", peek().row, peek().column);
      semanticErrorOcurred = true;
      return nullptr;
    }

    if (!expect(TokenType::SEMICOLON, "parseFor", "Se esperaba ';' después de la condición")) {
      synchronize();
      return nullptr;
    }

    // if (!match(TokenType::SEMICOLON)) {
    //   LOG(LogLevel::ERROR, "[parseFor] Se esperaba ';' después de la inicialización");
    //   ErrorReporter::getInstance().report("Se esperaba ';' después de la inicialización", peek().row, peek().column);
    //   synchronize();
    //   return nullptr;
    // }

    std::shared_ptr<Statement> increment = parseIncrement();
    if (!std::dynamic_pointer_cast<Assignment>(increment)) {
      LOG(LogLevel::ERROR, "[parseFor] El incremento del 'para' debe ser una asignación (por ejemplo: i = i + 1)");
      ErrorReporter::getInstance().report("El incremento del 'para' debe ser una asignación válida", peek().row, peek().column);
      semanticErrorOcurred = true;
      return nullptr;
    }

    if (!increment) {
      LOG(LogLevel::ERROR, "[parseFor] Incremento inválido en 'para'");
      ErrorReporter::getInstance().report("Incremento inválido en 'para'", peek().row, peek().column);
      semanticErrorOcurred = true;
      synchronize();
      return nullptr;
    }

    if (!expect(TokenType::RPAREN, "parseFor", "Se esperaba ')' después de los parámetros. se encuentra " + peek().getPrint())) {
      synchronize();
      return nullptr;
    }

    auto body = parseBlock("for");

    return std::make_shared<ForStatement>(initializer, condition, increment, body);
}

std::shared_ptr<Statement> Parser::parseIncrement() {
    if (!check(TokenType::IDENTIFIER)) {
        LOG(LogLevel::ERROR, "[parseIncrement] Se esperaba un identificador para el incremento.");
        ErrorReporter::getInstance().report("El incremento del 'para' debe iniciar con un identificador", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
    }

    std::string name = peek().value;
    auto assignment = parseAssignment(false);

    if (!assignment) {
        LOG(LogLevel::ERROR, "[parseIncrement] El incremento no es una asignación válida.");
        ErrorReporter::getInstance().report("El incremento del 'para' debe ser una asignación válida", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
    }

    auto assignExpr = std::dynamic_pointer_cast<Assignment>(assignment);
    if (assignExpr) {
        std::string type = inferType(assignExpr->expr);
        if (type != "entero" && type != "decimal") {
            LOG(LogLevel::ERROR, "[parseIncrement] El incremento debe ser numérico.");
            ErrorReporter::getInstance().report("El incremento del 'para' debe ser entero o decimal", peek().row, peek().column);
            semanticErrorOcurred = true;
            return nullptr;
        }
    }

    return assignment;
}

std::shared_ptr<Statement> Parser::parsePrint() {
    auto expr = parseExpression();

    if (!expr) {
      LOG(LogLevel::ERROR, "[parsePrint] Expresión inválida después de 'mostrar'");
      ErrorReporter::getInstance().report("Se esperaba una expresión válida después de 'mostrar'", peek().row, peek().column);
      semanticErrorOcurred = true;
      synchronize();
      return nullptr;
    }

    validateVarDeclared(expr);

    if (!match(TokenType::SEMICOLON))
    {
      LOG(LogLevel::ERROR, "[parsePrint] Se esperaba ';' después de 'mostrar'");
      ErrorReporter::getInstance().report("Se esperaba ';' al final de la instrucción 'mostrar'", peek().row, peek().column);
      synchronize();
      return nullptr;
    }

    return std::make_shared<PrintStatement>(expr);
}

std::shared_ptr<Statement> Parser::parseInput() {
  if (!match(TokenType::IDENTIFIER)) {
    LOG(LogLevel::ERROR, "[parseInput] Se esperaba un identificador después de 'entrada'");
    ErrorReporter::getInstance().report("Se esperaba un nombre de variable después de 'entrada'", peek().row, peek().column);
    semanticErrorOcurred = true;
    return nullptr;
  }

  std::string name = previous().value;
  int line = previous().row;
  int column = previous().column;

  validateVarDeclared(std::make_shared<VariableExpr>(name));

  std::string type = symbols.getType(name);
  if (type != "entero" && type != "decimal" && type != "texto" && type != "bool") {
    std::string msg = "Tipo no válido para entrada: '" + type + "'";
    LOG(LogLevel::ERROR, "[parseInput] " + msg);
    ErrorReporter::getInstance().report(msg, line, column);
  semanticErrorOcurred = true;
    return nullptr;
  }

  if (!match(TokenType::SEMICOLON)) {
    LOG(LogLevel::ERROR, "[parseInput] Se esperaba ';' después de 'entrada'");
    ErrorReporter::getInstance().report("Se esperaba ';' después de la entrada", peek().row, peek().column);
  semanticErrorOcurred = true;
    return nullptr;
  }

  return std::make_shared<InputStatement>(name);
}

std::shared_ptr<Statement> Parser::parseBlock(const std::string& context) {
    if (!match(TokenType::LBRACE)) {
      LOG(LogLevel::ERROR, "[parseBlock] Se esperaba '{' para iniciar un bloque");
      ErrorReporter::getInstance().report("Se esperaba '{' para iniciar un bloque", peek().row, peek().column);
      semanticErrorOcurred = true;
      return nullptr;
    }

    symbols.enterScope(context);

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
  semanticErrorOcurred = true;
      symbols.exitScope();
      return nullptr;
    }

    if (statements.empty()) {
      LOG(LogLevel::ERROR, "[parseBlock] El bloque está vacío y no contiene ninguna instrucción ejecutable");
      ErrorReporter::getInstance().report("El bloque está vacío y no contiene ninguna instrucción", peek().row, peek().column);
  semanticErrorOcurred = true;
      symbols.exitScope();
      return nullptr;
    }

    symbols.exitScope();
    return std::make_shared<BlockStatement>(statements);
}

std::shared_ptr<Statement> Parser::parseAssignment(bool consumeSemicolon)
{
  if (!match(TokenType::IDENTIFIER)) {
    LOG(LogLevel::ERROR, "[parseAssignment] Se esperaba un identificador al inicio de la asignación. Se encontró: " + peek().getPrint());
    ErrorReporter::getInstance().report("Se esperaba un identificador para la asignación", peek().row, peek().column);
    semanticErrorOcurred = true;
    return nullptr;
  }

  std::string name = previous().value;
  int line = previous().row;
  int column = previous().column;

  if (!symbols.validateVarDeclared(name, peek().row, peek().column)) {
    semanticErrorOcurred = true;
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
    semanticErrorOcurred = true;
    return nullptr;
  }

  validateVarDeclared(expr);

  std::string varType = symbols.getType(name);
  std::string exprType = inferType(expr); 

  if (exprType == "error" || varType == "error") {
    semanticErrorOcurred = true;
    return nullptr;
  }

  if (varType == "bool" && exprType == "entero") {
    if (auto number = std::dynamic_pointer_cast<NumberExpr>(expr)) {
      if (number->value != "0" && number-> value != "1") {
        LOG(LogLevel::ERROR, "[parseAssignment] Solo se puede asignar 0 o 1 a una variable bool");
        ErrorReporter::getInstance().report("Solo se puede asignar 0 o 1 a una variable bool", peek().row, peek().column);
        semanticErrorOcurred = true;
        return nullptr;
      }
    }
  } else if (varType != exprType) {
    std::string msg = "Error de tipo: no se puede asignar una expresión de tipo '" + exprType +
                      "' a una variable de tipo '" + varType + "'.";
    LOG(LogLevel::ERROR, "[parseAssignment] " + msg);
    ErrorReporter::getInstance().report(msg, peek().row, peek().column);
    semanticErrorOcurred = true;
    return nullptr;
  }

  std::string value = "";
  if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr)) value = num->value;
  else if (auto str = std::dynamic_pointer_cast<StringExpr>(expr)) value = str->text;
  else if (auto boolean = std::dynamic_pointer_cast<BooleanExpr>(expr)) value = boolean->value ? "verdadero" : "falso";

  symbols.updateValue(name, value, line, column);

  if (consumeSemicolon) {
    if (!match(TokenType::SEMICOLON)) {
      LOG(LogLevel::ERROR, "[parseAssignment] Se esperaba ';' después de la asignación");
      ErrorReporter::getInstance().report("Falta ';' después de la asignación", peek().row, peek().column);
      return nullptr;
    }
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
      logTokenContext("[parseExpression] Operador binario sin operando");
      LOG(LogLevel::ERROR, "[parseExpression] Se encontró un operador binario sin operando izquierdo: " + peek().getPrint());
      ErrorReporter::getInstance().report("Se encontró un operador binario sin operando izquierdo: ", peek().row, peek().column);
      semanticErrorOcurred = true;
      return nullptr;
    }

    return parseAssignmentExpression();
}

std::shared_ptr<Expression> Parser::parseAssignmentExpression() {
    auto expr = parseOr();

    if (match(TokenType::ASSIGN)) {
      if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        auto value = parseAssignmentExpression();
        if (!value) {
          LOG(LogLevel::ERROR, "[parseAssignmentExpression] Expresión inválida después del '='");
          ErrorReporter::getInstance().report("Expresión inválida después del '='", peek().row, peek().column);
  semanticErrorOcurred = true;
          return nullptr;
        }
        return std::make_shared<AssignmentExpr>(varExpr->name, value);
      } else {
        LOG(LogLevel::ERROR, "[parseAssignmentExpression] Se esperaba una variable a la izquierda del '='");
        ErrorReporter::getInstance().report("Se esperaba una variable a la izquierda del '='", peek().row, peek().column);
  semanticErrorOcurred = true;
        return nullptr;
      }
    }

    return expr;
}

std::shared_ptr<Expression> Parser::parseOr() {
    auto expr = parseAnd();
    if (!expr) {
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'");
        ErrorReporter::getInstance().report("Operando izquierdo inválido antes del operador '+' o '-'", peek().row, peek().column);
        semanticErrorOcurred = true;
      }
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
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'");
        ErrorReporter::getInstance().report("Operando izquierdo inválido antes del operador '+' o '-'", peek().row, peek().column);
        semanticErrorOcurred = true;
      }
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
    if (!semanticErrorOcurred) {
      LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'");
      ErrorReporter::getInstance().report("Operando izquierdo inválido antes del operador '+' o '-'", peek().row, peek().column);
      semanticErrorOcurred = true;
    }
    return nullptr;
  }
  while (match(TokenType::LT) || match(TokenType::LTE) ||
    match(TokenType::GT) || match(TokenType::GTE)) {
    std::string op = previous().value;
    auto right = parseTerm();
    
    if (!right) {
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parseComparison] Operando derecho inválido para el operador: '" + op + "'");
        ErrorReporter::getInstance().report("Operando derecho inválido para el operador: '" + op + "'", peek().row, peek().column);
        semanticErrorOcurred = true;
      }
      return nullptr;
    }

    expr = std::make_shared<BinaryExpr>(op, expr, right);
  }
  return expr;
}

std::shared_ptr<Expression> Parser::parseEquality() {
  auto expr = parseComparison();
  if (!expr) {
    if (!semanticErrorOcurred) {
      LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'");
      ErrorReporter::getInstance().report("Operando izquierdo inválido antes del operador '+' o '-'", peek().row, peek().column);
      semanticErrorOcurred = true;
    }
    return nullptr;
  }
  while (match(TokenType::EQ) || match(TokenType::NEQ)) {
    std::string op = previous().value;
    auto right = parseComparison();

    if (!right) {
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parseEquality] Operando derecho inválido para el operador: '" + op + "'");
        ErrorReporter::getInstance().report("Operando derecho inválido para el operador: '" + op + "'", peek().row, peek().column);
        semanticErrorOcurred = true;
      }
      return nullptr;
    }
    expr = std::make_shared<BinaryExpr>(op, expr, right);
  }
  return expr;
}

std::shared_ptr<Expression> Parser::parseAdditive() {
    auto expr = parseTerm();

    if (!expr) {
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parseAdditive] Operando izquierdo inválido antes del operador '+' o '-'");
        ErrorReporter::getInstance().report("Operando izquierdo inválido antes del operador '+' o '-'", peek().row, peek().column);
        semanticErrorOcurred = true;
      }
      return nullptr;
    }

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        std::string op = previous().value;
        if (isBinaryOperator(peek().type))
        {
          if (!semanticErrorOcurred) {
            LOG(LogLevel::ERROR, "[parseAdditive] Operadores binarios consecutivos inválidos: '" + op + "' seguido de '" + peek().value + "'");
            ErrorReporter::getInstance().report("Operadores binarios consecutivos inválidos: '" + op + "' seguido de '" + peek().value + "'", peek().row, peek().column);
            semanticErrorOcurred = true;
          }
          return nullptr;
        }

        auto right = parseTerm();
        if (!right)
        {
          if (!semanticErrorOcurred) {
            LOG(LogLevel::ERROR, "[parseAdditive] Operando derecho inválido para el operador: '" + op + "'");
            ErrorReporter::getInstance().report("Operando derecho inválido para el operador: '" + op + "'", peek().row, peek().column);
            semanticErrorOcurred = true;
          }
          return nullptr;
        }

        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::parseTerm() {
  auto expr = parseUnary();

  if (!expr) {
    if (!semanticErrorOcurred) {
      LOG(LogLevel::ERROR, "[parseTerm] Operando izquierdo inválido antes del operador '+' o '-'");
      ErrorReporter::getInstance().report("Operando izquierdo inválido antes del operador '+' o '-'", peek().row, peek().column);
      semanticErrorOcurred = true;
    }
    return nullptr;
  }

  while (match(TokenType::MUL) || match(TokenType::SLASH)) {
    std::string op = previous().value;
    if (isBinaryOperator(peek().type))
    {
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parseTerm] Operadores binarios consecutivos inválidos: '" + op + "' seguido de '" + peek().value + "'");
        ErrorReporter::getInstance().report("Operadores binarios consecutivos inválidos: '" + op + "' seguido de '" + peek().value + "'", peek().row, peek().column);
        semanticErrorOcurred = true;
      }
      return nullptr;
    }

    auto right = parseUnary();
    if (!right) {
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parseTerm] Operando derecho inválido para el operador: '" + op + "'");
        ErrorReporter::getInstance().report("Operando derecho inválido para el operador: '" + op + "'", peek().row, peek().column);
        semanticErrorOcurred = true;
      }
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

    if (!symbols.isDeclared(name)) {
      if (!semanticErrorOcurred) {
        std::string msg = "La variable '" + name + "' no ha sido declarada.";
        LOG(LogLevel::ERROR, "[parsePrimary] " + msg);
        ErrorReporter::getInstance().report(msg, peek().row, peek().column);
        semanticErrorOcurred = true;
      }
      return nullptr;
    }

    return std::make_shared<VariableExpr>(name);
  }
  if (match(TokenType::LPAREN)) {
    if (check(TokenType::RPAREN)) {
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parsePrimary] Se encontró un paréntesis vacío '()'. Se esperaba una expresión dentro");
        ErrorReporter::getInstance().report("Se encontró un paréntesis vacío '()'. Se esperaba una expresión dentro", peek().row, peek().column);
        semanticErrorOcurred = true;
      }
      return nullptr;
    }

    auto expr = parseExpression();
    if (!expr) {
      if (!semanticErrorOcurred) {
        LOG(LogLevel::ERROR, "[parsePrimary] Se esperaba una expresión válida dentro del paréntesis");
        ErrorReporter::getInstance().report("Se esperaba una expresión válida dentro del paréntesis", peek().row, peek().column);
        semanticErrorOcurred = true;
      }
      return nullptr;
    }

    if (!expect(TokenType::RPAREN, "parsePrimary", "Se esperaba ')' para cerrar la expresión"))
        return nullptr;

    return expr;
  }

  LOG(LogLevel::ERROR, "[parsePrimary] Expresión desconocida: " + peek().value);
  ErrorReporter::getInstance().report("Expresión desconocida: " + peek().value, peek().row, peek().column);
  semanticErrorOcurred = true;
  return nullptr;
}

std::shared_ptr<Expression> Parser::parseUnary() {
    if (match(TokenType::NOT) || match(TokenType::MINUS)) {
        std::string op = previous().value;
        auto right = parseUnary();

        if (!right) {
          if (!semanticErrorOcurred) {
            LOG(LogLevel::ERROR, "[parseTerm] Operando derecho inválido para el operador: '" + op + "'");
            ErrorReporter::getInstance().report("Operando derecho inválido para el operador: '" + op + "'", peek().row, peek().column);
            semanticErrorOcurred = true;
          }
          return nullptr;
        }

        return std::make_shared<UnaryExpr>(op, right);
    }
    return parsePrimary();
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

std::string Parser::inferType(const std::shared_ptr<Expression>& expr) {
  if (!expr) return "error";

  if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr)) return "entero";
  if (auto str = std::dynamic_pointer_cast<StringExpr>(expr)) return "texto";
  if (auto boolLit = std::dynamic_pointer_cast<BooleanExpr>(expr)) return "bool";
  if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
    if (!symbols.validateVarDeclared(var->name, peek().row, peek().column)) {
      return "error";
    }
    return symbols.getType(var->name);
  }

  if (auto bin = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
    std::string leftType = inferType(bin->left);
    std::string rightType = inferType(bin->right);
    std::string op = bin->op;

    LOG(LogLevel::DEBUG, "[inferType] BinaryExpr: op=" + op + ", left=" + leftType + ", right=" + rightType);

    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
      if (leftType == "entero" && rightType == "entero") return "entero";
      if ((leftType == "entero" || leftType == "decimal") &&
          (rightType == "entero" || rightType == "decimal")) return "decimal";
      LOG(LogLevel::ERROR, "[inferType] Operación aritmética inválida con tipos: " + leftType + " y " + rightType);
      ErrorReporter::getInstance().report("Operación aritmética inválida con tipos: " + leftType + " y " + rightType, peek().row, peek().column);
      return "error";
    }

    if (op == "==" || op == "!=") {
      if (leftType == rightType) return "bool";
      LOG(LogLevel::ERROR, "[inferType] Comparación inválida entre tipos distintos: " + leftType + " y " + rightType);
      ErrorReporter::getInstance().report("Comparación inválida entre tipos distintos: " + leftType + " y " + rightType, peek().row, peek().column);
      return "error";
    }

    if (op == "<" || op == ">" || op == "<=" || op == ">=") {
      if ((leftType == "entero" || leftType == "decimal") &&
          (rightType == "entero" || rightType == "decimal")) return "bool";
      LOG(LogLevel::ERROR, "[inferType] Comparación inválida entre tipos no numéricos: " + leftType + " y " + rightType);
      ErrorReporter::getInstance().report("Comparación inválida entre tipos no numéricos: " + leftType + " y " + rightType, peek().row, peek().column);
      return "error";
    }

    if (op == "&&" || op == "||") {
      if (leftType == "bool" && rightType == "bool") return "bool";
      LOG(LogLevel::ERROR, "[inferType] Operador lógico entre tipos no booleanos: " + leftType + " y " + rightType);
      ErrorReporter::getInstance().report("Operador lógico entre tipos no booleanos: " + leftType + " y " + rightType, peek().row, peek().column);
      return "error";
    }

    LOG(LogLevel::ERROR, "[inferType] Operador desconocido o no soportado: " + op);
    ErrorReporter::getInstance().report("Operador desconocido o no soportado: " + op, peek().row, peek().column);
    return "error";
  }

  if (auto un = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
    std::string innerType = inferType(un->right);
    std::string op = un->op;

    if (op == "!") {
      if (innerType == "bool") return "bool";
      LOG(LogLevel::ERROR, "[inferType] Negación lógica '!' sobre tipo no booleano: " + innerType);
      ErrorReporter::getInstance().report("Negación lógica '!' sobre tipo no booleano: " + innerType, peek().row, peek().column);
      return "error";
    }
    if (op == "-") {
      if (innerType == "entero" || innerType == "decimal") return innerType;
      LOG(LogLevel::ERROR, "[inferType] Negación aritmética '-' sobre tipo inválido: " + innerType);
      ErrorReporter::getInstance().report("Negación aritmética '-' sobre tipo inválido: " + innerType, peek().row, peek().column);
      return "error";
    }

    LOG(LogLevel::ERROR, "[inferType] Operador unario no reconocido: " + op);
    ErrorReporter::getInstance().report("Operador unario no reconocido: " + op, peek().row, peek().column);
    return "error";
  }

  LOG(LogLevel::ERROR, "[inferType] Tipo de expresión desconocida");
  ErrorReporter::getInstance().report("Tipo de expresión desconocida", peek().row, peek().column);
  return "error";
}

void Parser::validateVarDeclared(const std::shared_ptr<Expression>& expr) {
    if (!expr) return;

    if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
      if (!symbols.isDeclared(var->name)) {
        std::string msg = "La variable '" + var->name + "' no ha sido declarada previamente.";
        LOG(LogLevel::ERROR, "[validateVarDeclared] " + msg);
        ErrorReporter::getInstance().report(msg, peek().row, peek().column);
        semanticErrorOcurred = true;
      }
    }
    else if (auto bin = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        validateVarDeclared(bin->left);
        validateVarDeclared(bin->right);
    }
    else if (auto un = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
        validateVarDeclared(un->right);
    }
}
