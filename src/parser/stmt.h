#ifndef SAPPHIRE_STMT_H
#define SAPPHIRE_STMT_H

#include "expr.h"
#include <memory>
#include <string>
#include <vector>

namespace sapphire {

// Forward declarations
class StmtVisitor;

// Base statement class
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

// Expression statement
class ExprStmt : public Stmt {
public:
    std::unique_ptr<Expr> expression;
    
    explicit ExprStmt(std::unique_ptr<Expr> expr)
        : expression(std::move(expr)) {}
    void accept(StmtVisitor& visitor) override;
};

// Variable declaration
class VarDeclStmt : public Stmt {
public:
    std::string name;
    std::string type;
    std::unique_ptr<Expr> initializer;
    bool is_const;
    
    VarDeclStmt(const std::string& n, const std::string& t, 
                std::unique_ptr<Expr> init, bool is_const = false)
        : name(n), type(t), initializer(std::move(init)), is_const(is_const) {}
    void accept(StmtVisitor& visitor) override;
};

// Function declaration
class FunctionDecl : public Stmt {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::string return_type;
    std::vector<std::unique_ptr<Stmt>> body;
    bool is_async;
    
    FunctionDecl(const std::string& n,
                 std::vector<std::pair<std::string, std::string>> params,
                 const std::string& ret_type,
                 std::vector<std::unique_ptr<Stmt>> b,
                 bool async = false)
        : name(n), parameters(std::move(params)), return_type(ret_type),
          body(std::move(b)), is_async(async) {}
    void accept(StmtVisitor& visitor) override;
};

// Return statement

class ReturnStmt : public Stmt {
public:
    std::unique_ptr<Expr> value;
    
    explicit ReturnStmt(std::unique_ptr<Expr> v = nullptr)
        : value(std::move(v)) {}
    void accept(StmtVisitor& visitor) override;
};

// If statement
class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> then_branch;
    std::vector<std::unique_ptr<Stmt>> else_branch;
    
    IfStmt(std::unique_ptr<Expr> cond,
           std::vector<std::unique_ptr<Stmt>> then_b,
           std::vector<std::unique_ptr<Stmt>> else_b = {})
        : condition(std::move(cond)), then_branch(std::move(then_b)),
          else_branch(std::move(else_b)) {}
    void accept(StmtVisitor& visitor) override;
};

// While statement
class WhileStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> body;
    
    WhileStmt(std::unique_ptr<Expr> cond, std::vector<std::unique_ptr<Stmt>> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    void accept(StmtVisitor& visitor) override;
};

// For statement
class ForStmt : public Stmt {
public:
    std::string variable;
    std::unique_ptr<Expr> iterable;
    std::vector<std::unique_ptr<Stmt>> body;
    
    ForStmt(const std::string& var, std::unique_ptr<Expr> iter,
            std::vector<std::unique_ptr<Stmt>> b)
        : variable(var), iterable(std::move(iter)), body(std::move(b)) {}
    void accept(StmtVisitor& visitor) override;
};

// Catch clause for try statement
class CatchClause {
public:
    std::string exception_type;  // Empty string means catch all
    std::string variable_name;   // Variable to bind exception to
    std::vector<std::unique_ptr<Stmt>> body;
    
    CatchClause(const std::string& type, const std::string& var,
                std::vector<std::unique_ptr<Stmt>> b)
        : exception_type(type), variable_name(var), body(std::move(b)) {}
};

// Try statement
class TryStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> try_body;
    std::vector<CatchClause> catch_clauses;
    std::vector<std::unique_ptr<Stmt>> finally_body;
    
    TryStmt(std::vector<std::unique_ptr<Stmt>> try_b,
            std::vector<CatchClause> catches,
            std::vector<std::unique_ptr<Stmt>> finally_b)
        : try_body(std::move(try_b)),
          catch_clauses(std::move(catches)),
          finally_body(std::move(finally_b)) {}
    void accept(StmtVisitor& visitor) override;
};

// Throw statement
class ThrowStmt : public Stmt {
public:
    std::string exception_type;
    std::unique_ptr<Expr> message;
    
    ThrowStmt(const std::string& type, std::unique_ptr<Expr> msg)
        : exception_type(type), message(std::move(msg)) {}
    void accept(StmtVisitor& visitor) override;
};

// Visitor interface
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visitExprStmt(ExprStmt& stmt) = 0;
    virtual void visitVarDeclStmt(VarDeclStmt& stmt) = 0;
    virtual void visitFunctionDecl(FunctionDecl& stmt) = 0;
    virtual void visitReturnStmt(ReturnStmt& stmt) = 0;
    virtual void visitIfStmt(IfStmt& stmt) = 0;
    virtual void visitWhileStmt(WhileStmt& stmt) = 0;
    virtual void visitForStmt(ForStmt& stmt) = 0;
    virtual void visitTryStmt(TryStmt& stmt) = 0;
    virtual void visitThrowStmt(ThrowStmt& stmt) = 0;
};

} // namespace sapphire

#endif // SAPPHIRE_STMT_H
