// No copyright.

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lib/Tokens.hpp"

TEST(TokensTest, tokenTypeKeywordNamesTest) {
  Tokens sut(kKeywordNames);
  for (int i = 0; i < kKeywordNames.size() - 1; ++i) {
    EXPECT_EQ(sut.tokenType(), TokenType::kKeyWord);
    sut.advance();
  }
  EXPECT_EQ(sut.tokenType(), TokenType::kKeyWord);
  EXPECT_FALSE(sut.hasMoreTokens());
}

TEST(TokensTest, tokenTypeSymbolsTest) {
  Tokens sut(kSymbols);
  for (int i = 0; i < kSymbols.size() - 1; ++i) {
    EXPECT_EQ(sut.tokenType(), TokenType::kSymbol);
    sut.advance();
  }
  EXPECT_EQ(sut.tokenType(), TokenType::kSymbol);
  EXPECT_FALSE(sut.hasMoreTokens());
}

TEST(TokensTest, KeyWordTestOk) {
  Tokens sut(kKeywordNames);
  for (int i = 0; i < kKeywordNames.size() - 1; ++i) {
    EXPECT_EQ(sut.keyWord(), static_cast<KeyWordType>(i));
    sut.advance();
  }
  EXPECT_EQ(sut.keyWord(), static_cast<KeyWordType>(kKeywordNames.size() - 1));
  EXPECT_FALSE(sut.hasMoreTokens());
}

TEST(JackTokenizerTest, KeyWordTestUnknownKeyRuntimeError) {
  Tokens sut({"unknown key"});
  EXPECT_THROW(sut.keyWord(), std::runtime_error);
}

TEST(TokensTest, SymbolTestOk) {
  Tokens sut(kSymbols);
  for (int i = 0; i < kSymbols.size() - 1; ++i) {
    EXPECT_EQ(sut.symbol(), kSymbols[i]);
    sut.advance();
  }
  EXPECT_EQ(sut.symbol(), kSymbols[kSymbols.size() - 1]);
  EXPECT_FALSE(sut.hasMoreTokens());
}

TEST(JackTokenizerTest, SymbolTestUnknownKeyRuntimeError) {
  Tokens sut({"unknown key"});
  EXPECT_THROW(sut.symbol(), std::runtime_error);
}

TEST(TokensTest, IntTestOk) {
  for (int i = 0; i < 32768; ++i) {
    Tokens sut({std::to_string(i)});
    EXPECT_EQ(sut.intVal(), i);
  }
}

TEST(JackTokenizerTest, IntTestUnknownKeyRuntimeError) {
  Tokens sut({"unknown key"});
  EXPECT_THROW(sut.intVal(), std::runtime_error);
}

TEST(JackTokenizerTest, IntTestOutOfBoundRuntimeError) {
  Tokens sut({"32769"});
  EXPECT_THROW(sut.intVal(), std::runtime_error);
}

TEST(JackTokenizerTest, StringTestOk) {
  Tokens sut({"\"ok string\""});
  EXPECT_EQ(sut.stringVal(), "ok string");
}

TEST(JackTokenizerTest, StringTestUnknownKeyRuntimeError) {
  Tokens sut({"\'ng string\'"});
  EXPECT_THROW(sut.stringVal(), std::runtime_error);
}

TEST(JackTokenizerTest, IdentifierOk) {
  Tokens sut({"ok_name_123"});
  EXPECT_EQ(sut.identifier(), "ok_name_123");
}

TEST(JackTokenizerTest, IdentifierUnknownKeyRuntimeError) {
  {
    Tokens sut({"ng-name-hyphen"});
    EXPECT_THROW(sut.identifier(), std::runtime_error);
  }
  {
    Tokens sut({"1ng_name_number"});
    EXPECT_THROW(sut.identifier(), std::runtime_error);
  }
  {
    Tokens sut({"ng_name_?_symbol"});
    EXPECT_THROW(sut.identifier(), std::runtime_error);
  }
}
