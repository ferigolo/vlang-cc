#include <string>

#include "DiagnosticEngine.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticAnalyzer.hpp"

int main() {
  std::string sourceCode = R"(
    posicao = 10.0;
    velocidade = 2.5;
    destino = posicao + velocidade * 5.0;
)";  // Example broken code

  DiagnosticEngine diagEngine;

  // Phase 1 & 2: Lexical and Syntax Analysis
  Lexer lexer(sourceCode, diagEngine);
  Parser parser(lexer, diagEngine);
  auto ast = parser.parse();

  // Check if any phase reported an error
  diagEngine.printDiagnostics();
  if (diagEngine.hasError()) return 1;

  // Phase 3: Semantic Analysis
  SemanticAnalyzer semantic(diagEngine);
  for (auto& node : ast) semantic.analyze(node.get());

  diagEngine.printDiagnostics();
  if (diagEngine.hasError()) {
    return 1;
  }

  std::cout << "Compilation successfull!\n";
  return 0;
}