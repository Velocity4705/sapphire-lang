#include "llvm_codegen.h"
#include "codegen_impl.h"
#include "../parser/stmt.h"
#include "../parser/expr.h"
#include "../lexer/token.h"
#include <iostream>
#include <stdexcept>

#ifdef HAVE_LLVM

namespace sapphire {

LLVMCodeGen::LLVMCodeGen(const std::string& module_name) 
    : module_name_(module_name), impl_(nullptr), current_function_(nullptr) {
    
    // Create implementation
    impl_ = new LLVMCodeGenImpl(module_name);
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    
    // Initialize LLVM
    impl->initialize();
    
    std::cout << "✓ LLVM Code Generator initialized\n";
    std::cout << "  Module: " << module_name << "\n";
    std::cout << "  Phase 2: Expression generation enabled\n";
}

LLVMCodeGen::~LLVMCodeGen() {
    if (impl_) {
        delete static_cast<LLVMCodeGenImpl*>(impl_);
    }
}

void LLVMCodeGen::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    
    // Create main function
    impl->createMainFunction();
    
    // Generate code for each statement
    for (const auto& stmt : statements) {
        generateStmt(stmt.get());
    }
    
    // Finalize main function
    impl->finalizeMainFunction();
    
    std::cout << "✓ Code generation complete\n";
}

void LLVMCodeGen::printIR() const {
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    impl->module->print(llvm::outs(), nullptr);
}

void LLVMCodeGen::writeIR(const std::string& filename) {
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    
    std::error_code ec;
    llvm::raw_fd_ostream file(filename, ec, llvm::sys::fs::OF_None);
    if (ec) {
        throw std::runtime_error("Could not open file: " + ec.message());
    }
    impl->module->print(file, nullptr);
}

void LLVMCodeGen::optimize(int level) {
    if (level == 0) return;
    
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    
    // Create pass manager
    llvm::legacy::FunctionPassManager fpm(impl->module.get());
    
    // Add optimization passes based on level
    if (level >= 1) {
        fpm.add(llvm::createInstructionCombiningPass());
        fpm.add(llvm::createReassociatePass());
    }
    
    if (level >= 2) {
        fpm.add(llvm::createGVNPass());
        fpm.add(llvm::createCFGSimplificationPass());
    }
    
    if (level >= 3) {
        fpm.add(llvm::createDeadCodeEliminationPass());
    }
    
    fpm.doInitialization();
    
    // Run passes on all functions
    for (auto& func : *impl->module) {
        fpm.run(func);
    }
}

// Expression generation
llvm::Value* LLVMCodeGen::generateExpr(Expr* expr) {
    if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        return generateBinaryExpr(binary);
    } else if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
        return generateUnaryExpr(unary);
    } else if (auto* literal = dynamic_cast<LiteralExpr*>(expr)) {
        return generateLiteralExpr(literal);
    } else if (auto* variable = dynamic_cast<VariableExpr*>(expr)) {
        return generateVariableExpr(variable);
    } else if (auto* call = dynamic_cast<CallExpr*>(expr)) {
        return generateCallExpr(call);
    }
    
    throw std::runtime_error("Unknown expression type");
}

llvm::Value* LLVMCodeGen::generateLiteralExpr(LiteralExpr* expr) {
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    
    LiteralExpr::Type type = expr->type;
    
    if (type == LiteralExpr::Type::INTEGER) {
        int64_t val = std::stoll(expr->value);
        return llvm::ConstantInt::get(*impl->context, llvm::APInt(64, val, true));
    } 
    else if (type == LiteralExpr::Type::FLOAT) {
        double val = std::stod(expr->value);
        return llvm::ConstantFP::get(*impl->context, llvm::APFloat(val));
    } 
    else if (type == LiteralExpr::Type::BOOLEAN) {
        bool val = (expr->value == "true");
        return llvm::ConstantInt::get(*impl->context, llvm::APInt(1, val ? 1 : 0));
    }
    else if (type == LiteralExpr::Type::STRING) {
        // String literals - create global constant
        return impl->builder->CreateGlobalStringPtr(expr->value);
    }
    else if (type == LiteralExpr::Type::NONE) {
        // None/null - return null pointer for now
        return llvm::ConstantPointerNull::get(llvm::PointerType::get(*impl->context, 0));
    }
    
    throw std::runtime_error("Unknown literal type: " + expr->value);
}

