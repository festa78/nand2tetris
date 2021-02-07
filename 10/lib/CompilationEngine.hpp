// No copyright.
// Jack code compiler engine.

#ifndef LIB_COMPILATIONENGINE_HPP_
#define LIB_COMPILATIONENGINE_HPP_

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "JackDeclarations.hpp"
#include "Tokens.hpp"

// TODO(me): ClassCompilationEngine
class ICompilationEngine {
 public:
  virtual ~ICompilationEngine() = default;
  virtual void compileClass() = 0;
  virtual void compileClassVarDec() = 0;
  virtual void compileSubroutine() = 0;
  virtual void compileParameterList() = 0;
  virtual void compileVarDec() = 0;
  virtual void compileStatements() = 0;
  virtual void compileDo() = 0;
  virtual void compileLet() = 0;
  virtual void compileWhile() = 0;
  virtual void compileReturn() = 0;
  virtual void compileIf() = 0;
  virtual void compileExpression() = 0;
  virtual void compileTerm() = 0;
  virtual void compileExpressionList() = 0;
};

class CompilationEngine : public ICompilationEngine {
 public:
  // TODO(me): use shared_ptr.
  CompilationEngine(
      const std::string& output_filename, std::unique_ptr<ITokens> tokens,
      std::unique_ptr<IJackDeclarations> class_name_decs =
          std::make_unique<JackDeclarations>(),
      std::unique_ptr<IJackDeclarations> class_var_name_decs =
          std::make_unique<JackDeclarations>(),
      std::unique_ptr<IJackDeclarations> subroutine_name_decs =
          std::make_unique<JackDeclarations>(),
      std::unique_ptr<IJackDeclarations> subroutine_var_name_decs =
          std::make_unique<JackDeclarations>());
  ~CompilationEngine() = default;

  void compileClass();
  void compileClassVarDec();
  void compileSubroutine();
  void compileParameterList();
  void compileVarDec();
  void compileStatements();
  void compileDo();
  void compileLet();
  void compileWhile();
  void compileReturn();
  void compileIf();
  void compileExpression();
  void compileTerm();
  void compileExpressionList();

  void compileKeywordConstant();
  void compileUnaryOp();
  void compileOp();
  void compileSubroutineCall();
  void compileStatement();
  void compileType();
  void compileClassVarDecStar();
  void compileSubroutineStar();
  void compileSubroutineBody();

  void writeXMLTokens() const noexcept;

 private:
  bool isTerm();
  bool isExpression();
  bool isKeywordConstant();
  bool isUnaryOp();
  bool isOp();
  bool isSubroutineCall();
  bool isStatement();
  bool isType();
  bool isClassVarDec();
  bool isSubroutine();

  void initializeXML();

  const std::string output_filename_;

  std::shared_ptr<ITokens> tokens_;

  // TODO(me): class_decs_
  std::shared_ptr<IJackDeclarations> class_name_decs_;
  std::shared_ptr<IJackDeclarations> class_var_name_decs_;
  std::shared_ptr<IJackDeclarations> subroutine_name_decs_;
  std::shared_ptr<IJackDeclarations> subroutine_var_name_decs_;

  std::vector<std::string> xml_tokens_;
};

#endif  // LIB_COMPILATIONENGINE_HPP_
