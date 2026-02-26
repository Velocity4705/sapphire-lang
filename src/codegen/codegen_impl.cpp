#include "codegen_impl.h"
#include <iostream>

#ifdef HAVE_LLVM

namespace sapphire {

LLVMCodeGenImpl::LLVMCodeGenImpl(const std::string& name) 
    : module_name(name), current_function(nullptr) {
}

LLVMCodeGenImpl::~LLVMCodeGenImpl() = default;

void LLVMCodeGenImpl::initialize() {
    // Initialize LLVM
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    
    // Create LLVM context and module
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>(module_name, *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

llvm::Type* LLVMCodeGenImpl::getIntType() {
    return llvm::Type::getInt64Ty(*context);
}

llvm::Type* LLVMCodeGenImpl::getFloatType() {
    return llvm::Type::getDoubleTy(*context);
}

llvm::Type* LLVMCodeGenImpl::getBoolType() {
    return llvm::Type::getInt1Ty(*context);
}

llvm::Type* LLVMCodeGenImpl::getStringType() {
    return llvm::PointerType::get(*context, 0);
}

void LLVMCodeGenImpl::createMainFunction() {
    // Create main function: i32 @main()
    llvm::FunctionType* main_type = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context), false);
    
    current_function = llvm::Function::Create(
        main_type, 
        llvm::Function::ExternalLinkage, 
        "main", 
        module.get()
    );
    
    // Create entry basic block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(
        *context, "entry", current_function);
    builder->SetInsertPoint(entry);
}

void LLVMCodeGenImpl::finalizeMainFunction() {
    // Return 0 from main
    builder->CreateRet(llvm::ConstantInt::get(*context, llvm::APInt(32, 0)));
    
    // Verify the function
    std::string error;
    llvm::raw_string_ostream error_stream(error);
    if (llvm::verifyFunction(*current_function, &error_stream)) {
        throw std::runtime_error("Function verification failed: " + error);
    }
}

} // namespace sapphire

#endif // HAVE_LLVM
