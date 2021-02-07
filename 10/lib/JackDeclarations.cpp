// No copyright.
// Declaration manager.

#include "JackDeclarations.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>

void JackDeclarations::addDeclaration(const std::string& new_declaration) {
  if (isDeclared(new_declaration)) {
    throw std::runtime_error("Cannot declare twice: " + new_declaration);
  }
  declarations_.insert(new_declaration);
}

bool JackDeclarations::isDeclared(const std::string& name) {
  return declarations_.find(name) != declarations_.end();
}

bool JackDeclarations::isEmpty() { return declarations_.empty(); }
void JackDeclarations::clear() { declarations_.clear(); }
