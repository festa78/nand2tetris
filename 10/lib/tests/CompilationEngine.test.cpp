// No copyright.

#include <fstream>
#include <iostream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lib/CompilationEngine.hpp"
#include "lib/JackDeclarations.hpp"
#include "lib/Tokens.hpp"

namespace {

class MockTokens : public ITokens {
 public:
  MOCK_CONST_METHOD0(hasMoreTokens, bool());
  MOCK_METHOD(void, advance, (), (noexcept));
  MOCK_METHOD(TokenType, tokenType, (), (noexcept));
  MOCK_METHOD(KeyWordType, keyWord, (), (noexcept));
  MOCK_METHOD(std::string, symbol, (), (noexcept));
  MOCK_METHOD(int, intVal, (), (noexcept));
  MOCK_METHOD(std::string, stringVal, (), (noexcept));
  MOCK_METHOD(std::string, identifier, (), (noexcept));

  MOCK_METHOD(void, recede, (), (noexcept));
};

class MockJackDeclarations : public IJackDeclarations {
 public:
  MOCK_METHOD(void, addDeclaration, (const std::string&));
  MOCK_METHOD(bool, isDeclared, (const std::string&));
  MOCK_METHOD(bool, isEmpty, ());
  MOCK_METHOD(void, clear, ());
};

constexpr char input_file[] = "lib/tests/data/test.jack";

}  // namespace

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

TEST(CompilationEngineTest, CompileKeywordConstantOk) {
  for (const auto pair : {
           std::make_pair("true", KeyWordType::kTrue),
           std::make_pair("false", KeyWordType::kFalse),
           std::make_pair("null", KeyWordType::kNull),
           std::make_pair("this", KeyWordType::kThis),
       }) {
    auto m = std::make_unique<MockTokens>();
    EXPECT_CALL(*m, keyWord()).WillOnce(Return(pair.second));
    EXPECT_CALL(*m, hasMoreTokens()).WillOnce(Return(false));

    CompilationEngine sut("./dummy.xml", std::move(m),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>());
    sut.compileKeywordConstant();
    sut.writeXMLTokens();

    std::ifstream output("./dummy.xml");
    EXPECT_TRUE(output);

    std::string line;
    std::getline(output, line);
    const auto gt = "<keyword> " + std::string(pair.first) + " </keyword>";
    EXPECT_EQ(line, gt);
    EXPECT_FALSE(std::getline(output, line));
  }
}

TEST(CompilationEngineTest, CompileKeywordConstantThrow) {
  auto m = std::make_unique<MockTokens>();
  EXPECT_CALL(*m, keyWord()).WillOnce(Return(KeyWordType::kBoolean));
  CompilationEngine sut("./dummy.xml", std::move(m),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>());
  EXPECT_THROW(sut.compileKeywordConstant(), std::runtime_error);
}

TEST(CompilationEngineTest, CompileUnaryOpOk) {
  for (const auto unary : {"-", "~"}) {
    auto m = std::make_unique<MockTokens>();
    EXPECT_CALL(*m, tokenType()).WillOnce(Return(TokenType::kSymbol));
    EXPECT_CALL(*m, symbol()).WillRepeatedly(Return(unary));
    EXPECT_CALL(*m, hasMoreTokens()).WillOnce(Return(false));

    CompilationEngine sut("./dummy.xml", std::move(m),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>());
    sut.compileUnaryOp();
    sut.writeXMLTokens();

    std::ifstream output("./dummy.xml");
    EXPECT_TRUE(output);

    std::string line;
    std::getline(output, line);
    const auto gt = "<symbol> " + std::string(unary) + " </symbol>";
    EXPECT_EQ(line, gt);
    EXPECT_FALSE(std::getline(output, line));
  }
}

TEST(CompilationEngineTest, CompileUnaryOpThrow1) {
  auto m = std::make_unique<MockTokens>();
  EXPECT_CALL(*m, tokenType()).WillOnce(Return(TokenType::kSymbol));
  EXPECT_CALL(*m, symbol()).WillRepeatedly(Return("/"));
  CompilationEngine sut("./dummy.xml", std::move(m),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>());
  EXPECT_THROW(sut.compileUnaryOp(), std::runtime_error);
}

