// No copyright.

#include <iostream>

#include "gtest/gtest.h"
#include "lib/JackTokenizer.hpp"

namespace {

constexpr char input_file[] = "lib/tests/data/test.jack";

}  // namespace

TEST(JackTokenizerTest, ParseTest) {
  const std::vector<std::string> kGroundTruth = {
      "class", "Main",   "{",          "function",   "void",    "main", "(",
      ")",     "{",      "var",        "SquareGame", "game",    ";",    "let",
      "game",  "=",      "SquareGame", ".",          "new",     "(",    ")",
      ";",     "do",     "game",       ".",          "run",     "(",    ")",
      ";",     "do",     "game",       ".",          "dispose", "(",    ")",
      ";",     "return", ";",          "}",          "}",
  };

  JackTokenizer sut(input_file);
  auto tokens = sut.parseInputFile();
  Tokens gt(kGroundTruth);

  while (gt.hasMoreTokens()) {
    EXPECT_EQ(tokens.tokenType(), gt.tokenType());
    tokens.advance();
    gt.advance();
  }
  EXPECT_FALSE(tokens.hasMoreTokens());
}
