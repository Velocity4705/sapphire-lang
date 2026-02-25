#ifndef SAPPHIRE_EXPR_H
#define SAPPHIRE_EXPR_H

#include <memory>
#include <string>
#include <vector>

namespace sapphire {

// Forward declarations
class ExprVisitor;

// Base expression class
class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

// Literal expression
class LiteralExpr : public Expr {
public:
    enum class Type { INTEGER, FLOAT, STRING, BOOLEAN, NONE };
    Type type;
    std::string value;
    
    LiteralExpr(Type t, const std::string& v) : type(t), value(v) {}
    void accept(ExprVisitor& visitor) override;
};

// Variable expression
class VariableExpr : public Expr {
public:
    std::string name;
    
    explicit VariableExpr(const std::string& n) : name(n) {}
    void accept(ExprVisitor& visitor) override;
};

// Binary expression
class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    std::string op;
    std::unique_ptr<Expr> right;
    
    BinaryExpr(std::unique_ptr<Expr> l, const std::string& o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
    void accept(ExprVisitor& visitor) override;
};

// Unary expression
class UnaryExpr : public Expr {
public:
    std::string op;
    std::unique_ptr<Expr> operand;
    
    UnaryExpr(const std::string& o, std::unique_ptr<Expr> operand)
        : op(o), operand(std::move(operand)) {}
    void accept(ExprVisitor& visitor) override;
};

// Call expression
class CallExpr : public Expr {
public:
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> arguments;
    
    CallExpr(std::unique_ptr<Expr> c, std::vector<std::unique_ptr<Expr>> args)
        : callee(std::move(c)), arguments(std::move(args)) {}
    void accept(ExprVisitor& visitor) override;
};

// List expression
class ListExpr : public Expr {
public:
    std::vector<std::unique_ptr<Expr>> elements;
    
    explicit ListExpr(std::vector<std::unique_ptr<Expr>> elems)
        : elements(std::move(elems)) {}
    void accept(ExprVisitor& visitor) override;
};

// Index expression
class IndexExpr : public Expr {
public:
    std::unique_ptr<Expr> object;
    std::unique_ptr<Expr> index;
    
    IndexExpr(std::unique_ptr<Expr> obj, std::unique_ptr<Expr> idx)
        : object(std::move(obj)), index(std::move(idx)) {}
    void accept(ExprVisitor& visitor) override;
};

// Visitor interface
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visitLiteralExpr(LiteralExpr& expr) = 0;
    virtual void visitVariableExpr(VariableExpr& expr) = 0;
    virtual void visitBinaryExpr(BinaryExpr& expr) = 0;
    virtual void visitUnaryExpr(UnaryExpr& expr) = 0;
    virtual void visitCallExpr(CallExpr& expr) = 0;
    virtual void visitListExpr(ListExpr& expr) = 0;
    virtual void visitIndexExpr(IndexExpr& expr) = 0;
};

} // namespace sapphire

#endif // SAPPHIRE_EXPR_H
