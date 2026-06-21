#include "Parser.hpp"

#include <charconv>

#include "CompilerError.hpp"

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
  auto result = std::make_unique<NumberExprAST>(val);
  getNextToken();
  return std::move(result);
}

// Variable node
std::unique_ptr<ExprAST> Parser::parseVariableExpr() {
  auto result = std::make_unique<VariableExprAST>(currentToken.lexeme);
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
        throw CompilerError("Sintax error: Expected ')'", currentToken.line,
                            currentToken.column);
      getNextToken();  // Consumes ')'
      return v;
    }
    default:
      throw CompilerError("Sintax error: Unespected factor.", currentToken.line,
                          currentToken.column);
  }
}

// Term -> Factor ( ('*' | '/') Factor )*
std::unique_ptr<ExprAST> Parser::parseTerm() {
  auto lhs = parseFactor();  // Left hand side
  if (!lhs) return nullptr;

  while (currentToken.type == TokenType::Asterisk ||
         currentToken.type == TokenType::Slash) {
    char op = currentToken.type == TokenType::Asterisk ? '*' : '/';
    getNextToken();

    auto rhs = parseFactor();  // Right hand side
    if (!rhs) return nullptr;

    lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
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
    getNextToken();

    auto rhs = parseTerm();  // Right hand side
    if (!rhs) return nullptr;

    lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
  }
  return lhs;
}

// Statement​ -> Identifier ’=’ Expr ’;'
std::unique_ptr<ExprAST> Parser::parseStatement() {
  if (currentToken.type != TokenType::Identifier)
    throw CompilerError(
        "Sintatic error: Variable name espected at the beggining of statement",
        currentToken.line, currentToken.column);

  std::string varName = std::string(currentToken.lexeme);
  getNextToken();

  if (currentToken.type != TokenType::Assign)
    throw CompilerError("Expected '=' after variable name.", currentToken.line,
                        currentToken.column);

  getNextToken();  // Consumes '='

  auto expr = parseExpr();
  if (!expr) return nullptr;

  if (currentToken.type != TokenType::Semicolon)
    throw CompilerError("Expected ';' at end of statement.", currentToken.line,
                        currentToken.column);
  getNextToken();  // Consumes ';'

  return std::make_unique<AssignExprAST>(varName, std::move(expr));
}

std::vector<std::unique_ptr<ExprAST>> Parser::parse() {
  std::vector<std::unique_ptr<ExprAST>> programAST;
  while (currentToken.type != TokenType::EndOfFile) {
    if (auto statement = parseStatement())
      programAST.push_back(std::move(statement));
    else // Error - we continue to try to recover
      getNextToken();
  }

  return programAST;
}

// Statement​ -> Identifier ’=’ Expr ’;'
// Expr      -> Term((’+’∣’-’) Term)∗
// Term      -> Factor((’*’∣’/’) Factor)∗
// Factor    -> Identifier ∣ Number ∣ ’(’ Expr ’)’​