#ifndef SAPPHIRE_CODEGEN_IMPL_H
#define SAPPHIRE_CODEGEN_IMPL_H

#ifdef HAVE_LLVM

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <map>
#include <string>
#include <memory>

namespace sapphire {

/**
 * Internal implementation details for LLVM code generation
 * This is separated to avoid exposing LLVM headers in the public interface
 */
struct LLVMCodeGenImpl {
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    
    // Symbol table: variable name -> (alloca, type)
    std::map<std::string, llvm::Value*> named_values;
    std::map<std::string, llvm::Type*> named_types;
    
    // Function table: function name -> function
    std::map<std::string, llvm::Function*> functions;
    
    // Current function being generated
    llvm::Function* current_function;
    
    // Module name
    std::string module_name;
    
    LLVMCodeGenImpl(const std::string& name);
    ~LLVMCodeGenImpl();
    
    // Initialize LLVM
    void initialize();
    
    // Type helpers
    llvm::Type* getIntType();
    llvm::Type* getFloatType();
    llvm::Type* getBoolType();
    llvm::Type* getStringType();
    
    // Create main function
    void createMainFunction();
    
    // Finalize main function
    void finalizeMainFunction();
};

} // namespace sapphire

#endif // HAVE_LLVM

#endif // SAPPHIRE_CODEGEN_IMPL_H
