#pragma once
#include <memory>
#include <stdexcept>
#include <vector>

#include "ASTNodes.hpp"
#include "DiagnosticEngine.hpp"
#include "Lexer.hpp"

// Checks the syntax and builds the AST
// Does not handle semantic analysis (next stage), only checks the structure of the code
class Parser {
 private:
  Lexer& lexer;
  Token currentToken;
  DiagnosticEngine& diagEngine;

  Token getNextToken() { return currentToken = lexer.getNextToken(); };

  std::unique_ptr<ExprAST> parseNumberExpr(), parseVariableExpr(),
      parseFactor(), parseTerm(), parseExpr(), parseStatement();

  // Helper to log and propagate nullptr
  std::unique_ptr<ExprAST> reportError(const std::string& message) {
    diagEngine.report(DiagnosticLevel::Error, message, currentToken.line,
                      currentToken.column);
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