TEST(CompilationEngineTest, CompileUnaryOpThrow2) {
  auto m = std::make_unique<MockTokens>();
  EXPECT_CALL(*m, tokenType()).WillOnce(Return(TokenType::kKeyWord));
  CompilationEngine sut("./dummy.xml", std::move(m),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>());
  EXPECT_THROW(sut.compileUnaryOp(), std::runtime_error);
}

TEST(CompilationEngineTest, CompileOpOk) {
  for (const auto op : {"*", "-", "*", "/", "&", "|", "<", ">", "="}) {
    auto m = std::make_unique<MockTokens>();
    EXPECT_CALL(*m, tokenType()).WillOnce(Return(TokenType::kSymbol));
    EXPECT_CALL(*m, symbol()).WillRepeatedly(Return(op));
    EXPECT_CALL(*m, hasMoreTokens()).WillOnce(Return(false));

    CompilationEngine sut("./dummy.xml", std::move(m),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>());
    sut.compileOp();
    sut.writeXMLTokens();

    std::ifstream output("./dummy.xml");
    EXPECT_TRUE(output);

    std::string line;
    std::getline(output, line);
    const auto gt = "<symbol> " + std::string(op) + " </symbol>";
    EXPECT_EQ(line, gt);
    EXPECT_FALSE(std::getline(output, line));
  }
}

TEST(CompilationEngineTest, CompileOpThrow1) {
  auto m = std::make_unique<MockTokens>();
  EXPECT_CALL(*m, tokenType()).WillOnce(Return(TokenType::kSymbol));
  EXPECT_CALL(*m, symbol()).WillRepeatedly(Return("~"));
  CompilationEngine sut("./dummy.xml", std::move(m),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>());
  EXPECT_THROW(sut.compileOp(), std::runtime_error);
}

TEST(CompilationEngineTest, CompileOpThrow2) {
  auto m = std::make_unique<MockTokens>();
  EXPECT_CALL(*m, tokenType()).WillOnce(Return(TokenType::kKeyWord));
  CompilationEngine sut("./dummy.xml", std::move(m),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>());
  EXPECT_THROW(sut.compileOp(), std::runtime_error);
}

TEST(CompilationEngineTest, CompileTermInteger) {
  for (const auto i : {"1", "32767"}) {
    auto m = std::make_unique<MockTokens>();
    EXPECT_CALL(*m, tokenType()).WillRepeatedly(Return(TokenType::kIntConst));
    EXPECT_CALL(*m, intVal()).WillRepeatedly(Return(std::stoi(i)));
    EXPECT_CALL(*m, hasMoreTokens()).WillOnce(Return(false));

    CompilationEngine sut("./dummy.xml", std::move(m),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>(),
                          std::make_unique<MockJackDeclarations>());
    sut.compileTerm();
    sut.writeXMLTokens();

    std::ifstream output("./dummy.xml");
    EXPECT_TRUE(output);

    std::string line;
    std::getline(output, line);
    const auto gt =
        "<integerConstant> " + std::string(i) + " </integerConstant>";
    EXPECT_EQ(line, gt);
    EXPECT_FALSE(std::getline(output, line));
  }
}

