#ifndef SAPPHIRE_INTERPRETER_H
#define SAPPHIRE_INTERPRETER_H

#include "../parser/stmt.h"
#include "../parser/expr.h"
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <variant>

namespace sapphire {

// Forward declarations
class Environment;
class Interpreter;
class Function;
class Class;
class Instance;
class BoundMethod;
struct ArrayValue;

// Function object to store function definitions
class Function {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::string return_type;
    std::vector<std::unique_ptr<Stmt>>* body;
    std::shared_ptr<Environment> closure;
    
    Function(const std::string& n, 
             const std::vector<std::pair<std::string, std::string>>& params,
             const std::string& ret_type,
             std::vector<std::unique_ptr<Stmt>>* b,
             std::shared_ptr<Environment> c)
        : name(n), parameters(params), return_type(ret_type), body(b), closure(c) {}
};

// Class object representing a user-defined class
class Class {
public:
    std::string name;
    std::shared_ptr<Class> superclass;
    std::map<std::string, std::shared_ptr<Function>> methods;
    
    Class(const std::string& n,
          std::shared_ptr<Class> super,
          const std::map<std::string, std::shared_ptr<Function>>& m)
        : name(n), superclass(std::move(super)), methods(m) {}
    
    std::shared_ptr<Function> findMethod(const std::string& method_name) const;
};

// Forward declare Value for use in Instance
using Value = std::variant<int, double, std::string, bool, std::nullptr_t, 
                           std::shared_ptr<ArrayValue>,
                           std::shared_ptr<Function>,
                           std::shared_ptr<Class>,
                           std::shared_ptr<Instance>,
                           std::shared_ptr<BoundMethod>>;

// Array type (defined after Value)
struct ArrayValue {
    std::vector<Value> elements;
};

// Instance of a class
class Instance {
public:
    std::shared_ptr<Class> klass;
    std::map<std::string, Value> fields;
    
    explicit Instance(std::shared_ptr<Class> k) : klass(std::move(k)) {}
};

// Bound method (instance + function)
class BoundMethod {
public:
    std::shared_ptr<Instance> instance;
    std::shared_ptr<Function> method;
    
    BoundMethod(std::shared_ptr<Instance> inst, std::shared_ptr<Function> m)
        : instance(std::move(inst)), method(std::move(m)) {}
};

class Environment {
private:
    std::map<std::string, Value> values;
    std::shared_ptr<Environment> enclosing;

public:
    Environment() : enclosing(nullptr) {}
    explicit Environment(std::shared_ptr<Environment> enc) : enclosing(enc) {}
    
    void define(const std::string& name, const Value& value);
    Value get(const std::string& name);
    void assign(const std::string& name, const Value& value);
};

class ReturnException : public std::exception {
public:
    Value value;
    explicit ReturnException(const Value& v) : value(v) {}
};

class Interpreter : public ExprVisitor, public StmtVisitor {
private:
    std::shared_ptr<Environment> environment;
    Value last_value;
    
    std::string valueToString(const Value& value);
    bool isTruthy(const Value& value);
    Value evaluateExpr(Expr& expr);
    void throwException(const std::string& type, const std::string& message);

public:
    Interpreter();
    void interpret(std::vector<std::unique_ptr<Stmt>>& statements);
    
    // Expression visitors
    void visitLiteralExpr(LiteralExpr& expr) override;
    void visitVariableExpr(VariableExpr& expr) override;
    void visitAssignExpr(AssignExpr& expr) override;
    void visitBinaryExpr(BinaryExpr& expr) override;
    void visitUnaryExpr(UnaryExpr& expr) override;
    void visitCallExpr(CallExpr& expr) override;
    void visitListExpr(ListExpr& expr) override;
    void visitIndexExpr(IndexExpr& expr) override;
    void visitGetExpr(GetExpr& expr) override;
    void visitSetExpr(SetExpr& expr) override;
    
    // Statement visitors
    void visitExprStmt(ExprStmt& stmt) override;
    void visitVarDeclStmt(VarDeclStmt& stmt) override;
    void visitFunctionDecl(FunctionDecl& stmt) override;
    void visitReturnStmt(ReturnStmt& stmt) override;
    void visitIfStmt(IfStmt& stmt) override;
    void visitWhileStmt(WhileStmt& stmt) override;
    void visitForStmt(ForStmt& stmt) override;
    void visitTryStmt(TryStmt& stmt) override;
    void visitThrowStmt(ThrowStmt& stmt) override;
    void visitClassDecl(ClassDecl& stmt) override;
    void visitImportStmt(ImportStmt& stmt) override;
};

} // namespace sapphire

#endif // SAPPHIRE_INTERPRETER_H
