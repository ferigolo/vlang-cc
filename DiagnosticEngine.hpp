#pragma once
#include <iostream>
#include <string>
#include <vector>

enum class DiagnosticLevel { Note, Warning, Error };

struct Diagnostic {
  DiagnosticLevel level;
  std::string message;
  int line, column;
};

class DiagnosticEngine {
 private:
  std::vector<Diagnostic> diagnostics;
  bool hasErrors = false;

 public:
  // Log an error and track if compilation should fail
  void report(DiagnosticLevel level, const std::string& message, int line,
              int column) {
    diagnostics.push_back({level, message, line, column});
    if (level == DiagnosticLevel::Error) hasErrors = true;
  }
  bool hasError() const { return hasErrors; }
  
  // Print all collected errors beautifully
  void printDiagnostics() const {
    for (const auto& diag : diagnostics) {
      std::string prefix;
      switch (diag.level) {
        case DiagnosticLevel::Error:
          prefix = "\033[31m[ERROR]\033[0m ";
          break;  // Red
        case DiagnosticLevel::Warning:
          prefix = "\033[33m[WARNING]\033[0m ";
          break;  // Yellow
        case DiagnosticLevel::Note:
          prefix = "\033[36m[NOTE]\033[0m ";
          break;  // Cyan
      }
      std::cerr << prefix << "Line " << diag.line << ", Col " << diag.column
                << ": " << diag.message << ".\n";
    }
  }
};