TEST(CompilationEngineTest, CompileTermString) {
  auto m = std::make_unique<MockTokens>();
  EXPECT_CALL(*m, tokenType()).WillRepeatedly(Return(TokenType::kStringConst));
  EXPECT_CALL(*m, stringVal()).WillRepeatedly(Return("test string"));
  EXPECT_CALL(*m, hasMoreTokens()).WillOnce(Return(false));

  CompilationEngine sut("./dummy.xml", std::move(m),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>());
  sut.compileTerm();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  const auto gt = "<stringConstant> test string </stringConstant>";
  EXPECT_EQ(line, gt);
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileTermKeyword) {
  auto m = std::make_unique<MockTokens>();
  EXPECT_CALL(*m, tokenType()).WillRepeatedly(Return(TokenType::kKeyWord));
  EXPECT_CALL(*m, keyWord()).WillRepeatedly(Return(KeyWordType::kTrue));
  EXPECT_CALL(*m, hasMoreTokens()).WillOnce(Return(false));

  CompilationEngine sut("./dummy.xml", std::move(m),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>(),
                        std::make_unique<MockJackDeclarations>());
  sut.compileTerm();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  const auto gt = "<keyword> true </keyword>";
  EXPECT_EQ(line, gt);
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileTermVarName) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"variable_name"};
  const std::vector<std::string> kVarNames{"variable_name"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  dummy_declarations->addDeclaration(kTokens[0]);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(dummy_declarations), std::move(m2),
                        std::move(m3));
  sut.compileTerm();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileTermVarNameSquareBracket) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kVarNames{"variable_name"};
  const std::vector<std::string> kTokens{"variable_name", "[", "1", "]"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  for (const auto& var_name : kVarNames) {
    dummy_declarations->addDeclaration(var_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(dummy_declarations), std::move(m2),
                        std::move(m3));
  sut.compileTerm();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> [ </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ] </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileTermSubroutineCall) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kSubroutineNames{"subroutine_name"};
  const std::vector<std::string> kTokens{"subroutine_name", "(", ")", "{", "}"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  for (const auto& subroutine_name : kSubroutineNames) {
    dummy_declarations->addDeclaration(subroutine_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(dummy_declarations),
                        std::move(m3));
  sut.compileTerm();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> subroutine_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileTermCurlyBracket) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"(", "1", ")"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileTerm();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileTermUnaryOp) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"-", "1"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileTerm();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> - </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileExpressionSingleTerm) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"1"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileExpression();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileExpressionTwoTerms) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"2", "-", "1"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileExpression();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 2 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> - </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileExpressionListZero) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileExpressionList();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileExpressionListOne) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"1", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileExpressionList();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileExpressionListTwo) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"1", "+", "2", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileExpressionList();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> + </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 2 </integerConstant>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileSubroutineCall) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kSubroutineNames{"subroutine_name"};
  const std::vector<std::string> kTokens{"subroutine_name", "(", ")"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  for (const auto& subroutine_name : kSubroutineNames) {
    dummy_declarations->addDeclaration(subroutine_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(dummy_declarations),
                        std::move(m3));
  sut.compileSubroutineCall();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> subroutine_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileSubroutineCallMember) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kVariableNames{"variable_name"};
  const std::vector<std::string> kSubroutineNames{"subroutine_name"};
  const std::vector<std::string> kTokens{"variable_name", ".",
                                         "subroutine_name", "(", ")"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  auto dummy_variable_declarations = std::make_unique<JackDeclarations>();
  auto dummy_subroutine_declarations = std::make_unique<JackDeclarations>();
  for (const auto& variable_name : kVariableNames) {
    dummy_variable_declarations->addDeclaration(variable_name);
  }
  for (const auto& subroutine_name : kSubroutineNames) {
    dummy_subroutine_declarations->addDeclaration(subroutine_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(dummy_variable_declarations),
                        std::move(dummy_subroutine_declarations),
                        std::move(m2));
  sut.compileSubroutineCall();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> . </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> subroutine_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileReturnStatement) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"return", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileReturn();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileReturnStatementExpression) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"return", "1", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileReturn();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileDoStatement) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"do", "subroutine_name", "(", ")",
                                         ";"};
  const std::vector<std::string> kSubroutineNames{"subroutine_name"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  for (const auto& subroutine_name : kSubroutineNames) {
    dummy_declarations->addDeclaration(subroutine_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(dummy_declarations),
                        std::move(m3));
  sut.compileDo();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> do </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> subroutine_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileStatementToDo) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"do", "subroutine_name", "(", ")",
                                         ";"};
  const std::vector<std::string> kSubroutineNames{"subroutine_name"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  for (const auto& subroutine_name : kSubroutineNames) {
    dummy_declarations->addDeclaration(subroutine_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(dummy_declarations),
                        std::move(m3));
  sut.compileStatement();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> do </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> subroutine_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileWhileStatement) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"while", "(",      "true", ")",
                                         "{",     "return", ";",    "}"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileWhile();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> while </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> true </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileStatementToWhile) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"while", "(",      "true", ")",
                                         "{",     "return", ";",    "}"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileStatement();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> while </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> true </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileIfStatement) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"if", "(",      "true", ")",
                                         "{",  "return", ";",    "}"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileIf();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> if </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> true </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileStatementToIf) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"if", "(",      "true", ")",
                                         "{",  "return", ";",    "}"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileStatement();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> if </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> true </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileIfElseStatement) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"if",     "(", "true", ")",    "{",
                                         "return", ";", "}",    "else", "{",
                                         "return", ";", "}"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileIf();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> if </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> true </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> else </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileLetStatement) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"let", "variable_name", "=", "1", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  const std::vector<std::string> kVarNames{"variable_name"};
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  for (const auto& variable_name : kVarNames) {
    dummy_declarations->addDeclaration(variable_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(dummy_declarations), std::move(m2),
                        std::move(m3));
  sut.compileLet();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> let </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> = </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileLetBrakectStatement) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{
      "let", "variable_name", "[", "2", "]", "=", "1", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  const std::vector<std::string> kVarNames{"variable_name"};
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  for (const auto& variable_name : kVarNames) {
    dummy_declarations->addDeclaration(variable_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(dummy_declarations), std::move(m2),
                        std::move(m3));
  sut.compileLet();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> let </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> [ </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 2 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ] </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> = </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileStatementToLet) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"let", "variable_name", "=", "1", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  const std::vector<std::string> kVarNames{"variable_name"};
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  for (const auto& variable_name : kVarNames) {
    dummy_declarations->addDeclaration(variable_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(dummy_declarations), std::move(m2),
                        std::move(m3));
  sut.compileStatement();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> let </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> = </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileStatements) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{
      "let", "variable_name", "=", "1", ";", "return", "variable_name", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);
  const std::vector<std::string> kVarNames{"variable_name"};
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  for (const auto& variable_name : kVarNames) {
    dummy_declarations->addDeclaration(variable_name);
  }

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(dummy_declarations), std::move(m2),
                        std::move(m3));
  sut.compileStatements();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> let </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> = </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<integerConstant> 1 </integerConstant>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileStatementsNothing) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{")"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileStatements();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileVarDec) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  EXPECT_CALL(*m4, addDeclaration("variable_name")).Times(1);

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"var", "int", "variable_name", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileVarDec();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> var </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> int </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileVarDecMultiple) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  EXPECT_CALL(*m4, addDeclaration("variable_name_1")).Times(1);
  EXPECT_CALL(*m4, addDeclaration("variable_name_2")).Times(1);

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{
      "var", "int", "variable_name_1", ",", "variable_name_2", ";"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileVarDec();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> var </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> int </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name_1 </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> , </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name_2 </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileSubroutineBodyNoDec) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"{", "return", ";", "}"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileSubroutineBody();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileSubroutineBodyOneDec) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  EXPECT_CALL(*m4, addDeclaration("variable_name")).Times(1);

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"{", "var",    "int", "variable_name",
                                         ";", "return", ";",   "}"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileSubroutineBody();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> var </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> int </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileSubroutineBodyTwoDec) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  EXPECT_CALL(*m4, addDeclaration("variable_name_1")).Times(1);
  EXPECT_CALL(*m4, addDeclaration("variable_name_2")).Times(1);

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{
      "{",      "var", "int", "variable_name_1", ",", "variable_name_2", ";",
      "return", ";",   "}"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileSubroutineBody();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> var </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> int </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name_1 </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> , </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name_2 </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> return </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileParameterListZero) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{")"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileParameterList();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileParameterListOne) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  EXPECT_CALL(*m4, addDeclaration("variable_name")).Times(1);

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"int", "variable_name"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileParameterList();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> int </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name </identifier>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileParameterListTwo) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  EXPECT_CALL(*m4, addDeclaration("variable_name_1")).Times(1);
  EXPECT_CALL(*m4, addDeclaration("variable_name_2")).Times(1);

  // TODO(me): Use mock?
  const std::vector<std::string> kTokens{"int", "variable_name_1", ",", "int",
                                         "variable_name_2"};
  auto dummy_tokens = std::make_unique<Tokens>(kTokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileParameterList();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> int </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name_1 </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> , </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> int </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> variable_name_2 </identifier>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileSubroutineDec) {
  for (const std::string& subroutine_type :
       {"constructor", "function", "method"}) {
    for (const std::string& var_type : {"void", "int"}) {
      auto m1 = std::make_unique<MockJackDeclarations>();
      auto m2 = std::make_unique<MockJackDeclarations>();
      auto m3 = std::make_unique<MockJackDeclarations>();
      auto m4 = std::make_unique<MockJackDeclarations>();
      EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
      EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
      EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
      EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

      EXPECT_CALL(*m3, addDeclaration("subroutine_name")).Times(1);
      EXPECT_CALL(*m4, isEmpty).WillOnce(Return(true));
      EXPECT_CALL(*m4, addDeclaration("variable_name")).Times(1);
      EXPECT_CALL(*m4, clear).Times(1);

      // TODO(me): Use mock?
      const std::vector<std::string> tokens{
          subroutine_type,
          var_type,
          "subroutine_name",
          "(",
          "int",
          "variable_name",
          ")",
          "{",
          "return",
          ";",
          "}",
      };
      auto dummy_tokens = std::make_unique<Tokens>(tokens);

      CompilationEngine sut("./dummy.xml", std::move(dummy_tokens),
                            std::move(m1), std::move(m2), std::move(m3),
                            std::move(m4));
      sut.compileSubroutine();
      sut.writeXMLTokens();

      std::ifstream output("./dummy.xml");
      EXPECT_TRUE(output);

      std::string line;
      std::getline(output, line);
      EXPECT_EQ(line, "<keyword> " + subroutine_type + " </keyword>");
      std::getline(output, line);
      EXPECT_EQ(line, "<keyword> " + var_type + " </keyword>");
      std::getline(output, line);
      EXPECT_EQ(line, "<identifier> subroutine_name </identifier>");
      std::getline(output, line);
      EXPECT_EQ(line, "<symbol> ( </symbol>");
      std::getline(output, line);
      EXPECT_EQ(line, "<keyword> int </keyword>");
      std::getline(output, line);
      EXPECT_EQ(line, "<identifier> variable_name </identifier>");
      std::getline(output, line);
      EXPECT_EQ(line, "<symbol> ) </symbol>");
      std::getline(output, line);
      EXPECT_EQ(line, "<symbol> { </symbol>");
      std::getline(output, line);
      EXPECT_EQ(line, "<keyword> return </keyword>");
      std::getline(output, line);
      EXPECT_EQ(line, "<symbol> ; </symbol>");
      std::getline(output, line);
      EXPECT_EQ(line, "<symbol> } </symbol>");
      EXPECT_FALSE(std::getline(output, line));
    }
  }
}

