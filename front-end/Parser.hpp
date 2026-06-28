#pragma once
#include <memory>
#include <stdexcept>
#include <vector>

#include "ASTNodes.hpp"
#include "DiagnosticEngine.hpp"
#include "Lexer.hpp"

// Checks the syntax and builds the AST
// Does not handle semantic analysis (next stage), only checks the structure of
// the code
class Parser {
 private:
  Lexer& lexer;
  Token currentToken;
  DiagnosticEngine& diagEngine;

  Token getNextToken() { return currentToken = lexer.getNextToken(); };

  std::unique_ptr<ExprAST> parseNumberExpr(), parseRelationalExpr(),
      parseVariableExpr(), parseFactor(), parseTerm(), parseExpr(),
      parseStatement(), parseBlock(), parseIfElseStatement();

  // Helper to log and propagate nullptr
  std::unique_ptr<ExprAST> reportError(const std::string& message, int line = 0,
                                       int col = 0) {
    diagEngine.report(DiagnosticLevel::Error, message,
                      line ? line : currentToken.line,
                      col ? col : currentToken.column);
    return nullptr;
  };

 public:
  explicit Parser(Lexer& lex, DiagnosticEngine& diagEng)
      : lexer(lex), diagEngine(diagEng) {
    getNextToken();
  }

  // Main entry point
  std::vector<std::unique_ptr<ExprAST>> parse();
};