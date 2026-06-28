#pragma once

// Avoiding circular imports
class NumberExprAST;
class VariableExprAST;
class BinaryExprAST;
class AssignExprAST;
class BlockExprAST;
class IfExprAST;

// The visitor interface with double dispatch
class ASTVisitor {
 public:
  virtual ~ASTVisitor() = default;

  virtual void visit(NumberExprAST& node) = 0;
  virtual void visit(VariableExprAST& node) = 0;
  virtual void visit(BinaryExprAST& node) = 0;
  virtual void visit(AssignExprAST& node) = 0;
  virtual void visit(BlockExprAST& node) = 0;
  virtual void visit(IfExprAST& node) = 0;
};