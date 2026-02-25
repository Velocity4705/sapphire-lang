#ifndef SAPPHIRE_TYPE_INFERENCE_H
#define SAPPHIRE_TYPE_INFERENCE_H

#include "../parser/expr.h"
#include "../parser/stmt.h"
#include "../types/type.h"
#include "type_environment.h"
#include "constraints.h"
#include "substitution.h"
#include "unification.h"
#include <memory>
#include <vector>

namespace sapphire {

class TypeInference : public ExprVisitor, public StmtVisitor {
private:
    std::shared_ptr<TypeEnvironment> env;
    std::shared_ptr<Type> current_type;
    ConstraintSet constraints;
    Substitution substitution;
    
    // Generate a fresh type variable
    std::shared_ptr<Type> freshTypeVar() {
        return std::make_shared<TypeVariable>();
    }
    
    // Infer type of an expression
    std::shared_ptr<Type> inferExpr(Expr& expr) {
        expr.accept(*this);
        return current_type;
    }
    
    // Solve constraints using unification
    void solveConstraints() {
        for (const auto& constraint : constraints.getConstraints()) {
            try {
                substitution = Unifier::unify(
                    constraint.getLeft(),
                    constraint.getRight(),
                    substitution
                );
            } catch (const UnificationError& e) {
                throw std::runtime_error("Type error in " + constraint.getContext() + 
                                       ": " + e.what());
            }
        }
    }

public:
    TypeInference() {
        env = std::make_shared<TypeEnvironment>();
        
        // Built-in functions with polymorphic types
        auto print_type = std::make_shared<FunctionType>(
            std::vector<std::shared_ptr<Type>>{freshTypeVar()},
            std::make_shared<NoneType>()
        );
        env->define("print", print_type);
        
        auto range_type = std::make_shared<FunctionType>(
            std::vector<std::shared_ptr<Type>>{std::make_shared<IntType>()},
            std::make_shared<ListType>(std::make_shared<IntType>())
        );
        env->define("range", range_type);
    }
    
    // Infer types for a program
    void infer(std::vector<std::unique_ptr<Stmt>>& statements) {
        // Generate constraints
        for (auto& stmt : statements) {
            stmt->accept(*this);
        }
        
        // Solve constraints
        solveConstraints();
        
        // Apply substitution to environment
        // (This would update all type variables with their inferred types)
    }
    
    // Get the inferred type after solving
    std::shared_ptr<Type> getInferredType(std::shared_ptr<Type> type) {
        return substitution.apply(type);
    }
    
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

#endif // SAPPHIRE_TYPE_INFERENCE_H
