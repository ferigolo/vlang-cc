#include "SemanticAnalyzer.hpp"

std::string typeToString(ValueType type) {
  switch (type) {
    case ValueType::Int:
      return "Int";
    case ValueType::Float:
      return "Float";
    case ValueType::Vec3:
      return "Vec3";
    default:
      return "Unknown";
  }
}

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
  auto it = symTable.find(name);
  if (it == symTable.end()) {
    diagEng.report(
        DiagnosticLevel::Error,
        std::format("Variable '{}' has not been declared", node.getName()),
        node.getLine(), node.getColumn());
    currentType = ValueType::Unknown;
    return;
  }
  currentType = it->second;
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

  currentType = leftType;  // Valid math, we keep the type
}

void SemanticAnalyzer::visit(AssignExprAST& node) {
  node.getExpr()->accept(*this);
  ValueType exprType = currentType;

  if (exprType == ValueType::Unknown) return;

  const std::string& varName = node.getName();
  auto it = symTable.find(varName);

  if (it != symTable.end()) {
    if (it->second != exprType) {
      diagEng.report(
          DiagnosticLevel::Error,
          std::format("Attempy to assign {} to variable '{}' of type {}",
                      typeToString(exprType), varName,
                      typeToString(it->second)),
          node.getLine(), node.getColumn());
    }
  } else {
    symTable[varName] = exprType;
    diagEng.report(
        DiagnosticLevel::Warning,
        std::format(
            "Variable '{}' was declared implicitly with infered type of {}",
            varName, typeToString(exprType)),
        node.getLine(), node.getColumn());
  }
}
