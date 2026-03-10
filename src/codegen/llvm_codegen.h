#ifndef SAPPHIRE_LLVM_CODEGEN_H
#define SAPPHIRE_LLVM_CODEGEN_H

#include <memory>
#include <string>
#include <map>
#include <vector>

// Forward declarations for LLVM types
namespace llvm {
    class Value;
    class Function;
}

namespace sapphire {

// Forward declarations
class Stmt;
class Expr;
class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class VariableExpr;
class AssignExpr;
class CallExpr;
class MatchExpr;
class VarDeclStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class FunctionDecl;
class ReturnStmt;
class ImportStmt;

/**
 * LLVM Code Generator
 * 
 * Transforms Sapphire AST into LLVM IR, which is then compiled to native machine code.
 */
class LLVMCodeGen {
public:
    LLVMCodeGen(const std::string& module_name);
    ~LLVMCodeGen();

    // Main entry point: generate code for a list of statements
    void generate(const std::vector<std::unique_ptr<Stmt>>& statements);
    
    // Output methods
    void printIR() const;                          // Print LLVM IR to stdout
    void writeIR(const std::string& filename);     // Write IR to file
    void writeObject(const std::string& filename); // Write object file
    void writeExecutable(const std::string& filename); // Write executable
    
    // Optimization
    void optimize(int level = 2); // 0=none, 1=basic, 2=default, 3=aggressive
    
private:
    // Expression generation
    llvm::Value* generateExpr(Expr* expr);
    llvm::Value* generateBinaryExpr(BinaryExpr* expr);
    llvm::Value* generateLogicalExpr(BinaryExpr* expr);
    llvm::Value* generateUnaryExpr(UnaryExpr* expr);
    llvm::Value* generateLiteralExpr(LiteralExpr* expr);
    llvm::Value* generateVariableExpr(VariableExpr* expr);
    llvm::Value* generateAssignExpr(AssignExpr* expr);
    llvm::Value* generateCallExpr(CallExpr* expr);
    llvm::Value* generateMatchExpr(MatchExpr* expr);
    
    // Statement generation
    void generateStmt(Stmt* stmt);
    void generateVarDeclStmt(VarDeclStmt* stmt);
    void generateIfStmt(IfStmt* stmt);
    void generateWhileStmt(WhileStmt* stmt);
    void generateForStmt(ForStmt* stmt);
    void generateFunctionDecl(FunctionDecl* stmt);
    void generateReturnStmt(ReturnStmt* stmt);
    void generateImportStmt(ImportStmt* stmt);
    
    // Built-in functions
    llvm::Value* generatePrintCall(CallExpr* expr);
    llvm::Function* getPrintfFunction();
    
    // Window built-in functions
    llvm::Value* generateWindowCreateCall(CallExpr* expr);
    llvm::Value* generateWindowShowCall(CallExpr* expr);
    llvm::Value* generateWindowPollEventsCall(CallExpr* expr);
    llvm::Value* generateWindowClearCall(CallExpr* expr);
    llvm::Value* generateWindowFillRectCall(CallExpr* expr);
    llvm::Value* generateWindowPresentCall(CallExpr* expr);
    llvm::Value* generateWindowIsKeyDownCall(CallExpr* expr);
    llvm::Value* generateWindowShouldCloseCall(CallExpr* expr);
    llvm::Value* generateWindowDestroyCall(CallExpr* expr);
    llvm::Value* generateDelayCall(CallExpr* expr);
    
    // OpenGL built-in functions
    llvm::Value* generateGLClearColorCall(CallExpr* expr);
    llvm::Value* generateGLClearCall(CallExpr* expr);
    llvm::Value* generateGLBeginCall(CallExpr* expr);
    llvm::Value* generateGLEndCall(CallExpr* expr);
    llvm::Value* generateGLVertex3fCall(CallExpr* expr);
    llvm::Value* generateGLColor3fCall(CallExpr* expr);
    llvm::Value* generateGLTranslatefCall(CallExpr* expr);
    llvm::Value* generateGLRotatefCall(CallExpr* expr);
    llvm::Value* generateGLMatrixModeCall(CallExpr* expr);
    llvm::Value* generateGLLoadIdentityCall(CallExpr* expr);
    
    // GLUT built-in functions
    llvm::Value* generateGLUTInitCall(CallExpr* expr);
    llvm::Value* generateGLUTInitDisplayModeCall(CallExpr* expr);
    llvm::Value* generateGLUTInitWindowSizeCall(CallExpr* expr);
    llvm::Value* generateGLUTCreateWindowCall(CallExpr* expr);
    llvm::Value* generateGLUTSwapBuffersCall(CallExpr* expr);
    llvm::Value* generateGLUTSolidSphereCall(CallExpr* expr);
    llvm::Value* generateGLUTWireSphereCall(CallExpr* expr);
    
    llvm::Function* getWindowCreateFunction();
    llvm::Function* getWindowShowFunction();
    llvm::Function* getWindowPollEventsFunction();
    llvm::Function* getWindowClearFunction();
    llvm::Function* getWindowFillRectFunction();
    llvm::Function* getWindowPresentFunction();
    llvm::Function* getWindowIsKeyDownFunction();
    llvm::Function* getWindowShouldCloseFunction();
    llvm::Function* getWindowDestroyFunction();
    llvm::Function* getDelayFunction();
    
    // OpenGL function getters
    llvm::Function* getGLClearColorFunction();
    llvm::Function* getGLClearFunction();
    llvm::Function* getGLBeginFunction();
    llvm::Function* getGLEndFunction();
    llvm::Function* getGLVertex3fFunction();
    llvm::Function* getGLColor3fFunction();
    llvm::Function* getGLTranslatefFunction();
    llvm::Function* getGLRotatefFunction();
    llvm::Function* getGLMatrixModeFunction();
    llvm::Function* getGLLoadIdentityFunction();
    
    // GLUT function getters
    llvm::Function* getGLUTInitDisplayModeFunction();
    llvm::Function* getGLUTInitWindowSizeFunction();
    llvm::Function* getGLUTCreateWindowFunction();
    llvm::Function* getGLUTSwapBuffersFunction();
    llvm::Function* getGLUTSolidSphereFunction();
    llvm::Function* getGLUTWireSphereFunction();
    
    // Module import helpers
    void declareSDL2Functions();
    void declareOpenGLFunctions();
    void declareGLUTFunctions();
    void declareVulkanFunctions();
    void declareSystemFunctions();
    
    // Module name
    std::string module_name_;
    
    // Opaque pointer to LLVM implementation details
    // This avoids exposing LLVM headers in our public interface
    void* impl_;
    
    // Current function being generated (opaque)
    void* current_function_;
};

} // namespace sapphire

#endif // SAPPHIRE_LLVM_CODEGEN_H
