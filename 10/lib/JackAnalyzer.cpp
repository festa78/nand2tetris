// No copyright.
// Jack code compiler engine.

#include "JackAnalyzer.hpp"

#include "JackTokenizer.hpp"

JackAnalyzer::JackAnalyzer(const std::string& source,
                           const std::string& output_filename) {
  tokenizer_.reset(new JackTokenizer(source, output_filename));
}
