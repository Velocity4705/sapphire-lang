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

// Value type for runtime values
using Value = std::variant<int, double, std::string, bool, std::nullptr_t, 
                           std::vector<std::shared_ptr<void>>>;

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

public:
    Interpreter();
    void interpret(std::vector<std::unique_ptr<Stmt>>& statements);
    
    // Expression visitors
    void visitLiteralExpr(LiteralExpr& expr) override;
    void visitVariableExpr(VariableExpr& expr) override;
    void visitBinaryExpr(BinaryExpr& expr) override;
    void visitUnaryExpr(UnaryExpr& expr) override;
    void visitCallExpr(CallExpr& expr) override;
    void visitListExpr(ListExpr& expr) override;
    void visitIndexExpr(IndexExpr& expr) override;
    
    // Statement visitors
    void visitExprStmt(ExprStmt& stmt) override;
    void visitVarDeclStmt(VarDeclStmt& stmt) override;
    void visitFunctionDecl(FunctionDecl& stmt) override;
    void visitReturnStmt(ReturnStmt& stmt) override;
    void visitIfStmt(IfStmt& stmt) override;
    void visitWhileStmt(WhileStmt& stmt) override;
    void visitForStmt(ForStmt& stmt) override;
};

} // namespace sapphire

#endif // SAPPHIRE_INTERPRETER_H
