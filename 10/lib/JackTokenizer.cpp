// No copyright.
// Jack code tokenizer.

#include "JackTokenizer.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace {

const std::unordered_map<std::string, KeyWord> kKeywordNamesToEnum{
    {kKeywordNames[0], KeyWord::kClass},
    {kKeywordNames[1], KeyWord::kMethod},
    {kKeywordNames[2], KeyWord::kFunction},
    {kKeywordNames[3], KeyWord::kConstructor},
    {kKeywordNames[4], KeyWord::kInt},
    {kKeywordNames[5], KeyWord::kBoolean},
    {kKeywordNames[6], KeyWord::kChar},
    {kKeywordNames[7], KeyWord::kVoid},
    {kKeywordNames[8], KeyWord::kVar},
    {kKeywordNames[9], KeyWord::kStatic},
    {kKeywordNames[10], KeyWord::kField},
    {kKeywordNames[11], KeyWord::kLet},
    {kKeywordNames[12], KeyWord::kDo},
    {kKeywordNames[13], KeyWord::kIf},
    {kKeywordNames[14], KeyWord::kElse},
    {kKeywordNames[15], KeyWord::kWhile},
    {kKeywordNames[16], KeyWord::kReturn},
    {kKeywordNames[17], KeyWord::kTrue},
    {kKeywordNames[18], KeyWord::kFalse},
    {kKeywordNames[19], KeyWord::kNull},
    {kKeywordNames[20], KeyWord::kThis},
};

}  // namespace

JackTokenizer::JackTokenizer(const std::string& input_filename,
                             const std::string& output_filename)
    : input_filename_(input_filename), output_filename_(output_filename) {}

void JackTokenizer::parseInputFile() noexcept {
  tokens_ = getTokensFromCodeLines(getCodeLinesFromFile());
}

bool JackTokenizer::hasMoreTokens() const noexcept {
  return token_index_ + 1 < tokens_.size();
}

void JackTokenizer::advance() {
  if (!hasMoreTokens()) {
    throw std::runtime_error("No more tokens exists.");
  }
  ++token_index_;
}

