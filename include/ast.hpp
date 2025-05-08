#pragma once

#include <string>
#include <memory>
#include <vector>

struct Expression
{
  virtual ~Expression() = default;
};

struct NumberExpr : public Expression
{
  std::string value;
  NumberExpr(const std::string& val) : value(val) {}
};

struct VariableExpr : public Expression
{
  std::string name;
  VariableExpr(const std::string& number) : name(number) {}
};

struct StringExpr : public Expression
{
  std::string text;
  StringExpr(const std::string& txt) : text(txt) {}
};

struct BinaryExpr : public Expression
{
  std::string op;
  std::shared_ptr<Expression> left, right;

  BinaryExpr(const std::string& o,
    std::shared_ptr<Expression> l,
    std::shared_ptr<Expression> r
  ) : op(o), left(l), right(r) {}
};

struct UnaryExpr : public Expression {
  std::string op;
  std::shared_ptr<Expression> right;
  UnaryExpr(const std::string& o, std::shared_ptr<Expression> r)
      : op(o), right(std::move(r)) {}
};

struct BooleanExpr : public Expression {
    bool value;

    BooleanExpr(const std::string& val) {
        if (val == "verdadero" || val == "1")
            value = true;
        else
            value = false;
    }
};

struct AssignmentExpr : public Expression {
  std::string name;
  std::shared_ptr<Expression> value;

  AssignmentExpr(const std::string& name, std::shared_ptr<Expression> value)
    : name(name), value(value) {}
};

struct Statement {
  virtual ~Statement() = default;
};

struct IfStatement : public Statement {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> thenBranch;
    std::shared_ptr<Statement> elseBranch;

    IfStatement(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> thenBr, std::shared_ptr<Statement> elseBr)
        : condition(cond), thenBranch(thenBr), elseBranch(elseBr) {}
};

struct WhileStatement : public Statement {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> body;

    WhileStatement(std::shared_ptr<Expression> cond, std::shared_ptr<Statement> body)
        : condition(cond), body(body) {}
};

struct ForStatement : public Statement {
    std::shared_ptr<Statement> init;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> increment;
    std::shared_ptr<Statement> body;

    ForStatement(std::shared_ptr<Statement> init, 
      std::shared_ptr<Expression> cond,
      std::shared_ptr<Statement> incr, 
      std::shared_ptr<Statement> body
    )
    : init(init), condition(cond), increment(incr), body(body) {}
};

struct PrintStatement : public Statement {
    std::shared_ptr<Expression> expr;
    PrintStatement(std::shared_ptr<Expression> expr) : expr(expr) {}
};

struct InputStatement : public Statement {
    std::string variable;
    InputStatement(const std::string& var) : variable(var) {}
};

struct BlockStatement : public Statement {
    std::vector<std::shared_ptr<Statement>> statements;
    BlockStatement(const std::vector<std::shared_ptr<Statement>>& stmts) : statements(stmts) {}
};

struct Declaration : public Statement
{
  std::string type;
  std::string name;
  Declaration(const std::string& t, const std::string& n)
    : type(t), name(n) {}
};

struct Assignment : public Statement
{
  std::string name;
  std::shared_ptr<Expression> expr;
  Assignment(const std::string& n, std::shared_ptr<Expression> e)
    : name(n), expr(e) {}
};

struct ExpressionStatement : public Statement {
    std::shared_ptr<Expression> expression;

    ExpressionStatement(std::shared_ptr<Expression> expr)
      : expression(expr) {}
};
