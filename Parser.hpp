#pragma once
#include <memory>
#include <stdexcept>
#include <vector>

#include "ASTNodes.hpp"
#include "Lexer.hpp"

class Parser {
 private:
  Lexer& lexer;
  Token currentToken;

  Token getNextToken() { return currentToken = lexer.getNextToken(); };

  std::unique_ptr<ExprAST> parseNumberExpr(), parseVariableExpr(),
      parseFactor(), parseTerm(), parseExpr(), parseStatement();

 public:
  explicit Parser(Lexer& lex) : lexer(lex) { getNextToken(); }

  // Main entry point
  std::vector<std::unique_ptr<ExprAST>> parse();
};