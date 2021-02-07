// No copyright.
// Jack tokens.

#include "lib/Tokens.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace {

const std::unordered_map<std::string, KeyWordType> kKeywordNamesToEnum{
    {kKeywordNames[0], KeyWordType::kClass},
    {kKeywordNames[1], KeyWordType::kMethod},
    {kKeywordNames[2], KeyWordType::kFunction},
    {kKeywordNames[3], KeyWordType::kConstructor},
    {kKeywordNames[4], KeyWordType::kInt},
    {kKeywordNames[5], KeyWordType::kBoolean},
    {kKeywordNames[6], KeyWordType::kChar},
    {kKeywordNames[7], KeyWordType::kVoid},
    {kKeywordNames[8], KeyWordType::kVar},
    {kKeywordNames[9], KeyWordType::kStatic},
    {kKeywordNames[10], KeyWordType::kField},
    {kKeywordNames[11], KeyWordType::kLet},
    {kKeywordNames[12], KeyWordType::kDo},
    {kKeywordNames[13], KeyWordType::kIf},
    {kKeywordNames[14], KeyWordType::kElse},
    {kKeywordNames[15], KeyWordType::kWhile},
    {kKeywordNames[16], KeyWordType::kReturn},
    {kKeywordNames[17], KeyWordType::kTrue},
    {kKeywordNames[18], KeyWordType::kFalse},
    {kKeywordNames[19], KeyWordType::kNull},
    {kKeywordNames[20], KeyWordType::kThis},
};

}  // namespace

Tokens::Tokens(const std::vector<std::string> tokens) : tokens_(tokens) {}

bool Tokens::hasMoreTokens() const { return token_index_ + 1 < tokens_.size(); }

void Tokens::advance() {
  if (!hasMoreTokens()) {
    throw std::runtime_error("No more tokens exists.");
  }
  ++token_index_;
}

TokenType Tokens::tokenType() {
  const auto& token = tokens_[token_index_];

  if (std::find(std::cbegin(kKeywordNames), std::cend(kKeywordNames), token) !=
      std::cend(kKeywordNames)) {
    return TokenType::kKeyWord;
  }

  if (std::find(std::cbegin(kSymbols), std::cend(kSymbols), token) !=
      std::cend(kSymbols)) {
    return TokenType::kSymbol;
  }

  if (isIntegerConstant(token)) {
    return TokenType::kIntConst;
  }

  if (token[0] == '\"' && token[token.size() - 1] == '\"') {
    return TokenType::kStringConst;
  }

  if (isValidIdentifier(token)) {
    return TokenType::kIdentifier;
  }

  throw std::runtime_error("Invalied token.");
}

KeyWordType Tokens::keyWord() {
  if (tokenType() != TokenType::kKeyWord) {
    std::cout << int(tokenType()) << std::endl;
    std::cout << identifier() << std::endl;
    throw std::runtime_error("It is not kKeyWord token type");
  }

  return kKeywordNamesToEnum.at(tokens_[token_index_]);
}

std::string Tokens::symbol() {
  if (tokenType() != TokenType::kSymbol) {
    throw std::runtime_error("It is not kSymbol token type");
  }

  return tokens_[token_index_];
}

int Tokens::intVal() {
  if (tokenType() != TokenType::kIntConst) {
    throw std::runtime_error("It is not kIntConst token type");
  }

  return std::stoi(tokens_[token_index_]);
}

std::string Tokens::stringVal() {
  if (tokenType() != TokenType::kStringConst) {
    throw std::runtime_error("It is not kStringConst token type");
  }

  return tokens_[token_index_].substr(1, tokens_[token_index_].size() - 2);
}

std::string Tokens::identifier() {
  if (tokenType() != TokenType::kIdentifier) {
    throw std::runtime_error("It is not kIdentifier token type");
  }

  return tokens_[token_index_];
}

void Tokens::recede() {
  if (token_index_ == 0) {
    throw std::runtime_error("No more tokens exists.");
  }
  --token_index_;
}

bool Tokens::isIntegerConstant(const std::string& token) const {
  if (std::isdigit(token[0])) {
    if (token.find_first_not_of("0123456789") == std::string::npos) {
      const auto token_int = std::stoi(token);
      if (token_int < 0 || token_int > 32768) {
        throw std::runtime_error("Interger out of bound.");
      }

      return true;
    } else {
      throw std::runtime_error("Identifier should not start with integer");
    }
  }

  return false;
}

bool Tokens::isValidIdentifier(const std::string& token) const {
  if (std::isdigit(token[0])) {
    return false;
  }

  for (const auto& c : token) {
    if (std::isalpha(c) || std::isdigit(c) || c == '_') {
      continue;
    }
    return false;
  }

  return true;
}
