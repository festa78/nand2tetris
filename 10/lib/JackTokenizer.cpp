// No copyright.
// Jack code tokenizer.

#include "JackTokenizer.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <utility>

JackTokenizer::JackTokenizer(const std::string& input_filename)
    : input_filename_(input_filename) {}

Tokens JackTokenizer::parseInputFile() noexcept {
  return Tokens(getTokensFromCodeLines(getCodeLinesFromFile()));
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
    if (index_end == std::string::npos) {
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