TEST(CompilationEngineTest, CompileTypePrimitive) {
  for (const std::string& type_name : {"int", "char", "boolean"}) {
    auto m1 = std::make_unique<MockJackDeclarations>();
    auto m2 = std::make_unique<MockJackDeclarations>();
    auto m3 = std::make_unique<MockJackDeclarations>();
    auto m4 = std::make_unique<MockJackDeclarations>();
    EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

    // TODO(me): Use mock?
    const std::vector<std::string> tokens{type_name};
    auto dummy_tokens = std::make_unique<Tokens>(tokens);

    CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                          std::move(m2), std::move(m3), std::move(m4));
    sut.compileType();
    sut.writeXMLTokens();

    std::ifstream output("./dummy.xml");
    EXPECT_TRUE(output);

    std::string line;
    std::getline(output, line);
    EXPECT_EQ(line, "<keyword> " + type_name + " </keyword>");
    EXPECT_FALSE(std::getline(output, line));
  }
}

TEST(CompilationEngineTest, CompileTypeClassName) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));

  // TODO(me): Use mock?
  const std::vector<std::string> tokens{"class_name"};
  auto dummy_tokens = std::make_unique<Tokens>(tokens);
  auto dummy_declarations = std::make_unique<JackDeclarations>();
  dummy_declarations->addDeclaration(tokens[0]);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens),
                        std::move(dummy_declarations), std::move(m1),
                        std::move(m2), std::move(m3));
  sut.compileType();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> class_name </identifier>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileClassVarDec) {
  for (const std::string& class_var_type : {"static", "field"}) {
    auto m1 = std::make_unique<MockJackDeclarations>();
    auto m2 = std::make_unique<MockJackDeclarations>();
    auto m3 = std::make_unique<MockJackDeclarations>();
    auto m4 = std::make_unique<MockJackDeclarations>();
    EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

    EXPECT_CALL(*m2, addDeclaration("variable_name")).Times(1);

    // TODO(me): Use mock?
    const std::vector<std::string> tokens{class_var_type, "int",
                                          "variable_name", ";"};
    auto dummy_tokens = std::make_unique<Tokens>(tokens);

    CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                          std::move(m2), std::move(m3), std::move(m4));
    sut.compileClassVarDec();
    sut.writeXMLTokens();

    std::ifstream output("./dummy.xml");
    EXPECT_TRUE(output);

    std::string line;
    std::getline(output, line);
    EXPECT_EQ(line, "<keyword> " + class_var_type + " </keyword>");
    std::getline(output, line);
    EXPECT_EQ(line, "<keyword> int </keyword>");
    std::getline(output, line);
    EXPECT_EQ(line, "<identifier> variable_name </identifier>");
    std::getline(output, line);
    EXPECT_EQ(line, "<symbol> ; </symbol>");
    EXPECT_FALSE(std::getline(output, line));
  }
}

