#pragma once
#include <unordered_map>

#include "ASTNodes.hpp"
#include "ASTVisitor.hpp"
#include "DiagnosticEngine.hpp"
#include "SymbolTable.hpp"

enum class ValueType { Int, Float, Vec3, Unknown };

class SemanticAnalyzer : public ASTVisitor {
 private:
  SymbolTable symTable;
  ValueType currentType = ValueType::Unknown;
  DiagnosticEngine& diagEng;

 public:
  explicit SemanticAnalyzer(DiagnosticEngine& diag)
      : diagEng(diag), symTable(diagEng) {}

  // Starting point for analyzing a tree
  void analyze(ExprAST* rootNode);
  void visit(NumberExprAST& node) override;
  void visit(VariableExprAST& node) override;
  void visit(BinaryExprAST& node) override;
  void visit(AssignExprAST& node) override;
};