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

struct Statement {
  virtual ~Statement() = default;
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


