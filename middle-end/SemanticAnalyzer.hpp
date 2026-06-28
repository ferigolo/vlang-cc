#pragma once
#include <unordered_map>

#include "ASTNodes.hpp"
#include "ASTVisitor.hpp"
#include "DiagnosticEngine.hpp"
#include "SymbolTable.hpp"

class SemanticAnalyzer : public ASTVisitor {
 private:
  SymbolTable symTable;
  ValueType currentType = ValueType::Unknown;
  DiagnosticEngine& diagEng;

 public:
  explicit SemanticAnalyzer(DiagnosticEngine& diag)
      : diagEng(diag), symTable(diag) {}

  // Starting point for analyzing a tree
  void analyze(ExprAST* rootNode);
  void visit(NumberExprAST& node) override;
  void visit(VariableExprAST& node) override;
  void visit(BinaryExprAST& node) override;
  void visit(AssignExprAST& node) override;
  void visit(BlockExprAST& node) override;
  void visit(IfExprAST& node) override;
};