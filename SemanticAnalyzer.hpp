#pragma once
#include <unordered_map>

#include "ASTNodes.hpp"
#include "ASTVisitor.hpp"
#include "DiagnosticEngine.hpp"

enum class ValueType { Int, Float, Vec3, Unknown };

class SemanticAnalyzer : public ASTVisitor {
 private:
  std::unordered_map<std::string, ValueType> symTable;
  ValueType currentType = ValueType::Unknown;

  DiagnosticEngine& diagEng;

 public:
  explicit SemanticAnalyzer(DiagnosticEngine& diag) : diagEng(diag) {}

  // Starting point for analyzing a tree
  void analyze(ExprAST* rootNode);

  void visit(NumberExprAST& node) override;
  void visit(VariableExprAST& node) override;
  void visit(BinaryExprAST& node) override;
  void visit(AssignExprAST& node) override;
};