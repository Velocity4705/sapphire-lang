#ifndef SAPPHIRE_AST_H
#define SAPPHIRE_AST_H

#include <memory>
#include <vector>
#include <string>

namespace sapphire {

// Base AST node
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Expressions
class Expr : public ASTNode {};

class LiteralExpr : public Expr {
public:
    enum class Type { INTEGER, FLOAT, STRING, BOOLEAN };
    Type type;
    std::string value;
    
    LiteralExpr(Type t, const std::string& v) : type(t), value(v) {}
};

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    std::string op;
    std::unique_ptr<Expr> right;
    
    BinaryExpr(std::unique_ptr<Expr> l, const std::string& o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

class VariableExpr : public Expr {
public:
    std::string name;
    explicit VariableExpr(const std::string& n) : name(n) {}
};

// Statements
class Stmt : public ASTNode {};

class VarDeclStmt : public Stmt {
public:
    std::string name;
    std::string type;
    std::unique_ptr<Expr> initializer;
    
    VarDeclStmt(const std::string& n, const std::string& t, std::unique_ptr<Expr> init)
        : name(n), type(t), initializer(std::move(init)) {}
};

class FunctionDecl : public Stmt {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::string returnType;
    std::vector<std::unique_ptr<Stmt>> body;
};

} // namespace sapphire

#endif // SAPPHIRE_AST_H
