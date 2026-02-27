#ifndef SAPPHIRE_TYPE_CHECKER_H
#define SAPPHIRE_TYPE_CHECKER_H

#include "../parser/expr.h"
#include "../parser/stmt.h"
#include "../types/type.h"
#include "type_environment.h"
#include <memory>
#include <vector>
#include <stdexcept>

namespace sapphire {

class TypeError : public std::runtime_error {
public:
    explicit TypeError(const std::string& msg) : std::runtime_error(msg) {}
};

class TypeChecker : public ExprVisitor, public StmtVisitor {
private:
    std::shared_ptr<TypeEnvironment> env;
    std::shared_ptr<Type> current_type;
    std::vector<std::string> errors;
    
    void error(const std::string& message) {
        errors.push_back(message);
    }
    
    std::shared_ptr<Type> checkExpr(Expr& expr) {
        expr.accept(*this);
        return current_type;
    }

public:
    TypeChecker() {
        env = std::make_shared<TypeEnvironment>();
        
        // Built-in functions
        auto print_type = std::make_shared<FunctionType>(
            std::vector<std::shared_ptr<Type>>{std::make_shared<TypeVariable>()},
            std::make_shared<NoneType>()
        );
        env->define("print", print_type);
        
        auto range_type = std::make_shared<FunctionType>(
            std::vector<std::shared_ptr<Type>>{std::make_shared<IntType>()},
            std::make_shared<ListType>(std::make_shared<IntType>())
        );
        env->define("range", range_type);
    }
    
    void check(std::vector<std::unique_ptr<Stmt>>& statements) {
        for (auto& stmt : statements) {
            stmt->accept(*this);
        }
        
        if (!errors.empty()) {
            std::string error_msg = "Type errors:\n";
            for (const auto& err : errors) {
                error_msg += "  - " + err + "\n";
            }
            throw TypeError(error_msg);
        }
    }
    
    // Expression visitors
    void visitLiteralExpr(LiteralExpr& expr) override;
    void visitVariableExpr(VariableExpr& expr) override;
    void visitBinaryExpr(BinaryExpr& expr) override;
    void visitUnaryExpr(UnaryExpr& expr) override;
    void visitCallExpr(CallExpr& expr) override;
    void visitListExpr(ListExpr& expr) override;
    void visitIndexExpr(IndexExpr& expr) override;
    void visitAssignExpr(AssignExpr& expr) override;
    
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

#endif // SAPPHIRE_TYPE_CHECKER_H
