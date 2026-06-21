#pragma once
#include <memory>
#include <string>
#include <string_view>

// Base interface class
// Represents any node which makes part of the expression
class ExprAST {
 public:
  virtual ~ExprAST() =
      default;  // Every node from the root will be correctly deleted
};

// Operators - Tree nodes
class BinaryExprAST : public ExprAST {
 private:
  char op;                       // (+, -, *, /)
  std::unique_ptr<ExprAST> lhs;  // Left hand side
  std::unique_ptr<ExprAST> rhs;  // Right hand side

 public:
  BinaryExprAST(char operation, std::unique_ptr<ExprAST> left,
                std::unique_ptr<ExprAST> right)
      : op(operation), lhs(std::move(left)), rhs(std::move(right)) {}

  char getOperator() const { return op; }
  ExprAST* getLHS() const { return lhs.get(); }
  ExprAST* getRHS() const { return rhs.get(); }
};

// Factor rules - Tree leafs
// Escalar numbers
class NumberExprAST : public ExprAST {
 private:
  double val;

 public:
  explicit NumberExprAST(double v) : val(v) {}
  double getVal() const { return val; };
};

// Variables
class VariableExprAST : public ExprAST {
 private:
  std::string name;  // Needs to be the new string owner

 public:
  explicit VariableExprAST(std::string_view n) : name(n) {}
  const std::string& getName() const { return name; };
};

// Statement : var = expr
class AssignExprAST : public ExprAST {
 private:
  std::string name;  // Variable name
  std::unique_ptr<ExprAST> expr;

 public:
  AssignExprAST(std::string_view varName, std::unique_ptr<ExprAST> expression)
      : name(varName), expr(std::move(expression)) {}

  const std::string& getName() const { return name; }
  ExprAST* getExpr() const { return expr.get(); }
};
