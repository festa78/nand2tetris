// No copyright.
// Jack code compiler engine.

#include "CompilationEngine.hpp"

#include <fstream>
#include <stdexcept>
#include <utility>

CompilationEngine::CompilationEngine(
    const std::string& output_filename, std::unique_ptr<ITokens> tokens,
    std::unique_ptr<IJackDeclarations> class_name_decs,
    std::unique_ptr<IJackDeclarations> class_var_name_decs,
    std::unique_ptr<IJackDeclarations> subroutine_name_decs,
    std::unique_ptr<IJackDeclarations> subroutine_var_name_decs)
    : output_filename_(output_filename),
      tokens_(std::move(tokens)),
      class_name_decs_(std::move(class_name_decs)),
      class_var_name_decs_(std::move(class_var_name_decs)),
      subroutine_name_decs_(std::move(subroutine_name_decs)),
      subroutine_var_name_decs_(std::move(subroutine_var_name_decs)) {}

void CompilationEngine::compileClass() {
  if (tokens_->keyWord() != KeyWordType::kClass) {
    throw std::runtime_error("should be class");
  }
  xml_tokens_.emplace_back("<keyword> class </keyword>");
  tokens_->advance();

  // className
  xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                           " </identifier>");
  class_name_decs_->addDeclaration(tokens_->identifier());
  tokens_->advance();

  // '{'.
  if (tokens_->symbol() != "{") {
    throw std::runtime_error("should be {");
  }
  xml_tokens_.emplace_back("<symbol> { </symbol>");
  tokens_->advance();

  // classVarDec*.
  compileClassVarDecStar();

  // subroutineDec*.
  compileSubroutineStar();

  if (tokens_->symbol() != "}") {
    throw std::runtime_error("should be }");
  }
  xml_tokens_.emplace_back("<symbol> } </symbol>");

  if (tokens_->hasMoreTokens()) {
    throw std::runtime_error("Code still exists after class");
  }
}

void CompilationEngine::compileClassVarDec() {
  if (!isClassVarDec()) {
    throw std::runtime_error("Not a classvardec");
  }

  // static field
  if (tokens_->keyWord() == KeyWordType::kStatic) {
    xml_tokens_.emplace_back("<keyword> static </keyword>");
  } else {
    xml_tokens_.emplace_back("<keyword> field </keyword>");
  }
  tokens_->advance();

  // type.
  compileType();

  // varName.
  xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                           " </identifier>");
  class_var_name_decs_->addDeclaration(tokens_->identifier());
  tokens_->advance();

  // (',' varName)*.
  while (tokens_->tokenType() == TokenType::kSymbol &&
         tokens_->symbol() == ",") {
    xml_tokens_.emplace_back("<symbol> , </symbol>");
    tokens_->advance();

    // varName.
    xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                             " </identifier>");
    class_var_name_decs_->addDeclaration(tokens_->identifier());
    tokens_->advance();
  }

  // ';'.
  if (tokens_->symbol() != ";") {
    throw std::runtime_error("should be ;");
  }
  xml_tokens_.emplace_back("<symbol> " + tokens_->symbol() + " </symbol>");

  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::compileSubroutine() {
  if (!isSubroutine()) {
    throw std::runtime_error("Not a subroutine");
  }

  // constructor function method.
  if (tokens_->keyWord() == KeyWordType::kConstructor) {
    xml_tokens_.emplace_back("<keyword> constructor </keyword>");
  } else if (tokens_->keyWord() == KeyWordType::kFunction) {
    xml_tokens_.emplace_back("<keyword> function </keyword>");
  } else {
    xml_tokens_.emplace_back("<keyword> method </keyword>");
  }
  tokens_->advance();

  // void type.
  if (tokens_->tokenType() == TokenType::kKeyWord &&
      tokens_->keyWord() == KeyWordType::kVoid) {
    xml_tokens_.emplace_back("<keyword> void </keyword>");
    tokens_->advance();
  } else {
    compileType();
  }

  // subroutineName.
  xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                           " </identifier>");
  subroutine_name_decs_->addDeclaration(tokens_->identifier());
  tokens_->advance();

  // Starts new subroutine scope.
  if (!subroutine_var_name_decs_->isEmpty()) {
    throw std::runtime_error(
        "Subroutine variable declaratons already exists when it declares a new "
        "subroutine. Illegal");
  }

  // '('.
  if (tokens_->symbol() != "(") {
    throw std::runtime_error("should be (");
  }
  xml_tokens_.emplace_back("<symbol> ( </symbol>");
  tokens_->advance();

  // parameterList
  compileParameterList();

  // ')'.
  if (tokens_->symbol() != ")") {
    throw std::runtime_error("should be )");
  }
  xml_tokens_.emplace_back("<symbol> ) </symbol>");
  tokens_->advance();

  compileSubroutineBody();

  // End of subroutine scope.
  subroutine_var_name_decs_->clear();
}