TokenType JackTokenizer::tokenType() {
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

KeyWord JackTokenizer::keyWord() {
  if (tokenType() != TokenType::kKeyWord) {
    throw std::runtime_error("It is not kKeyWord token type");
  }

  return kKeywordNamesToEnum.at(tokens_[token_index_]);
}

std::string JackTokenizer::symbol() {
  if (tokenType() != TokenType::kSymbol) {
    throw std::runtime_error("It is not kSymbol token type");
  }

  return tokens_[token_index_];
}

int JackTokenizer::intVal() {
  if (tokenType() != TokenType::kIntConst) {
    throw std::runtime_error("It is not kIntConst token type");
  }

  return std::stoi(tokens_[token_index_]);
}

std::string JackTokenizer::stringVal() {
  if (tokenType() != TokenType::kStringConst) {
    throw std::runtime_error("It is not kStringConst token type");
  }

  return tokens_[token_index_].substr(1, tokens_[token_index_].size() - 2);
}

std::string JackTokenizer::identifier() {
  if (tokenType() != TokenType::kIdentifier) {
    throw std::runtime_error("It is not kIdentifier token type");
  }

  return tokens_[token_index_];
}

std::vector<std::string> JackTokenizer::getTokens() const noexcept {
  return tokens_;
}

void JackTokenizer::setTokens(const std::vector<std::string>& tokens) noexcept {
  tokens_.clear();
  tokens_.insert(std::cend(tokens_), std::cbegin(tokens), std::cend(tokens));
}

bool JackTokenizer::isIntegerConstant(const std::string& token) const {
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

bool JackTokenizer::isValidIdentifier(const std::string& token) const {
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

std::vector<std::string> JackTokenizer::getCodeLinesFromFile() {
  std::ifstream input_stream(input_filename_);
  if (!input_stream) {
    throw std::runtime_error("Fail to read input file");
  }
  std::cout << "Read input file: " << input_filename_ << std::endl;

  std::vector<std::string> code_lines;
  std::string line;
  while (std::getline(input_stream, line)) {
    const auto jack_code = extractJackCode(line);
    if (jack_code.empty()) {
      continue;
    }

    code_lines.emplace_back(jack_code);
  }

  return code_lines;
}

std::string JackTokenizer::extractJackCode(const std::string& line) const {
  auto result_line = line;

  const std::string comment_prefix_start = "/*";
  const std::string comment_prefix_end = "*/";
  const auto index_start = result_line.find(comment_prefix_start);
  if (index_start != std::string::npos) {
    const auto index_end =
        result_line.find(comment_prefix_end, index_start + 2);
    if (index_end != std::string::npos) {
      throw std::runtime_error("May be multiline comment, not supported.");
    }
    result_line = result_line.substr(index_end + 2, result_line.size());
  }

  const std::string comment_prefix_two_slash = "//";
  const auto index = result_line.find(comment_prefix_two_slash);
  if (index != std::string::npos) {
    result_line = result_line.substr(0, index);
  }

  result_line = trimSpaceFromLine(result_line);

  return result_line;
}

std::string JackTokenizer::trimSpaceFromLine(
    const std::string& line) const noexcept {
  const auto left_index = line.find_first_not_of(" ");
  if (left_index == std::string::npos) {
    return "";
  }

  const auto right_index = line.find_last_not_of(" ");

  return line.substr(left_index, right_index + 1);
}

std::vector<std::string> JackTokenizer::getTokensFromCodeLines(
    std::vector<std::string> code_lines) {
  std::vector<std::string> tokens;

  for (const auto& code_line : code_lines) {
    const auto tokens_in_code_line = getTokensFromCodeLine(code_line);
    for (const auto& token : tokens_in_code_line) {
      tokens.emplace_back(token);
    }
  }

  return tokens;
}

std::vector<std::string> JackTokenizer::getTokensFromCodeLine(
    const std::string& code_line) {
  std::vector<std::string> tokens;

  const auto space_separated_terms = separateCodeLineBySpace(code_line);
  for (const auto& space_separated_term : space_separated_terms) {
    const auto symbol_indices = findSymbolIndices(space_separated_term);

    size_t start_index = 0;
    for (const auto& symbol_index : symbol_indices) {
      const auto token_before_symbol =
          space_separated_term.substr(start_index, symbol_index - start_index);

      // Can be empty when a symbol exists on index = 0.
      if (!token_before_symbol.empty()) {
        tokens.emplace_back(token_before_symbol);
      }

      const auto symbol = space_separated_term.substr(symbol_index, 1);
      tokens.emplace_back(symbol);

      start_index = symbol_index + 1;
    }

    if (start_index < space_separated_term.size()) {
      tokens.emplace_back(space_separated_term.substr(
          start_index, space_separated_term.size() - start_index));
    }
  }

  return tokens;
}

std::vector<std::string> JackTokenizer::separateCodeLineBySpace(
    const std::string& code_line) {
  std::vector<std::string> space_separated_terms;

  size_t start_index = 0;
  auto next_index = code_line.find(' ');
  while (next_index != std::string::npos) {
    space_separated_terms.emplace_back(
        code_line.substr(start_index, next_index - start_index));

    start_index = next_index + 1;
    next_index = code_line.find(' ', start_index);
  }

  if (start_index < code_line.size()) {
    space_separated_terms.emplace_back(
        code_line.substr(start_index, code_line.size()));
  }

  return space_separated_terms;
}

std::vector<std::size_t> JackTokenizer::findSymbolIndices(
    const std::string& line) const noexcept {
  std::vector<std::size_t> indices;
  for (const auto& symbol : kSymbols) {
    auto index = line.find(symbol);
    while (index != std::string::npos) {
      indices.emplace_back(static_cast<size_t>(index));

      index = line.find(symbol, index + 1);
    }
  }

  std::sort(std::begin(indices), std::end(indices));

  return indices;
}
