#include "SemanticAnalyzer.hpp"

void SemanticAnalyzer::analyze(ExprAST* rootNode) {
  if (rootNode)
    rootNode->accept(*this);
  else
    diagEng.report(DiagnosticLevel::Warning,
                   "No root node was provided. Returning.", rootNode->getLine(),
                   rootNode->getColumn());
}

void SemanticAnalyzer::visit(NumberExprAST& node) {
  currentType = ValueType::Float;
}

void SemanticAnalyzer::visit(VariableExprAST& node) {
  const std::string& name = node.getName();
  currentType = symTable.lookupVariable(name, node.getLine(), node.getColumn());
}

void SemanticAnalyzer::visit(BinaryExprAST& node) {
  // Inspects left side
  node.getLHS()->accept(*this);
  ValueType leftType = currentType;

  node.getRHS()->accept(*this);
  ValueType rightType = currentType;

  if (leftType == ValueType::Unknown || rightType == ValueType::Unknown) {
    currentType = ValueType::Unknown;
    return;
  }

  if (leftType != rightType) {
    diagEng.report(
        DiagnosticLevel::Error,
        std::format("Type incompatibility: operation '{}' between {} and {}",
                    node.getOperator(), typeToString(leftType),
                    typeToString(rightType)),
        node.getLine(), node.getColumn());
    diagEng.report(DiagnosticLevel::Note,
                   "Both equation sides should share the same math type",
                   node.getLine(), node.getColumn());

    currentType = ValueType::Unknown;
    return;
  }

  TokenType op = node.getOperator();
  bool isMath = (op == TokenType::Plus || op == TokenType::Minus ||
                 op == TokenType::Asterisk || op == TokenType::Slash);
  bool isRelational =
      (op == TokenType::Equals || op == TokenType::NotEquals ||
       op == TokenType::LessThan || op == TokenType::LessEqual ||
       op == TokenType::GreaterThan || op == TokenType::GreaterEqual);

  if (isMath)
    currentType = leftType;  // Float + Float = Float
  else if (isRelational)
    currentType = ValueType::Bool;  // Float < Float = Bool
  else
    currentType = ValueType::Unknown;
}

void SemanticAnalyzer::visit(AssignExprAST& node) {
  node.getExpr()->accept(*this);
  ValueType exprType = currentType;

  if (exprType == ValueType::Unknown) return;
  symTable.assignVariable(node.getName(), exprType, node.getLine(),
                          node.getColumn());
}
