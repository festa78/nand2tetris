// No copyright.

#include <iostream>

#include "gtest/gtest.h"
#include "lib/JackTokenizer.hpp"

namespace {

constexpr char input_file[] = "lib/tests/data/test.jack";

}

TEST(JackTokenizerTest, getCodeLinesFromFileTest) {
  const std::vector<std::string> kGroundTruth = {
      "class Main {",
      "function void main() {",
      "var SquareGame game;",
      "let game = SquareGame.new();",
      "do game.run();",
      "do game.dispose();",
      "return;",
      "}",
      "}",
  };

  auto sut = JackTokenizer(input_file, "dummy.xml");
  const auto lines = sut.getCodeLinesFromFile();

  EXPECT_EQ(kGroundTruth.size(), lines.size());
  for (size_t i = 0; i < kGroundTruth.size(); ++i) {
    EXPECT_EQ(kGroundTruth[i], lines[i]);
  }
}

TEST(JackTokenizerTest, getTokensFromCodeLinesTest) {
  const std::vector<std::string> kGroundTruth = {
      "class", "Main",   "{",          "function",   "void",    "main", "(",
      ")",     "{",      "var",        "SquareGame", "game",    ";",    "let",
      "game",  "=",      "SquareGame", ".",          "new",     "(",    ")",
      ";",     "do",     "game",       ".",          "run",     "(",    ")",
      ";",     "do",     "game",       ".",          "dispose", "(",    ")",
      ";",     "return", ";",          "}",          "}",
  };

  auto sut = JackTokenizer(input_file, "dummy.xml");
  sut.parseInputFile();

  const auto tokens = sut.getTokens();
  EXPECT_EQ(kGroundTruth.size(), tokens.size());
  for (size_t i = 0; i < kGroundTruth.size(); ++i) {
    EXPECT_EQ(kGroundTruth[i], tokens[i]);
  }
}

TEST(JackTokenizerTest, tokenTypeKeywordNamesTest) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  for (const auto& keyword_name : kKeywordNames) {
    sut.setTokens({keyword_name});
    EXPECT_EQ(sut.tokenType(), TokenType::kKeyWord);
  }
}

TEST(JackTokenizerTest, tokenTypeSymbolsTest) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  for (const auto& symbol : kSymbols) {
    sut.setTokens({symbol});
    EXPECT_EQ(sut.tokenType(), TokenType::kSymbol);
  }
}

TEST(JackTokenizerTest, KeyWordTestOk) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  for (size_t i = 0; i < static_cast<int>(KeyWord::kFieldSize); ++i) {
    sut.setTokens({kKeywordNames[i]});
    EXPECT_EQ(sut.keyWord(), static_cast<KeyWord>(i));
  }
}

TEST(JackTokenizerTest, KeyWordTestUnknownKeyRuntimeError) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  sut.setTokens({"unknown key"});
  EXPECT_THROW(sut.keyWord(), std::runtime_error);
}

TEST(JackTokenizerTest, SymbolTestOk) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  for (const auto& symbol : kSymbols) {
    sut.setTokens({symbol});
    EXPECT_EQ(sut.symbol(), symbol);
  }
}

TEST(JackTokenizerTest, SymbolTestUnknownKeyRuntimeError) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  sut.setTokens({"unknown key"});
  EXPECT_THROW(sut.symbol(), std::runtime_error);
}

TEST(JackTokenizerTest, IntTestOk) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  for (int i = 0; i < 32768; ++i) {
    sut.setTokens({std::to_string(i)});
    EXPECT_EQ(sut.intVal(), i);
  }
}

TEST(JackTokenizerTest, IntTestUnknownKeyRuntimeError) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  sut.setTokens({"unknown key"});
  EXPECT_THROW(sut.intVal(), std::runtime_error);
}

TEST(JackTokenizerTest, IntTestOutOfBoundRuntimeError) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  sut.setTokens({"32769"});
  EXPECT_THROW(sut.intVal(), std::runtime_error);
}

TEST(JackTokenizerTest, StringTestOk) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  sut.setTokens({"\"ok string\""});
  EXPECT_EQ(sut.stringVal(), "ok string");
}

TEST(JackTokenizerTest, StringTestUnknownKeyRuntimeError) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  sut.setTokens({"\'ng string\'"});
  EXPECT_THROW(sut.stringVal(), std::runtime_error);
}

TEST(JackTokenizerTest, IdentifierOk) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  sut.setTokens({"ok_name_123"});
  EXPECT_EQ(sut.identifier(), "ok_name_123");
}

TEST(JackTokenizerTest, IdentifierUnknownKeyRuntimeError) {
  auto sut = JackTokenizer(input_file, "dummy.xml");

  sut.setTokens({"ng-name-hyphen"});
  EXPECT_THROW(sut.identifier(), std::runtime_error);
  sut.setTokens({"1ng_name_number"});
  EXPECT_THROW(sut.identifier(), std::runtime_error);
  sut.setTokens({"ng_name_?_symbol"});
  EXPECT_THROW(sut.identifier(), std::runtime_error);
}
