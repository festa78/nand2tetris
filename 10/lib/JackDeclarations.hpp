// No copyright.
// Declaration manager.

#ifndef LIB_JACKDECLARATIONS_HPP_
#define LIB_JACKDECLARATIONS_HPP_

#include <string>
#include <unordered_set>

class IJackDeclarations {
 public:
  virtual ~IJackDeclarations() = default;
  virtual void addDeclaration(const std::string& class_variable_name) = 0;
  virtual bool isDeclared(const std::string& class_variable_name) = 0;
  virtual bool isEmpty() = 0;
  virtual void clear() = 0;
};

class JackDeclarations final : public IJackDeclarations {
 public:
  JackDeclarations() = default;
  ~JackDeclarations() = default;

  void addDeclaration(const std::string& class_variable_name);
  bool isDeclared(const std::string& class_variable_name);
  // TODO(me): remove the below.
  bool isEmpty();
  void clear();

 private:
  std::unordered_set<std::string> declarations_;
};

#endif  // LIB_JACKDECLARATIONS_HPP_