llvm::Value* LLVMCodeGen::generateBinaryExpr(BinaryExpr* expr) {
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    
    const std::string& op = expr->op;
    
    // Handle logical operators with short-circuit evaluation
    if (op == "and" || op == "or") {
        return generateLogicalExpr(expr);
    }
    
    llvm::Value* left = generateExpr(expr->left.get());
    llvm::Value* right = generateExpr(expr->right.get());
    
    // Type promotion if needed
    if (left->getType() != right->getType()) {
        if (left->getType()->isIntegerTy() && right->getType()->isDoubleTy()) {
            left = impl->builder->CreateSIToFP(left, right->getType(), "promoteL");
        }
        else if (left->getType()->isDoubleTy() && right->getType()->isIntegerTy()) {
            right = impl->builder->CreateSIToFP(right, left->getType(), "promoteR");
        }
    }
    
    // Generate operation based on type
    if (left->getType()->isIntegerTy(64)) {
        // Integer operations
        if (op == "+") return impl->builder->CreateAdd(left, right, "addtmp");
        if (op == "-") return impl->builder->CreateSub(left, right, "subtmp");
        if (op == "*") return impl->builder->CreateMul(left, right, "multmp");
        if (op == "/") return impl->builder->CreateSDiv(left, right, "divtmp");
        if (op == "%") return impl->builder->CreateSRem(left, right, "modtmp");
        if (op == "<") return impl->builder->CreateICmpSLT(left, right, "cmptmp");
        if (op == ">") return impl->builder->CreateICmpSGT(left, right, "cmptmp");
        if (op == "<=") return impl->builder->CreateICmpSLE(left, right, "cmptmp");
        if (op == ">=") return impl->builder->CreateICmpSGE(left, right, "cmptmp");
        if (op == "==") return impl->builder->CreateICmpEQ(left, right, "cmptmp");
        if (op == "!=") return impl->builder->CreateICmpNE(left, right, "cmptmp");
    }
    else if (left->getType()->isDoubleTy()) {
        // Float operations
        if (op == "+") return impl->builder->CreateFAdd(left, right, "addtmp");
        if (op == "-") return impl->builder->CreateFSub(left, right, "subtmp");
        if (op == "*") return impl->builder->CreateFMul(left, right, "multmp");
        if (op == "/") return impl->builder->CreateFDiv(left, right, "divtmp");
        if (op == "<") return impl->builder->CreateFCmpOLT(left, right, "cmptmp");
        if (op == ">") return impl->builder->CreateFCmpOGT(left, right, "cmptmp");
        if (op == "<=") return impl->builder->CreateFCmpOLE(left, right, "cmptmp");
        if (op == ">=") return impl->builder->CreateFCmpOGE(left, right, "cmptmp");
        if (op == "==") return impl->builder->CreateFCmpOEQ(left, right, "cmptmp");
        if (op == "!=") return impl->builder->CreateFCmpONE(left, right, "cmptmp");
    }
    else if (left->getType()->isIntegerTy(1)) {
        // Boolean operations
        if (op == "==") return impl->builder->CreateICmpEQ(left, right, "cmptmp");
        if (op == "!=") return impl->builder->CreateICmpNE(left, right, "cmptmp");
    }
    
    throw std::runtime_error("Unknown binary operator or incompatible types: " + op);
}

llvm::Value* LLVMCodeGen::generateLogicalExpr(BinaryExpr* expr) {
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    
    const std::string& op = expr->op;
    
    if (op == "and") {
        // Short-circuit AND: if left is false, don't evaluate right
        llvm::Value* left = generateExpr(expr->left.get());
        
        llvm::BasicBlock* right_bb = llvm::BasicBlock::Create(*impl->context, "and_right", impl->current_function);
        llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*impl->context, "and_merge", impl->current_function);
        
        llvm::BasicBlock* left_bb = impl->builder->GetInsertBlock();
        impl->builder->CreateCondBr(left, right_bb, merge_bb);
        
        // Right side
        impl->builder->SetInsertPoint(right_bb);
        llvm::Value* right = generateExpr(expr->right.get());
        impl->builder->CreateBr(merge_bb);
        right_bb = impl->builder->GetInsertBlock();
        
        // Merge
        impl->builder->SetInsertPoint(merge_bb);
        llvm::PHINode* phi = impl->builder->CreatePHI(impl->getBoolType(), 2, "andtmp");
        phi->addIncoming(left, left_bb);
        phi->addIncoming(right, right_bb);
        return phi;
    }
    else if (op == "or") {
        // Short-circuit OR: if left is true, don't evaluate right
        llvm::Value* left = generateExpr(expr->left.get());
        
        llvm::BasicBlock* right_bb = llvm::BasicBlock::Create(*impl->context, "or_right", impl->current_function);
        llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*impl->context, "or_merge", impl->current_function);
        
        llvm::BasicBlock* left_bb = impl->builder->GetInsertBlock();
        impl->builder->CreateCondBr(left, merge_bb, right_bb);
        
        // Right side
        impl->builder->SetInsertPoint(right_bb);
        llvm::Value* right = generateExpr(expr->right.get());
        impl->builder->CreateBr(merge_bb);
        right_bb = impl->builder->GetInsertBlock();
        
        // Merge
        impl->builder->SetInsertPoint(merge_bb);
        llvm::PHINode* phi = impl->builder->CreatePHI(impl->getBoolType(), 2, "ortmp");
        phi->addIncoming(left, left_bb);
        phi->addIncoming(right, right_bb);
        return phi;
    }
    
    throw std::runtime_error("Unknown logical operator: " + op);
}

