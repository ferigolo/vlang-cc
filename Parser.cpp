#include "Parser.hpp"

#include <charconv>

// Statement​ -> Identifier ’=’ Expr ’;'
// Expr      -> Term((’+’∣’-’) Term)∗
// Term      -> Factor((’*’∣’/’) Factor)∗
// Factor    -> Identifier ∣ Number ∣ ’(’ Expr ’)’​

// Number node
std::unique_ptr<ExprAST> Parser::parseNumberExpr() {
  double val = 0;
  // Safest and fastest way to convert from string to numbers
  std::from_chars(currentToken.lexeme.data(),
                  currentToken.lexeme.data() + currentToken.lexeme.size(), val);
  // Create new node
  auto result = std::make_unique<NumberExprAST>(val, currentToken.line,
                                                currentToken.column);
  getNextToken();
  return std::move(result);
}

// Variable node
std::unique_ptr<ExprAST> Parser::parseVariableExpr() {
  auto result = std::make_unique<VariableExprAST>(
      currentToken.lexeme, currentToken.line, currentToken.column);
  getNextToken();
  return std::move(result);
}

// Factor -> Identifier | Number | '(' Expr ')'
std::unique_ptr<ExprAST> Parser::parseFactor() {
  switch (currentToken.type) {
    case TokenType::Identifier:
      return parseVariableExpr();
    case TokenType::Number:
      return parseNumberExpr();
    case TokenType::OpenParen: {
      getNextToken();  // Consumes '('
      auto v = parseExpr();
      if (currentToken.type != TokenType::CloseParen)
        return reportError("Expected ')'.");
      getNextToken();  // Consumes ')'
      return v;
    }
    default:
      return reportError("Unexpected factor");
  }
}

// Term -> Factor ( ('*' | '/') Factor )*
std::unique_ptr<ExprAST> Parser::parseTerm() {
  auto lhs = parseFactor();  // Left hand side
  if (!lhs) return nullptr;

  while (currentToken.type == TokenType::Asterisk ||
         currentToken.type == TokenType::Slash) {
    char op = currentToken.type == TokenType::Asterisk ? '*' : '/';
    int l = currentToken.line, c = currentToken.column;
    getNextToken();

    auto rhs = parseFactor();  // Right hand side
    if (!rhs) return nullptr;

    lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs), l,
                                          c);
  }

  return lhs;
}

// Expr -> Term ( ('+' | '-') Term )*
std::unique_ptr<ExprAST> Parser::parseExpr() {
  auto lhs = parseTerm();
  if (!lhs) return nullptr;
  while (currentToken.type == TokenType::Plus ||
         currentToken.type == TokenType::Minus) {
    char op = currentToken.type == TokenType::Plus ? '+' : '-';
    int l = currentToken.line, c = currentToken.column;
    getNextToken();

    auto rhs = parseTerm();  // Right hand side
    if (!rhs) return nullptr;

    lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs), l,
                                          c);
  }
  return lhs;
}

// Statement​ -> Identifier ’=’ Expr ’;'
std::unique_ptr<ExprAST> Parser::parseStatement() {
  if (currentToken.type != TokenType::Identifier)
    return reportError("Variable name expected at the beginning of statement");

  std::string varName = std::string(currentToken.lexeme);
  getNextToken();

  if (currentToken.type != TokenType::Assign)
    return reportError("Expected '=' after variable name.");
  getNextToken();  // Consumes '='

  auto expr = parseExpr();
  if (!expr) return nullptr;

  if (currentToken.type != TokenType::Semicolon)
    return reportError("Expected ';' at end of statement.");
  getNextToken();  // Consumes ';'

  return std::make_unique<AssignExprAST>(varName, std::move(expr));
}

std::vector<std::unique_ptr<ExprAST>> Parser::parse() {
  std::vector<std::unique_ptr<ExprAST>> programAST;
  while (currentToken.type != TokenType::EndOfFile) {
    if (auto statement = parseStatement())
      programAST.push_back(std::move(statement));
    else  // Error - we continue to try to recover
      getNextToken();
  }
  diagEngine.report(DiagnosticLevel::Note,
                    std::format("Successfully build AST with {} statements",
                                programAST.size()),
                    currentToken.line, currentToken.column);
  return programAST;
}

// Statement​ -> Identifier ’=’ Expr ’;'
// Expr      -> Term((’+’∣’-’) Term)∗
// Term      -> Factor((’*’∣’/’) Factor)∗
// Factor    -> Identifier ∣ Number ∣ ’(’ Expr ’)’​