TEST(CompilationEngineTest, CompileClassVarDecMultiple) {
  for (const std::string& class_var_type : {"static", "field"}) {
    auto m1 = std::make_unique<MockJackDeclarations>();
    auto m2 = std::make_unique<MockJackDeclarations>();
    auto m3 = std::make_unique<MockJackDeclarations>();
    auto m4 = std::make_unique<MockJackDeclarations>();
    EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

    EXPECT_CALL(*m2, addDeclaration("variable_name_1")).Times(1);
    EXPECT_CALL(*m2, addDeclaration("variable_name_2")).Times(1);

    // TODO(me): Use mock?
    const std::vector<std::string> tokens{
        class_var_type, "int", "variable_name_1", ",", "variable_name_2", ";"};
    auto dummy_tokens = std::make_unique<Tokens>(tokens);

    CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                          std::move(m2), std::move(m3), std::move(m4));
    sut.compileClassVarDec();
    sut.writeXMLTokens();

    std::ifstream output("./dummy.xml");
    EXPECT_TRUE(output);

    std::string line;
    std::getline(output, line);
    EXPECT_EQ(line, "<keyword> " + class_var_type + " </keyword>");
    std::getline(output, line);
    EXPECT_EQ(line, "<keyword> int </keyword>");
    std::getline(output, line);
    EXPECT_EQ(line, "<identifier> variable_name_1 </identifier>");
    std::getline(output, line);
    EXPECT_EQ(line, "<symbol> , </symbol>");
    std::getline(output, line);
    EXPECT_EQ(line, "<identifier> variable_name_2 </identifier>");
    std::getline(output, line);
    EXPECT_EQ(line, "<symbol> ; </symbol>");
    EXPECT_FALSE(std::getline(output, line));
  }
}