void CompilationEngine::compileParameterList() {
  // "?": 0 or 1 time operation.
  // Check if it start with type.
  if (!isType()) {
    return;
  }

  // type.
  compileType();

  // varName.
  xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                           " </identifier>");
  subroutine_var_name_decs_->addDeclaration(tokens_->identifier());

  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }

  // (',' type varName)*.
  while (tokens_->tokenType() == TokenType::kSymbol &&
         tokens_->symbol() == ",") {
    xml_tokens_.emplace_back("<symbol> , </symbol>");
    tokens_->advance();

    // type.
    compileType();

    // varName.
    xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                             " </identifier>");
    subroutine_var_name_decs_->addDeclaration(tokens_->identifier());

    if (tokens_->hasMoreTokens()) {
      tokens_->advance();
    }
  }
}

void CompilationEngine::compileVarDec() {
  // var
  if (tokens_->keyWord() != KeyWordType::kVar) {
    throw std::runtime_error("should be var");
  }
  xml_tokens_.emplace_back("<keyword> var </keyword>");
  tokens_->advance();

  // type.
  compileType();

  // varName.
  xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                           " </identifier>");
  subroutine_var_name_decs_->addDeclaration(tokens_->identifier());

  if (!tokens_->hasMoreTokens()) {
    return;
  }
  tokens_->advance();

  // (',' varName)*.
  while (tokens_->tokenType() == TokenType::kSymbol &&
         tokens_->symbol() == ",") {
    xml_tokens_.emplace_back("<symbol> , </symbol>");
    tokens_->advance();

    // varName.
    xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                             " </identifier>");
    subroutine_var_name_decs_->addDeclaration(tokens_->identifier());

    tokens_->advance();
  }

  // ';'.
  if (tokens_->symbol() != ";") {
    throw std::runtime_error("should be ;");
  }
  xml_tokens_.emplace_back("<symbol> ; </symbol>");

  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::compileStatements() {
  // "*": 0 or more.
  while (isStatement()) {
    compileStatement();
  }
}

