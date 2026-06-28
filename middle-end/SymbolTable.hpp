#pragma once
#include <format>
#include <string>
#include <string_view>
#include <unordered_map>

#include "DiagnosticEngine.hpp"

enum class ValueType { Int, Float, Vec3, Bool, Unknown };

inline constexpr std::string_view typeToString(ValueType type) {
  switch (type) {
    case ValueType::Int:
      return "Int";
    case ValueType::Float:
      return "Float";
    case ValueType::Vec3:
      return "Vec3";
    case ValueType::Bool:
      return "Bool";
    default:
      return "Unknown";
  }
}

class SymbolTable {
 private:
  std::vector<std::unordered_map<std::string, ValueType>> scopes;
  DiagnosticEngine& diagEngine;

 public:
  explicit SymbolTable(DiagnosticEngine& diag) : diagEngine(diag) {
    enterScope();
  }

  void enterScope() { scopes.push_back({}); }

  void exitScope() {
    if (scopes.size() > 1) scopes.pop_back();  // Never deletes the global scope
  }

  ValueType lookupVariable(const std::string& name, int line, int column) {
    //                                             Position before first
    // Points to last element ⮯                  ⮮ element
    for (auto it = scopes.rbegin(); it != scopes.rend(); it++)
      if (it->find(name) != it->end()) return it->at(name);  // Found

    diagEngine.report(DiagnosticLevel::Error,
                      std::format("Variable '{}' has not been declared", name),
                      line, column);
    return ValueType::Unknown;
  }

  void assignVariable(const std::string& name, ValueType exprType, int line,
                      int column) {
    ValueType vt;
    for (auto it = scopes.rbegin(); it != scopes.rend(); it++) {
      auto found = it->find(name);
      
      if (found != it->end()) {  // Found
        if (found->second != exprType) {
          diagEngine.report(
              DiagnosticLevel::Error,
              std::format("Attempt to assign {} to variable '{}' of type {}",
                          typeToString(exprType), name,
                          typeToString(found->second)),
              line, column);
        }
        return;
      }
    }
    // If variable was not found, them it is declared now
    // returns a read/write reference to the data at the last element of the
    // vector ⮯
    scopes.back()[name] =
        exprType;  // Declare variable in last scope (top of the heap)
    diagEngine.report(
        DiagnosticLevel::Warning,
        std::format(
            "Variable '{}' was declared implicitly with infered type of {}",
            name, typeToString(exprType)),
        line, column);
  }
};