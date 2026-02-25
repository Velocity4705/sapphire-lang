#include "expr.h"

namespace sapphire {

void LiteralExpr::accept(ExprVisitor& visitor) {
    visitor.visitLiteralExpr(*this);
}

void VariableExpr::accept(ExprVisitor& visitor) {
    visitor.visitVariableExpr(*this);
}

void BinaryExpr::accept(ExprVisitor& visitor) {
    visitor.visitBinaryExpr(*this);
}

void UnaryExpr::accept(ExprVisitor& visitor) {
    visitor.visitUnaryExpr(*this);
}

void CallExpr::accept(ExprVisitor& visitor) {
    visitor.visitCallExpr(*this);
}

void ListExpr::accept(ExprVisitor& visitor) {
    visitor.visitListExpr(*this);
}

void IndexExpr::accept(ExprVisitor& visitor) {
    visitor.visitIndexExpr(*this);
}

} // namespace sapphire