TEST(CompilationEngineTest, CompileClassEmpty) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  EXPECT_CALL(*m1, addDeclaration("class_name")).Times(1);

  // TODO(me): Use mock?
  const std::vector<std::string> tokens{"class", "class_name", "{", "}"};
  auto dummy_tokens = std::make_unique<Tokens>(tokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileClass();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> class </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> class_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}

TEST(CompilationEngineTest, CompileClassOneEach) {
  auto m1 = std::make_unique<MockJackDeclarations>();
  auto m2 = std::make_unique<MockJackDeclarations>();
  auto m3 = std::make_unique<MockJackDeclarations>();
  auto m4 = std::make_unique<MockJackDeclarations>();
  EXPECT_CALL(*m1, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m2, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m3, isDeclared(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*m4, isDeclared(_)).WillRepeatedly(Return(false));

  EXPECT_CALL(*m1, addDeclaration("class_name")).Times(1);
  EXPECT_CALL(*m2, addDeclaration("class_variable_name")).Times(1);
  EXPECT_CALL(*m3, addDeclaration("subroutine_name")).Times(1);
  EXPECT_CALL(*m4, isEmpty()).WillOnce(Return(true));
  EXPECT_CALL(*m4, clear()).Times(1);

  // TODO(me): Use mock?
  const std::vector<std::string> tokens{
      "class", "class_name",
      "{",     "static",
      "int",   "class_variable_name",
      ";",     "constructor",
      "void",  "subroutine_name",
      "(",     ")",
      "{",     "}",
      "}",
  };
  auto dummy_tokens = std::make_unique<Tokens>(tokens);

  CompilationEngine sut("./dummy.xml", std::move(dummy_tokens), std::move(m1),
                        std::move(m2), std::move(m3), std::move(m4));
  sut.compileClass();
  sut.writeXMLTokens();

  std::ifstream output("./dummy.xml");
  EXPECT_TRUE(output);

  std::string line;
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> class </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> class_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> static </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> int </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> class_variable_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ; </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> constructor </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<keyword> void </keyword>");
  std::getline(output, line);
  EXPECT_EQ(line, "<identifier> subroutine_name </identifier>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ( </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> ) </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> { </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  std::getline(output, line);
  EXPECT_EQ(line, "<symbol> } </symbol>");
  EXPECT_FALSE(std::getline(output, line));
}