void CompilationEngine::compileLet() {
  if (tokens_->keyWord() != KeyWordType::kLet) {
    throw std::runtime_error("Illegal keyword");
  }

  // let.
  xml_tokens_.emplace_back("<keyword> let </keyword>");
  tokens_->advance();

  // varName
  if (!subroutine_var_name_decs_->isDeclared(tokens_->identifier()) &&
      !class_var_name_decs_->isDeclared(tokens_->identifier())) {
    throw std::runtime_error("undefined varName");
  }
  xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                           " </identifier>");
  tokens_->advance();

  // ('[' expression ']')?.
  if (tokens_->tokenType() == TokenType::kSymbol && tokens_->symbol() == "[") {
    // '['.
    xml_tokens_.emplace_back("<symbol> [ </symbol>");
    tokens_->advance();

    // expression.
    compileExpression();

    // ']'.
    if (tokens_->symbol() != "]") {
      throw std::runtime_error("Should be ]");
    }
    xml_tokens_.emplace_back("<symbol> ] </symbol>");
    tokens_->advance();
  }

  // '='.
  if (tokens_->symbol() != "=") {
    throw std::runtime_error("should be =");
  }
  xml_tokens_.emplace_back("<symbol> = </symbol>");
  tokens_->advance();

  // expression.
  compileExpression();

  // ';'.
  if (tokens_->symbol() != ";") {
    throw std::runtime_error("should be ;");
  }
  xml_tokens_.emplace_back("<symbol> ; </symbol>");

  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::compileIf() {
  if (tokens_->keyWord() != KeyWordType::kIf) {
    throw std::runtime_error("Illegal keyword");
  }

  // if.
  xml_tokens_.emplace_back("<keyword> if </keyword>");
  tokens_->advance();

  // '('.
  if (tokens_->symbol() != "(") {
    throw std::runtime_error("should be (");
  }
  xml_tokens_.emplace_back("<symbol> ( </symbol>");
  tokens_->advance();

  // expression.
  compileExpression();

  // ')'.
  if (tokens_->symbol() != ")") {
    throw std::runtime_error("should be )");
  }
  xml_tokens_.emplace_back("<symbol> ) </symbol>");
  tokens_->advance();

  // '{'.
  if (tokens_->symbol() != "{") {
    throw std::runtime_error("should be {");
  }
  xml_tokens_.emplace_back("<symbol> { </symbol>");
  tokens_->advance();

  // statements.
  compileStatements();

  // '}'.
  if (tokens_->symbol() != "}") {
    throw std::runtime_error("should be }");
  }
  xml_tokens_.emplace_back("<symbol> } </symbol>");
  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }

  // ('else' '{' statements '}')?
  if (tokens_->tokenType() == TokenType::kKeyWord &&
      tokens_->keyWord() == KeyWordType::kElse) {
    xml_tokens_.emplace_back("<keyword> else </keyword>");
    tokens_->advance();

    // '{'.
    if (tokens_->symbol() != "{") {
      throw std::runtime_error("should be {");
    }
    xml_tokens_.emplace_back("<symbol> { </symbol>");
    tokens_->advance();

    // statements.
    compileStatements();

    // '}'.
    if (tokens_->symbol() != "}") {
      throw std::runtime_error("should be }");
    }
    xml_tokens_.emplace_back("<symbol> } </symbol>");
    if (tokens_->hasMoreTokens()) {
      tokens_->advance();
    }
  }
}

