#include "Parser.hpp"

#include <charconv>

// Statement​      -> Identifier ’=’ Expr ’;'
// Expr           -> Term((’+’∣’-’) Term)∗
// RelationalExpr -> Expr(('==' | '!=' | '<' | etc) Expr) *
// Term           -> Factor((’*’∣’/’) Factor)∗
// Factor         -> Identifier ∣ Number ∣ ’(’ Expr ’)’​

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

std::unique_ptr<ExprAST> Parser::parseBlock() {
  int line = currentToken.line, col = currentToken.column;
  getNextToken();  // Consumes '{'

  std::vector<std::unique_ptr<ExprAST>> statements;

  while (currentToken.type != TokenType::CloseBrace &&
         currentToken.type != TokenType::EndOfFile) {
    if (auto stmt = parseStatement())
      statements.push_back(std::move(stmt));
    else
      getNextToken();
  }

  if (currentToken.type != TokenType::CloseBrace) {
    diagEngine.report(DiagnosticLevel::Error, "Expected '}' at the end block",
                      currentToken.line, currentToken.column);
    return nullptr;
  }
  getNextToken();  // Consumes '}'

  return std::make_unique<BlockExprAST>(std::move(statements), line, col);
}

std::unique_ptr<ExprAST> Parser::parseIfElseStatement() {
  int line = currentToken.line, col = currentToken.column;
  getNextToken();  // Consumes 'if'

  if (currentToken.type != TokenType::OpenParen)
    return reportError("Expected '(' after 'if' statement", line, col);

  getNextToken();  // Consumes '('

  auto condition = parseRelationalExpr();

  if (currentToken.type != TokenType::CloseParen)
    return reportError("Expected ')' before 'if' condition");
  getNextToken();  // Consumes ')'

  if (currentToken.type != TokenType::OpenBrace)
    return reportError("Expected '{' after 'if' statement");

  auto thenBlock = parseBlock();

  std::unique_ptr<ExprAST> elseBlock = nullptr;
  if (currentToken.type == TokenType::KeywordElse) {
    getNextToken();  // Consumes 'else'
    if (currentToken.type != TokenType::OpenBrace &&
        currentToken.type != TokenType::KeywordIf)
      return reportError("Expected '{' or 'if' after else statement");

    if (currentToken.type == TokenType::KeywordIf)
      elseBlock = parseIfElseStatement();
    else if (currentToken.type == TokenType::OpenBrace)
      elseBlock = parseBlock();
    else
      return reportError("Expected '{' or 'if' after 'else' keyword");
  }

  return std::make_unique<IfExprAST>(std::move(condition), std::move(thenBlock),
                                     std::move(elseBlock), line, col);
}

std::unique_ptr<ExprAST> Parser::parseRelationalExpr() {
  auto lhs = parseExpr();
  if (!lhs) return nullptr;

  while (currentToken.type == TokenType::Equals ||
         currentToken.type == TokenType::NotEquals ||
         currentToken.type == TokenType::LessThan ||
         currentToken.type == TokenType::LessEqual ||
         currentToken.type == TokenType::GreaterThan ||
         currentToken.type == TokenType::GreaterEqual) {
    TokenType op = currentToken.type;
    int line = currentToken.line;
    int col = currentToken.column;
    getNextToken();  // Consumes operator

    auto rhs = parseExpr();
    if (!rhs) return nullptr;

    lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs),
                                          line, col);
  }
  return lhs;
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
    TokenType op = currentToken.type;
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
    TokenType op = currentToken.type;
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
  if (currentToken.type == TokenType::KeywordIf) {
    return parseIfElseStatement();
  }
  if (currentToken.type == TokenType::OpenBrace) {
    return parseBlock();
  }

  const int line = currentToken.line, col = currentToken.column;
  if (currentToken.type != TokenType::Identifier)
    return reportError("Variable name expected at the beginning of statement");

  std::string varName = std::string(currentToken.lexeme);
  getNextToken();

  if (currentToken.type != TokenType::Assign)
    return reportError("Expected '=' after variable name");
  getNextToken();  // Consumes '='

  auto expr = parseExpr();
  if (!expr) return nullptr;

  if (currentToken.type != TokenType::Semicolon)
    return reportError("Expected ';' at end of statement.");
  getNextToken();  // Consumes ';'

  return std::make_unique<AssignExprAST>(varName, std::move(expr), line, col);
}

std::vector<std::unique_ptr<ExprAST>> Parser::parse() {
  std::vector<std::unique_ptr<ExprAST>> programAST;
  while (currentToken.type != TokenType::EndOfFile) {
    if (auto statement = parseStatement())
      programAST.push_back(std::move(statement));
    else
      getNextToken();  // Error - we continue to try to recover
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