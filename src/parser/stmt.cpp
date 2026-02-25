#include "stmt.h"

namespace sapphire {

void ExprStmt::accept(StmtVisitor& visitor) {
    visitor.visitExprStmt(*this);
}

void VarDeclStmt::accept(StmtVisitor& visitor) {
    visitor.visitVarDeclStmt(*this);
}

void FunctionDecl::accept(StmtVisitor& visitor) {
    visitor.visitFunctionDecl(*this);
}

void ReturnStmt::accept(StmtVisitor& visitor) {
    visitor.visitReturnStmt(*this);
}

void IfStmt::accept(StmtVisitor& visitor) {
    visitor.visitIfStmt(*this);
}

void WhileStmt::accept(StmtVisitor& visitor) {
    visitor.visitWhileStmt(*this);
}

void ForStmt::accept(StmtVisitor& visitor) {
    visitor.visitForStmt(*this);
}

} // namespace sapphire
