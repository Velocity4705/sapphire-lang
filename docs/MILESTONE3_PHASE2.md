# Milestone 3 - Phase 2: Expression Code Generation

## Status
- Phase 1: ✅ COMPLETE - LLVM Integration structure created
- Phase 2: 🚀 STARTING - Expression code generation

## Goal
Implement complete LLVM IR generation for all Sapphire expression types, enabling compilation of arithmetic, logical, and comparison operations.

## Timeline
Weeks 3-4 (2 weeks)

## Objectives

### Week 3: Basic Expressions
1. Literal expressions (integers, floats, booleans, strings)
2. Binary arithmetic operations (+, -, *, /, %)
3. Comparison operations (<, >, <=, >=, ==, !=)
4. Variable references

### Week 4: Advanced Expressions
1. Logical operations (and, or, not)
2. Unary operations (-, not)
3. Type conversions (int ↔ float, bool ↔ int)
4. Assignment expressions

## Implementation Plan

### Step 1: Update Code Generator Header

Add actual LLVM includes and member functions:

```cpp
#ifdef HAVE_LLVM
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>

namespace sapphire {

class LLVMCodeGenImpl {
public:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::map<std::string, llvm::Value*> named_values;
    llvm::Function* current_function;
};

} // namespace sapphire
#endif
```

### Step 2: Implement Literal Generation

```cpp
llvm::Value* generateLiteral(LiteralExpr* expr) {
    if (expr->value.type == TokenType::INTEGER) {
        int64_t val = std::stoll(expr->value.lexeme);
        return llvm::ConstantInt::get(impl_->context, llvm::APInt(64, val, true));
    }
    else if (expr->value.type == TokenType::FLOAT) {
        double val = std::stod(expr->value.lexeme);
        return llvm::ConstantFP::get(impl_->context, llvm::APFloat(val));
    }
    else if (expr->value.type == TokenType::TRUE_KW) {
        return llvm::ConstantInt::get(impl_->context, llvm::APInt(1, 1));
    }
    else if (expr->value.type == TokenType::FALSE_KW) {
        return llvm::ConstantInt::get(impl_->context, llvm::APInt(1, 0));
    }
    throw std::runtime_error("Unknown literal type");
}
```

### Step 3: Implement Binary Operations

```cpp
llvm::Value* generateBinaryOp(BinaryExpr* expr) {
    llvm::Value* left = generateExpr(expr->left.get());
    llvm::Value* right = generateExpr(expr->right.get());
    
    // Type promotion if needed
    if (left->getType() != right->getType()) {
        // Promote to larger type
        if (left->getType()->isIntegerTy() && right->getType()->isDoubleTy()) {
            left = impl_->builder->CreateSIToFP(left, right->getType());
        }
        else if (left->getType()->isDoubleTy() && right->getType()->isIntegerTy()) {
            right = impl_->builder->CreateSIToFP(right, left->getType());
        }
    }
    
    // Generate operation based on type
    if (left->getType()->isIntegerTy()) {
        switch (expr->op.type) {
            case TokenType::PLUS:  return impl_->builder->CreateAdd(left, right, "addtmp");
            case TokenType::MINUS: return impl_->builder->CreateSub(left, right, "subtmp");
            case TokenType::STAR:  return impl_->builder->CreateMul(left, right, "multmp");
            case TokenType::SLASH: return impl_->builder->CreateSDiv(left, right, "divtmp");
            case TokenType::PERCENT: return impl_->builder->CreateSRem(left, right, "modtmp");
            case TokenType::LESS:  return impl_->builder->CreateICmpSLT(left, right, "cmptmp");
            case TokenType::GREATER: return impl_->builder->CreateICmpSGT(left, right, "cmptmp");
            case TokenType::LESS_EQUAL: return impl_->builder->CreateICmpSLE(left, right, "cmptmp");
            case TokenType::GREATER_EQUAL: return impl_->builder->CreateICmpSGE(left, right, "cmptmp");
            case TokenType::EQUAL_EQUAL: return impl_->builder->CreateICmpEQ(left, right, "cmptmp");
            case TokenType::BANG_EQUAL: return impl_->builder->CreateICmpNE(left, right, "cmptmp");
        }
    }
    else if (left->getType()->isDoubleTy()) {
        switch (expr->op.type) {
            case TokenType::PLUS:  return impl_->builder->CreateFAdd(left, right, "addtmp");
            case TokenType::MINUS: return impl_->builder->CreateFSub(left, right, "subtmp");
            case TokenType::STAR:  return impl_->builder->CreateFMul(left, right, "multmp");
            case TokenType::SLASH: return impl_->builder->CreateFDiv(left, right, "divtmp");
            case TokenType::LESS:  return impl_->builder->CreateFCmpOLT(left, right, "cmptmp");
            case TokenType::GREATER: return impl_->builder->CreateFCmpOGT(left, right, "cmptmp");
            case TokenType::LESS_EQUAL: return impl_->builder->CreateFCmpOLE(left, right, "cmptmp");
            case TokenType::GREATER_EQUAL: return impl_->builder->CreateFCmpOGE(left, right, "cmptmp");
            case TokenType::EQUAL_EQUAL: return impl_->builder->CreateFCmpOEQ(left, right, "cmptmp");
            case TokenType::BANG_EQUAL: return impl_->builder->CreateFCmpONE(left, right, "cmptmp");
        }
    }
    
    throw std::runtime_error("Unknown binary operator or type");
}
```

### Step 4: Implement Variable References

