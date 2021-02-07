// No copyright.
// Jack code compiler engine.

#include "JackAnalyzer.hpp"

#include "JackTokenizer.hpp"
#include "Tokens.hpp"

JackAnalyzer::JackAnalyzer(const std::string& source,
                           const std::string& output_filename)
    : source_(source), output_filename_(output_filename) {}

void JackAnalyzer::compileToXML() {
  JackTokenizer jack_tokenizer(source_);
  auto tokens = jack_tokenizer.parseInputFile();
  CompilationEngine compilation_engine(output_filename_,
                                       std::make_unique<Tokens>(tokens));
  compilation_engine.compileClass();
  compilation_engine.writeXMLTokens();
}
