#include <string>

#include "DiagnosticEngine.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

int main() {
  std::string sourceCode = "int x = 5;\ny = ;";  // Example broken code

  DiagnosticEngine diagEngine;

  // Phase 1 & 2: Lexical and Syntax Analysis
  Lexer lexer(sourceCode, diagEngine);
  Parser parser(lexer, diagEngine);

  auto ast = parser.parse();

  // Check if any phase reported an error
  diagEngine.printDiagnostics();
  if (diagEngine.hasError()) {
    return 1;  // Stop compilation
  }

  // Phase 3: Semantic Analysis (e.g., Type checking)
  // SemanticAnalyzer semantic(diagEngine);
  // semantic.analyze(ast);
  // if (diagEngine.hasError()) { ... }

  std::cout << "Compilation successful!\n";
  return 0;
}