```cpp
llvm::Value* generateVariable(VariableExpr* expr) {
    llvm::Value* var = impl_->named_values[expr->name.lexeme];
    if (!var) {
        throw std::runtime_error("Unknown variable: " + expr->name.lexeme);
    }
    
    // Load value from memory
    return impl_->builder->CreateLoad(
        var->getType()->getPointerElementType(),
        var,
        expr->name.lexeme
    );
}
```

### Step 5: Implement Logical Operations

```cpp
llvm::Value* generateLogicalOp(BinaryExpr* expr) {
    if (expr->op.type == TokenType::AND_KW) {
        // Short-circuit evaluation
        llvm::Value* left = generateExpr(expr->left.get());
        
        llvm::BasicBlock* right_bb = llvm::BasicBlock::Create(*impl_->context, "and_right");
        llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*impl_->context, "and_merge");
        
        impl_->builder->CreateCondBr(left, right_bb, merge_bb);
        
        // Right side
        impl_->current_function->insert(impl_->current_function->end(), right_bb);
        impl_->builder->SetInsertPoint(right_bb);
        llvm::Value* right = generateExpr(expr->right.get());
        impl_->builder->CreateBr(merge_bb);
        
        // Merge
        impl_->current_function->insert(impl_->current_function->end(), merge_bb);
        impl_->builder->SetInsertPoint(merge_bb);
        
        llvm::PHINode* phi = impl_->builder->CreatePHI(llvm::Type::getInt1Ty(*impl_->context), 2);
        phi->addIncoming(left, left_bb);
        phi->addIncoming(right, right_bb);
        return phi;
    }
    else if (expr->op.type == TokenType::OR_KW) {
        // Similar short-circuit for OR
        // ...
    }
}
```

## Test Programs

### Test 1: Arithmetic
```sapphire
let a = 10
let b = 20
let sum = a + b
let diff = a - b
let prod = a * b
let quot = a / b
print(sum)   # 30
print(diff)  # -10
print(prod)  # 200
print(quot)  # 0
```

### Test 2: Comparisons
```sapphire
let x = 15
let y = 10
print(x > y)   # true
print(x < y)   # false
print(x == y)  # false
print(x != y)  # true
```

### Test 3: Mixed Types
```sapphire
let i = 10
let f = 3.14
let result = i + f  # Should promote i to float
print(result)  # 13.14
```

### Test 4: Logical Operations
```sapphire
let a = true
let b = false
print(a and b)  # false
print(a or b)   # true
print(not a)    # false
```

## Type Mapping

| Sapphire Type | LLVM Type | Size |
|---------------|-----------|------|
| int | i64 | 64-bit |
| float | double | 64-bit |
| bool | i1 | 1-bit |
| string | i8* | pointer |
| none | void | - |

## Success Criteria

### Must Have
- ✅ Generate IR for all literal types
- ✅ Generate IR for arithmetic operations
- ✅ Generate IR for comparison operations
- ✅ Generate IR for variable references
- ✅ Type promotion works correctly
- ✅ Generated IR is valid (passes verification)

### Should Have
- ✅ Logical operations with short-circuit evaluation
- ✅ Unary operations (-, not)
- ✅ Assignment expressions
- ✅ Reasonable IR quality (not too verbose)

### Nice to Have
- ✅ Constant folding at IR level
- ✅ Dead code elimination hints
- ✅ Optimization-friendly IR patterns

## Testing Strategy

### Unit Tests
```cpp
TEST(CodeGen, LiteralInteger) {
    LLVMCodeGen codegen("test");
    // Test integer literal generation
}

TEST(CodeGen, BinaryAdd) {
    LLVMCodeGen codegen("test");
    // Test addition operation
}

TEST(CodeGen, TypePromotion) {
    LLVMCodeGen codegen("test");
    // Test int + float promotion
}
```

### Integration Tests
```bash
# Compile and verify IR
./sapp compile test.spp --emit-llvm -o test.ll
llvm-as test.ll -o test.bc
llvm-dis test.bc  # Should match original

# Verify IR is valid
opt -verify test.ll
```

### Performance Tests
```bash
# Compare with C equivalent
gcc -O2 test.c -o test_c
./sapp compile -O2 test.spp -o test_spp

time ./test_c
time ./test_spp

# Should be within 2x of C
```

## Deliverables

1. **Updated llvm_codegen.cpp** with full expression generation
2. **Test suite** for expression code generation
3. **Example programs** demonstrating all expression types
4. **Documentation** of IR generation patterns
5. **Performance benchmarks** vs C

## Next Phase

After Phase 2 is complete, Phase 3 will add:
- Statement code generation (if/while/for)
- Control flow graphs
- Basic blocks and branching
- Function definitions

## Timeline

| Day | Task | Deliverable |
|-----|------|-------------|
| 1-2 | Literal generation | Integer, float, bool literals work |
| 3-4 | Arithmetic operations | +, -, *, /, % work |
| 5-6 | Comparison operations | <, >, ==, != work |
| 7-8 | Variable references | Can load variables |
| 9-10 | Logical operations | and, or, not work |
| 11-12 | Type conversions | int ↔ float works |
| 13-14 | Testing & polish | All tests pass |

**Total:** 14 days (2 weeks)

## Resources

- [LLVM IR Tutorial](https://llvm.org/docs/tutorial/)
- [LLVM Language Reference](https://llvm.org/docs/LangRef.html)
- [IRBuilder Documentation](https://llvm.org/doxygen/classllvm_1_1IRBuilder.html)
- [Kaleidoscope Chapter 3](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl03.html)

---

**Let's generate some IR!** 🚀