llvm::Value* LLVMCodeGen::generateUnaryExpr(UnaryExpr* expr) {
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    llvm::Value* operand = generateExpr(expr->operand.get());
    
    const std::string& op = expr->op;
    
    if (op == "-") {
        if (operand->getType()->isIntegerTy()) {
            return impl->builder->CreateNeg(operand, "negtmp");
        } else if (operand->getType()->isDoubleTy()) {
            return impl->builder->CreateFNeg(operand, "negtmp");
        }
    }
    else if (op == "not") {
        return impl->builder->CreateNot(operand, "nottmp");
    }
    
    throw std::runtime_error("Unknown unary operator: " + op);
}

llvm::Value* LLVMCodeGen::generateVariableExpr(VariableExpr* expr) {
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    
    llvm::Value* var = impl->named_values[expr->name];
    if (!var) {
        throw std::runtime_error("Unknown variable: " + expr->name);
    }
    
    // Get the stored type
    llvm::Type* var_type = impl->named_types[expr->name];
    
    // Load value from memory
    return impl->builder->CreateLoad(var_type, var, expr->name);
}

llvm::Value* LLVMCodeGen::generateCallExpr(CallExpr* expr) {
    // Function calls will be implemented in Phase 4
    (void)expr;
    throw std::runtime_error("Function calls not yet implemented - Phase 4");
}

// Statement generation
void LLVMCodeGen::generateStmt(Stmt* stmt) {
    if (auto* expr_stmt = dynamic_cast<ExprStmt*>(stmt)) {
        generateExpr(expr_stmt->expression.get());
    } 
    else if (auto* var_decl = dynamic_cast<VarDeclStmt*>(stmt)) {
        generateVarDeclStmt(var_decl);
    } 
    else if (auto* if_stmt = dynamic_cast<IfStmt*>(stmt)) {
        generateIfStmt(if_stmt);
    } 
    else if (auto* while_stmt = dynamic_cast<WhileStmt*>(stmt)) {
        generateWhileStmt(while_stmt);
    }
    else if (auto* for_stmt = dynamic_cast<ForStmt*>(stmt)) {
        generateForStmt(for_stmt);
    }
}

void LLVMCodeGen::generateVarDeclStmt(VarDeclStmt* stmt) {
    auto* impl = static_cast<LLVMCodeGenImpl*>(impl_);
    
    llvm::Value* init_val = generateExpr(stmt->initializer.get());
    
    // Allocate stack space for variable
    llvm::AllocaInst* alloca = impl->builder->CreateAlloca(
        init_val->getType(), nullptr, stmt->name);
    
    // Store initial value
    impl->builder->CreateStore(init_val, alloca);
    
    // Remember variable and its type
    impl->named_values[stmt->name] = alloca;
    impl->named_types[stmt->name] = init_val->getType();
}

void LLVMCodeGen::generateIfStmt(IfStmt* stmt) {
    // Will be implemented in Phase 3
    (void)stmt;
    throw std::runtime_error("If statements not yet implemented - Phase 3");
}

void LLVMCodeGen::generateWhileStmt(WhileStmt* stmt) {
    // Will be implemented in Phase 3
    (void)stmt;
    throw std::runtime_error("While loops not yet implemented - Phase 3");
}

void LLVMCodeGen::generateForStmt(ForStmt* stmt) {
    // Will be implemented in Phase 3
    (void)stmt;
    throw std::runtime_error("For loops not yet implemented - Phase 3");
}

void LLVMCodeGen::writeObject(const std::string& filename) {
    (void)filename;
    throw std::runtime_error("Object file generation not yet implemented - Phase 5");
}

void LLVMCodeGen::writeExecutable(const std::string& filename) {
    (void)filename;
    throw std::runtime_error("Executable generation not yet implemented - Phase 5");
}

} // namespace sapphire

#else // !HAVE_LLVM

// Stub implementation when LLVM is not available
namespace sapphire {

LLVMCodeGen::LLVMCodeGen(const std::string& module_name) 
    : module_name_(module_name), impl_(nullptr), current_function_(nullptr) {
    std::cerr << "Warning: LLVM support not compiled in. Code generation disabled.\n";
    std::cerr << "Install LLVM development libraries and recompile.\n";
}

LLVMCodeGen::~LLVMCodeGen() = default;

void LLVMCodeGen::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    (void)statements;
    throw std::runtime_error("LLVM support not available. Install llvm-devel and recompile.");
}

void LLVMCodeGen::printIR() const {
    throw std::runtime_error("LLVM support not available.");
}

void LLVMCodeGen::writeIR(const std::string& filename) {
    (void)filename;
    throw std::runtime_error("LLVM support not available.");
}

void LLVMCodeGen::writeObject(const std::string& filename) {
    (void)filename;
    throw std::runtime_error("LLVM support not available.");
}

void LLVMCodeGen::writeExecutable(const std::string& filename) {
    (void)filename;
    throw std::runtime_error("LLVM support not available.");
}

void LLVMCodeGen::optimize(int level) {
    (void)level;
    throw std::runtime_error("LLVM support not available.");
}

} // namespace sapphire

#endif // HAVE_LLVM