void CompilationEngine::compileWhile() {
  if (tokens_->keyWord() != KeyWordType::kWhile) {
    throw std::runtime_error("Illegal keyword");
  }

  // while.
  xml_tokens_.emplace_back("<keyword> while </keyword>");
  tokens_->advance();

  // '('.
  if (tokens_->symbol() != "(") {
    throw std::runtime_error("should be (");
  }
  xml_tokens_.emplace_back("<symbol> ( </symbol>");
  tokens_->advance();

  // expression.
  compileExpression();

  // ')'.
  if (tokens_->symbol() != ")") {
    throw std::runtime_error("should be )");
  }
  xml_tokens_.emplace_back("<symbol> ) </symbol>");
  tokens_->advance();

  // '{'.
  if (tokens_->symbol() != "{") {
    throw std::runtime_error("should be {");
  }
  xml_tokens_.emplace_back("<symbol> { </symbol>");
  tokens_->advance();

  // statements.
  compileStatements();

  // '}'.
  if (tokens_->symbol() != "}") {
    throw std::runtime_error("should be }");
  }
  xml_tokens_.emplace_back("<symbol> } </symbol>");
  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::compileDo() {
  if (tokens_->keyWord() != KeyWordType::kDo) {
    throw std::runtime_error("Illegal keyword");
  }

  // do.
  xml_tokens_.emplace_back("<keyword> do </keyword>");
  tokens_->advance();

  // subroutineCall.
  compileSubroutineCall();

  // ';'.
  if (tokens_->symbol() != ";") {
    throw std::runtime_error("should be ;");
  }
  xml_tokens_.emplace_back("<symbol> ; </symbol>");
  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::compileReturn() {
  if (tokens_->keyWord() != KeyWordType::kReturn) {
    throw std::runtime_error("Illegal keyword");
  }

  // return.
  xml_tokens_.emplace_back("<keyword> return </keyword>");
  tokens_->advance();

  // expression?
  if (isExpression()) {
    compileExpression();
  }

  // ';'.
  if (tokens_->symbol() != ";") {
    throw std::runtime_error("should be ;");
  }
  xml_tokens_.emplace_back("<symbol> ; </symbol>");
  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::compileExpression() {
  if (!isExpression()) {
    throw std::runtime_error("Illegal expression");
  }

  // term.
  compileTerm();

  // (op term)*.
  while (isOp()) {
    // op.
    compileOp();

    // term.
    compileTerm();
  }
}

void CompilationEngine::compileOp() {
  if (!isOp()) {
    throw std::runtime_error("Illegal operation");
  }

  xml_tokens_.emplace_back("<symbol> " + tokens_->symbol() + " </symbol>");
  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::compileUnaryOp() {
  if (!isUnaryOp()) {
    throw std::runtime_error("Illegal unary operation");
  }

  xml_tokens_.emplace_back("<symbol> " + tokens_->symbol() + " </symbol>");
  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::compileTerm() {
  if (!isTerm()) {
    throw std::runtime_error("Illegal term");
  }

  // integerConstant | stringConstant | keywordConstant | varName | varName '['
  // expression ']' | subroutineCall | '(' expression ')' | unaryOp term.
  if (tokens_->tokenType() == TokenType::kIntConst) {
    xml_tokens_.emplace_back("<integerConstant> " +
                             std::to_string(tokens_->intVal()) +
                             " </integerConstant>");
    if (tokens_->hasMoreTokens()) {
      tokens_->advance();
    }
  } else if (tokens_->tokenType() == TokenType::kStringConst) {
    xml_tokens_.emplace_back("<stringConstant> " + tokens_->stringVal() +
                             " </stringConstant>");
    if (tokens_->hasMoreTokens()) {
      tokens_->advance();
    }
  } else if (isKeywordConstant()) {
    compileKeywordConstant();
  } else if (isSubroutineCall()) {
    compileSubroutineCall();
  } else if (tokens_->tokenType() == TokenType::kIdentifier) {
    // varName.
    if (!subroutine_var_name_decs_->isDeclared(tokens_->identifier()) &&
        !class_var_name_decs_->isDeclared(tokens_->identifier())) {
      throw std::runtime_error("undefined varName");
    }
    xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                             " </identifier>");
    if (tokens_->hasMoreTokens()) {
      tokens_->advance();
    }

    // '[' expression ']' if exists.
    if (tokens_->tokenType() == TokenType::kSymbol &&
        tokens_->symbol() == "[") {
      // '['.
      xml_tokens_.emplace_back("<symbol> [ </symbol>");
      tokens_->advance();

      // expression.
      compileExpression();

      // ']'.
      if (tokens_->symbol() != "]") {
        throw std::runtime_error("should be ]");
      }
      xml_tokens_.emplace_back("<symbol> ] </symbol>");
      if (tokens_->hasMoreTokens()) {
        tokens_->advance();
      }
    }
  } else if (tokens_->tokenType() == TokenType::kSymbol &&
             tokens_->symbol() == "(") {
    // '('.
    xml_tokens_.emplace_back("<symbol> ( </symbol>");
    tokens_->advance();

    // expressionList.
    compileExpressionList();

    // ')'.
    if (tokens_->symbol() != ")") {
      throw std::runtime_error("should be )");
    }
    xml_tokens_.emplace_back("<symbol> ) </symbol>");
    if (tokens_->hasMoreTokens()) {
      tokens_->advance();
    }
  } else if (isUnaryOp()) {
    compileUnaryOp();
    compileTerm();
  } else {
    throw std::runtime_error("Illegal term");
  }
}

void CompilationEngine::compileSubroutineCall() {
  if (!isSubroutineCall()) {
    throw std::runtime_error("Illegal subroutinecall");
  }

  xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                           " </identifier>");
  tokens_->advance();

  // If '.', '.' subroutineName '(' expressionList ')'.
  // Else if '(', '(' expressionList ')'.
  if (tokens_->symbol() == ".") {
    xml_tokens_.emplace_back("<symbol> . </symbol>");
    tokens_->advance();

    // subroutineName.
    if (tokens_->tokenType() != TokenType::kIdentifier) {
      throw std::runtime_error("should be identifier");
    }
    xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                             " </identifier>");
    tokens_->advance();
  }

  // "(".
  if (tokens_->symbol() != "(") {
    throw std::runtime_error("should be (");
  }
  xml_tokens_.emplace_back("<symbol> ( </symbol>");
  tokens_->advance();

  // expressionList.
  compileExpressionList();

  // ")".
  if (tokens_->symbol() != ")") {
    throw std::runtime_error("should be )");
  }
  xml_tokens_.emplace_back("<symbol> ) </symbol>");
  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::compileExpressionList() {
  // (expression (',' expression)* )?.
  if (!isExpression()) {
    return;
  }

  // expression.
  compileExpression();

  // (',' expression)*.
  while (tokens_->tokenType() == TokenType::kSymbol &&
         tokens_->symbol() == ",") {
    xml_tokens_.emplace_back("<symbol> , </symbol>");
    tokens_->advance();

    // expression.
    compileExpression();
  }
}

void CompilationEngine::compileKeywordConstant() {
  switch (tokens_->keyWord()) {
    case KeyWordType::kTrue:
      xml_tokens_.emplace_back("<keyword> true </keyword>");
      break;
    case KeyWordType::kFalse:
      xml_tokens_.emplace_back("<keyword> false </keyword>");
      break;
    case KeyWordType::kNull:
      xml_tokens_.emplace_back("<keyword> null </keyword>");
      break;
    case KeyWordType::kThis:
      xml_tokens_.emplace_back("<keyword> this </keyword>");
      break;
    default:
      throw std::runtime_error("illegal keyword");
  }

  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

bool CompilationEngine::isTerm() {
  if (tokens_->tokenType() == TokenType::kIntConst ||
      tokens_->tokenType() == TokenType::kStringConst || isKeywordConstant() ||
      isSubroutineCall() || tokens_->tokenType() == TokenType::kIdentifier ||
      (tokens_->tokenType() == TokenType::kSymbol &&
       tokens_->symbol() == "(") ||
      isUnaryOp()) {
    return true;
  }
  return false;
}

bool CompilationEngine::isExpression() { return isTerm(); }

bool CompilationEngine::isSubroutineCall() {
  // subroutineName, className, ClassVarName, SubroutineVarName.
  // TODO(me): Support className from other classes. Linking?
  if (tokens_->tokenType() != TokenType::kIdentifier) {
    return false;
  }

  if (subroutine_name_decs_->isDeclared(tokens_->identifier())) {
    return true;
  }

  if (class_name_decs_->isDeclared(tokens_->identifier()) ||
      class_var_name_decs_->isDeclared(tokens_->identifier()) ||
      subroutine_var_name_decs_->isDeclared(tokens_->identifier())) {
    // Make sure it uses subroutines.
    if (!tokens_->hasMoreTokens()) {
      return false;
    }
    tokens_->advance();

    if (tokens_->tokenType() == TokenType::kSymbol &&
        tokens_->symbol() == ".") {
      tokens_->recede();
      return true;
    }
    tokens_->recede();
  }

  return false;
}

bool CompilationEngine::isKeywordConstant() {
  if (tokens_->tokenType() == TokenType::kKeyWord &&
      (tokens_->keyWord() == KeyWordType::kTrue ||
       tokens_->keyWord() == KeyWordType::kFalse ||
       tokens_->keyWord() == KeyWordType::kNull ||
       tokens_->keyWord() == KeyWordType::kThis)) {
    return true;
  }
  return false;
}

bool CompilationEngine::isUnaryOp() {
  if (tokens_->tokenType() == TokenType::kSymbol &&
      (tokens_->symbol() == "-" || tokens_->symbol() == "~")) {
    return true;
  }
  return false;
}

bool CompilationEngine::isOp() {
  if (tokens_->tokenType() == TokenType::kSymbol &&
      (tokens_->symbol() == "+" || tokens_->symbol() == "-" ||
       tokens_->symbol() == "*" || tokens_->symbol() == "/" ||
       tokens_->symbol() == "&" || tokens_->symbol() == "|" ||
       tokens_->symbol() == "<" || tokens_->symbol() == ">" ||
       tokens_->symbol() == "=")) {
    return true;
  }
  return false;
}

bool CompilationEngine::isStatement() {
  if (tokens_->tokenType() == TokenType::kKeyWord) {
    const auto keyword = tokens_->keyWord();
    if (keyword == KeyWordType::kLet || keyword == KeyWordType::kIf ||
        keyword == KeyWordType::kDo || keyword == KeyWordType::kReturn) {
      return true;
    }
  }
  return false;
}

void CompilationEngine::compileStatement() {
  switch (tokens_->keyWord()) {
    case KeyWordType::kLet:
      compileLet();
      break;
    case KeyWordType::kIf:
      compileIf();
      break;
    case KeyWordType::kWhile:
      compileWhile();
      break;
    case KeyWordType::kDo:
      compileDo();
      break;
    case KeyWordType::kReturn:
      compileReturn();
      break;
    default:
      throw std::runtime_error("Illegal keyword");
  }
}

void CompilationEngine::compileType() {
  if (!isType()) {
    throw std::runtime_error("Invalid type");
  }

  if (tokens_->tokenType() == TokenType::kKeyWord) {
    const auto keyword = tokens_->keyWord();
    if (keyword == KeyWordType::kInt) {
      xml_tokens_.emplace_back("<keyword> int </keyword>");
    } else if (keyword == KeyWordType::kChar) {
      xml_tokens_.emplace_back("<keyword> char </keyword>");
    } else if (keyword == KeyWordType::kBoolean) {
      xml_tokens_.emplace_back("<keyword> boolean </keyword>");
    } else {
      throw std::runtime_error("Illegal keyword");
    }
  } else if (tokens_->tokenType() == TokenType::kIdentifier) {
    xml_tokens_.emplace_back("<identifier> " + tokens_->identifier() +
                             " </identifier>");
  } else {
    throw std::runtime_error("Invalid type");
  }

  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

bool CompilationEngine::isType() {
  if (tokens_->tokenType() == TokenType::kKeyWord) {
    const auto keyword = tokens_->keyWord();
    if (keyword == KeyWordType::kInt || keyword == KeyWordType::kChar ||
        keyword == KeyWordType::kBoolean) {
      return true;
    } else {
      return false;
    }
  } else if (tokens_->tokenType() == TokenType::kIdentifier) {
    return true;
  }

  return false;
}

void CompilationEngine::compileClassVarDecStar() {
  while (isClassVarDec()) {
    compileClassVarDec();
  }
}

bool CompilationEngine::isClassVarDec() {
  // static field
  if (tokens_->tokenType() == TokenType::kKeyWord) {
    const auto keyword = tokens_->keyWord();
    if (keyword == KeyWordType::kStatic || keyword == KeyWordType::kField) {
      return true;
    }
  }

  return false;
}

void CompilationEngine::compileSubroutineStar() {
  while (isSubroutine()) {
    compileSubroutine();
  }
}

bool CompilationEngine::isSubroutine() {
  // constructor function method.
  if (tokens_->tokenType() == TokenType::kKeyWord) {
    const auto keyword = tokens_->keyWord();
    if (keyword == KeyWordType::kConstructor ||
        keyword == KeyWordType::kFunction || keyword == KeyWordType::kMethod) {
      return true;
    }
  }

  return false;
}

void CompilationEngine::compileSubroutineBody() {
  // '{'.
  if (tokens_->symbol() != "{") {
    throw std::runtime_error("should be {");
  }
  xml_tokens_.emplace_back("<symbol> { </symbol>");
  tokens_->advance();

  // varDec*.
  while (tokens_->tokenType() == TokenType::kKeyWord &&
         tokens_->keyWord() == KeyWordType::kVar) {
    compileVarDec();
  }

  // statements.
  compileStatements();

  // '}'.
  if (tokens_->symbol() != "}") {
    throw std::runtime_error("should be }");
  }
  xml_tokens_.emplace_back("<symbol> } </symbol>");

  if (tokens_->hasMoreTokens()) {
    tokens_->advance();
  }
}

void CompilationEngine::initializeXML() {
  xml_tokens_.clear();
  xml_tokens_.emplace_back("<tokens>");

  // Should always start with class.
  xml_tokens_.emplace_back("<class>");
}

void CompilationEngine::writeXMLTokens() const noexcept {
  std::ofstream output(output_filename_);
  for (const auto& token : xml_tokens_) {
    output << token << std::endl;
  }
  output.close();
}
