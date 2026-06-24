#pragma once
#include <memory>
#include <string>
#include <string_view>

#include "ASTVisitor.hpp"

// Base interface class
// Represents any node which makes part of the expression
class ExprAST {
 protected:
  int line;
  int column;

 public:
  ExprAST(int l = 0, int c = 0) : line(l), column(c) {}
  virtual ~ExprAST() =
      default;  // Every node from the root will be correctly deleted

  virtual void accept(ASTVisitor& visitor) = 0;

  int getLine() const { return line; }
  int getColumn() const { return column; }
};

// Operators - Tree nodes
class BinaryExprAST : public ExprAST {
 private:
  char op;                       // (+, -, *, /)
  std::unique_ptr<ExprAST> lhs;  // Left hand side
  std::unique_ptr<ExprAST> rhs;  // Right hand side

 public:
  BinaryExprAST(char operation, std::unique_ptr<ExprAST> left,
                std::unique_ptr<ExprAST> right, int l, int c)
      : ExprAST(l, c),
        op(operation),
        lhs(std::move(left)),
        rhs(std::move(right)) {}

  char getOperator() const { return op; }
  ExprAST* getLHS() const { return lhs.get(); }
  ExprAST* getRHS() const { return rhs.get(); }

  void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Factor rules - Tree leafs
// Escalar numbers
class NumberExprAST : public ExprAST {
 private:
  double val;

 public:
  explicit NumberExprAST(double v, int l, int c) : ExprAST(l, c), val(v) {}
  double getVal() const { return val; };

  void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Variables
class VariableExprAST : public ExprAST {
 private:
  std::string name;  // Needs to be the new string owner

 public:
  explicit VariableExprAST(std::string_view n, int l, int c)
      : ExprAST(l, c), name(n) {}
  const std::string& getName() const { return name; };

  void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// Statement : var = expr
class AssignExprAST : public ExprAST {
 private:
  std::string name;  // Variable name
  std::unique_ptr<ExprAST> expr;

 public:
  AssignExprAST(std::string_view varName, std::unique_ptr<ExprAST> expression,
                int l, int c)
      : ExprAST(l, c), name(varName), expr(std::move(expression)) {}

  const std::string& getName() const { return name; }
  ExprAST* getExpr() const { return expr.get(); }

  void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};
