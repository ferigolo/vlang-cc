#include "SemanticAnalyzer.hpp"

inline constexpr std::string_view tokenToString(TokenType type) {
  switch (type) {
    // Identifiers and literals
    case TokenType::Identifier:
      return "Identifier";
    case TokenType::Number:
      return "Number";

    // Keywords
    case TokenType::KeywordInt:
      return "int";
    case TokenType::KeywordFloat:
      return "float";
    case TokenType::KeywordVec3:
      return "vec3";
    case TokenType::KeywordReturn:
      return "return";
    case TokenType::KeywordIf:
      return "if";
    case TokenType::KeywordElse:
      return "else";

    // Math operators
    case TokenType::Plus:
      return "+";
    case TokenType::Minus:
      return "-";
    case TokenType::Asterisk:
      return "*";
    case TokenType::Slash:
      return "/";

    // Relational and assign operators
    case TokenType::Assign:
      return "=";
    case TokenType::Equals:
      return "==";
    case TokenType::NotEquals:
      return "!=";
    case TokenType::LessThan:
      return "<";
    case TokenType::LessEqual:
      return "<=";
    case TokenType::GreaterThan:
      return ">";
    case TokenType::GreaterEqual:
      return ">=";

    // Grouping and pontuation
    case TokenType::OpenBrace:
      return "{";
    case TokenType::CloseBrace:
      return "}";
    case TokenType::OpenParen:
      return "(";
    case TokenType::CloseParen:
      return ")";
    case TokenType::Semicolon:
      return ";";
    case TokenType::Comma:
      return ",";

    // Special cases
    case TokenType::EndOfFile:
      return "EOF";
    case TokenType::Unknown:
      return "Unknown";

    default:
      return "InvalidToken";
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
                    tokenToString(node.getOperator()), typeToString(leftType),
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

void SemanticAnalyzer::visit(BlockExprAST& node) {
  symTable.enterScope();
  for (const auto& stmt : node.getStatements()) stmt->accept(*this);
  symTable.exitScope();
  currentType = ValueType::Unknown;
}

void SemanticAnalyzer::visit(IfExprAST& node) {
  node.getCondition()->accept(*this);  // Analise condition
  if (currentType != ValueType::Bool && currentType != ValueType::Unknown)
    diagEng.report(DiagnosticLevel::Error,
                   "'if' expression must result in boolean type result",
                   node.getLine(), node.getColumn());

  node.getThenBlock()->accept(*this);  // Analises if block
  if (node.getElseBlock())
    node.getElseBlock()->accept(*this);  // Analises else block
  currentType = ValueType::Unknown;
}