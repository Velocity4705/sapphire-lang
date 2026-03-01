#include "interpreter.h"
#include "../error/exception.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

namespace sapphire {

// Environment implementation

void Environment::define(const std::string& name, const Value& value) {
    values[name] = value;
}

Value Environment::get(const std::string& name) {
    if (values.find(name) != values.end()) {
        return values[name];
    }
    
    if (enclosing != nullptr) {
        return enclosing->get(name);
    }
    
    throw RuntimeError("Undefined variable '" + name + "'");
}

void Environment::assign(const std::string& name, const Value& value) {
    if (values.find(name) != values.end()) {
        values[name] = value;
        return;
    }
    
    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }
    
    throw RuntimeError("Undefined variable '" + name + "'");
}

// Class implementation

std::shared_ptr<Function> Class::findMethod(const std::string& method_name) const {
    auto it = methods.find(method_name);
    if (it != methods.end()) {
        return it->second;
    }
    if (superclass) {
        return superclass->findMethod(method_name);
    }
    return nullptr;
}

// Interpreter implementation

Interpreter::Interpreter() {
    environment = std::make_shared<Environment>();
    
    // Built-in functions
    environment->define("print", std::string("__builtin_print__"));
    environment->define("range", std::string("__builtin_range__"));
    environment->define("len", std::string("__builtin_len__"));
    environment->define("input", std::string("__builtin_input__"));
    environment->define("read_file", std::string("__builtin_read_file__"));
    environment->define("write_file", std::string("__builtin_write_file__"));
}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>& statements) {
    try {
        for (auto& stmt : statements) {
            stmt->accept(*this);
        }
    } catch (const ReturnException& e) {
        // Top-level return is ignored
        (void)e;
    }
}

void Interpreter::throwException(const std::string& type, const std::string& message) {
    std::string full_message = message;
    if (!type.empty() && !message.empty()) {
        full_message = type + ": " + message;
    } else if (!type.empty()) {
        full_message = type;
    }
    
    if (type == "DivisionByZeroError") {
        throw DivisionByZeroError(full_message);
    } else if (type == "FileNotFoundError") {
        throw FileNotFoundError(full_message);
    } else if (type == "IndexError") {
        throw IndexError(full_message);
    } else if (type == "TypeError") {
        throw TypeError(full_message);
    } else if (type == "ValueError") {
        throw ValueError(full_message);
    } else if (type == "PermissionError") {
        throw PermissionError(full_message);
    } else if (type == "RuntimeError" || type.empty()) {
        throw RuntimeError(full_message);
    } else {
        throw SapphireException(full_message, type);
    }
}

std::string Interpreter::valueToString(const Value& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::nullptr_t>(value)) {
        return "none";
    } else if (std::holds_alternative<std::shared_ptr<Function>>(value)) {
        auto func = std::get<std::shared_ptr<Function>>(value);
        return "<function " + func->name + ">";
    } else if (std::holds_alternative<std::shared_ptr<Class>>(value)) {
        auto cls = std::get<std::shared_ptr<Class>>(value);
        return "<class " + cls->name + ">";
    } else if (std::holds_alternative<std::shared_ptr<Instance>>(value)) {
        auto inst = std::get<std::shared_ptr<Instance>>(value);
        return "<" + inst->klass->name + " instance>";
    } else if (std::holds_alternative<std::shared_ptr<BoundMethod>>(value)) {
        auto bm = std::get<std::shared_ptr<BoundMethod>>(value);
        return "<bound method " + bm->method->name + " of " + bm->instance->klass->name + ">";
    } else if (std::holds_alternative<std::shared_ptr<ArrayValue>>(value)) {
        auto array = std::get<std::shared_ptr<ArrayValue>>(value);
        std::string result = "[";
        for (size_t i = 0; i < array->elements.size(); i++) {
            result += valueToString(array->elements[i]);
            if (i < array->elements.size() - 1) {
                result += ", ";
            }
        }
        result += "]";
        return result;
    }
    return "unknown";
}

bool Interpreter::isTruthy(const Value& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::nullptr_t>(value)) {
        return false;
    } else if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) != 0;
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        return !std::get<std::string>(value).empty();
    } else if (std::holds_alternative<std::shared_ptr<Instance>>(value)) {
        return true;
    } else if (std::holds_alternative<std::shared_ptr<Class>>(value)) {
        return true;
    } else if (std::holds_alternative<std::shared_ptr<BoundMethod>>(value)) {
        return true;
    }
    return true;
}

Value Interpreter::evaluateExpr(Expr& expr) {
    expr.accept(*this);
    return last_value;
}

// Expression visitors

void Interpreter::visitLiteralExpr(LiteralExpr& expr) {
    switch (expr.type) {
        case LiteralExpr::Type::INTEGER:
            last_value = std::stoi(expr.value);
            break;
        case LiteralExpr::Type::FLOAT:
            last_value = std::stod(expr.value);
            break;
        case LiteralExpr::Type::STRING:
            last_value = expr.value;
            break;
        case LiteralExpr::Type::BOOLEAN:
            last_value = (expr.value == "true");
            break;
        case LiteralExpr::Type::NONE:
            last_value = nullptr;
            break;
    }
}

void Interpreter::visitVariableExpr(VariableExpr& expr) {
    last_value = environment->get(expr.name);
}

void Interpreter::visitAssignExpr(AssignExpr& expr) {
    Value value = evaluateExpr(*expr.value);
    // In Python-like languages, assignment creates the variable if it doesn't exist
    environment->define(expr.name, value);
    last_value = value;
}

void Interpreter::visitBinaryExpr(BinaryExpr& expr) {
    Value left = evaluateExpr(*expr.left);
    Value right = evaluateExpr(*expr.right);
    
    // Arithmetic operations
    if (expr.op == "+") {
        if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
            last_value = std::get<int>(left) + std::get<int>(right);
        } else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
            last_value = std::get<std::string>(left) + std::get<std::string>(right);
        } else {
            double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
            double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
            last_value = l + r;
        }
    } else if (expr.op == "-") {
        if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
            last_value = std::get<int>(left) - std::get<int>(right);
        } else {
            double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
            double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
            last_value = l - r;
        }
    } else if (expr.op == "*") {
        if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
            last_value = std::get<int>(left) * std::get<int>(right);
        } else {
            double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
            double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
            last_value = l * r;
        }
    } else if (expr.op == "/") {
        double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
        double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
        if (r == 0.0) {
            throw DivisionByZeroError("Division by zero");
        }
        last_value = l / r;
    } else if (expr.op == "%") {
        int r = std::get<int>(right);
        if (r == 0) {
            throw DivisionByZeroError("Modulo by zero");
        }
        last_value = std::get<int>(left) % r;
    } else if (expr.op == "**") {
        double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
        double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
        last_value = std::pow(l, r);
    }
    // Comparison operations
    else if (expr.op == "==") {
        last_value = (left == right);
    } else if (expr.op == "!=") {
        last_value = (left != right);
    } else if (expr.op == "<") {
        if (std::holds_alternative<int>(left)) {
            last_value = std::get<int>(left) < std::get<int>(right);
        } else {
            last_value = std::get<double>(left) < std::get<double>(right);
        }
    } else if (expr.op == "<=") {
        if (std::holds_alternative<int>(left)) {
            last_value = std::get<int>(left) <= std::get<int>(right);
        } else {
            last_value = std::get<double>(left) <= std::get<double>(right);
        }
    } else if (expr.op == ">") {
        if (std::holds_alternative<int>(left)) {
            last_value = std::get<int>(left) > std::get<int>(right);
        } else {
            last_value = std::get<double>(left) > std::get<double>(right);
        }
    } else if (expr.op == ">=") {
        if (std::holds_alternative<int>(left)) {
            last_value = std::get<int>(left) >= std::get<int>(right);
        } else {
            last_value = std::get<double>(left) >= std::get<double>(right);
        }
    }
    // Logical operations
    else if (expr.op == "and") {
        last_value = isTruthy(left) && isTruthy(right);
    } else if (expr.op == "or") {
        last_value = isTruthy(left) || isTruthy(right);
    }
}

void Interpreter::visitUnaryExpr(UnaryExpr& expr) {
    Value operand = evaluateExpr(*expr.operand);
    
    if (expr.op == "-") {
        if (std::holds_alternative<int>(operand)) {
            last_value = -std::get<int>(operand);
        } else {
            last_value = -std::get<double>(operand);
        }
    } else if (expr.op == "not") {
        last_value = !isTruthy(operand);
    }
}

void Interpreter::visitCallExpr(CallExpr& expr) {
    Value callee = evaluateExpr(*expr.callee);
    
    // Evaluate arguments
    std::vector<Value> arguments;
    for (auto& arg : expr.arguments) {
        arguments.push_back(evaluateExpr(*arg));
    }
    
    // Handle user-defined functions
    if (std::holds_alternative<std::shared_ptr<Function>>(callee)) {
        auto func = std::get<std::shared_ptr<Function>>(callee);
        
        // Check argument count
        if (arguments.size() != func->parameters.size()) {
            throw TypeError("Function '" + func->name + "' expects " + 
                            std::to_string(func->parameters.size()) + 
                            " arguments but got " + 
                            std::to_string(arguments.size()));
        }
        
        // Create new environment for function execution
        auto func_env = std::make_shared<Environment>(func->closure);
        
        // Bind parameters to arguments
        for (size_t i = 0; i < func->parameters.size(); i++) {
            func_env->define(func->parameters[i].first, arguments[i]);
        }
        
        // Save current environment
        auto previous_env = environment;
        environment = func_env;
        
        // Execute function body
        try {
            for (auto& stmt : *func->body) {
                stmt->accept(*this);
            }
            // If no return statement, return null
            last_value = nullptr;
        } catch (const ReturnException& ret) {
            // Function returned a value
            last_value = ret.value;
        }
        
        // Restore previous environment
        environment = previous_env;
        return;
    }
    
    // Handle bound methods
    if (std::holds_alternative<std::shared_ptr<BoundMethod>>(callee)) {
        auto bm = std::get<std::shared_ptr<BoundMethod>>(callee);
        auto func = bm->method;
        
        if (func->parameters.empty()) {
            throw TypeError("Method '" + func->name + "' must have at least a 'self' parameter");
        }
        
        // Expected arguments: all parameters except 'self'
        if (arguments.size() != func->parameters.size() - 1) {
            throw TypeError("Method '" + func->name + "' expects " + 
                            std::to_string(func->parameters.size() - 1) + 
                            " arguments but got " + 
                            std::to_string(arguments.size()));
        }
        
        auto func_env = std::make_shared<Environment>(func->closure);
        
        // Bind self
        func_env->define(func->parameters[0].first, bm->instance);
        // Bind remaining parameters
        for (size_t i = 1; i < func->parameters.size(); i++) {
            func_env->define(func->parameters[i].first, arguments[i - 1]);
        }
        
        auto previous_env = environment;
        environment = func_env;
        
        try {
            for (auto& stmt : *func->body) {
                stmt->accept(*this);
            }
            last_value = nullptr;
        } catch (const ReturnException& ret) {
            last_value = ret.value;
        }
        
        environment = previous_env;
        return;
    }
    
    // Handle class constructors
    if (std::holds_alternative<std::shared_ptr<Class>>(callee)) {
        auto cls = std::get<std::shared_ptr<Class>>(callee);
        auto instance = std::make_shared<Instance>(cls);
        
        // Look for __init__ method
        auto init = cls->findMethod("__init__");
        if (init) {
            if (init->parameters.empty()) {
                throw TypeError("Method '__init__' must have at least a 'self' parameter");
            }
            
            if (arguments.size() != init->parameters.size() - 1) {
                throw TypeError("Constructor for class '" + cls->name + "' expects " +
                                std::to_string(init->parameters.size() - 1) +
                                " arguments but got " +
                                std::to_string(arguments.size()));
            }
            
            auto func_env = std::make_shared<Environment>(init->closure);
            // Bind self
            func_env->define(init->parameters[0].first, instance);
            // Bind remaining parameters
            for (size_t i = 1; i < init->parameters.size(); i++) {
                func_env->define(init->parameters[i].first, arguments[i - 1]);
            }
            
            auto previous_env = environment;
            environment = func_env;
            
            try {
                for (auto& stmt : *init->body) {
                    stmt->accept(*this);
                }
            } catch (const ReturnException&) {
                // Ignore return value from __init__
            }
            
            environment = previous_env;
        }
        
        last_value = instance;
        return;
    }
    
    // Handle built-in functions
    if (std::holds_alternative<std::string>(callee)) {
        std::string func_name = std::get<std::string>(callee);
        
        if (func_name == "__builtin_print__") {
            for (size_t i = 0; i < arguments.size(); i++) {
                std::cout << valueToString(arguments[i]);
                if (i < arguments.size() - 1) {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
            last_value = nullptr;
        } else if (func_name == "__builtin_range__") {
            // Simple range implementation
            int end = std::get<int>(arguments[0]);
            last_value = end;
        } else if (func_name == "__builtin_len__") {
            // Get length of array or string
            if (arguments.empty()) {
                throw TypeError("len() requires 1 argument");
            }
            
            Value arg = arguments[0];
            if (std::holds_alternative<std::shared_ptr<ArrayValue>>(arg)) {
                auto array = std::get<std::shared_ptr<ArrayValue>>(arg);
                last_value = static_cast<int>(array->elements.size());
            } else if (std::holds_alternative<std::string>(arg)) {
                std::string str = std::get<std::string>(arg);
                last_value = static_cast<int>(str.length());
            } else {
                throw TypeError("len() requires an array or string");
            }
        } else if (func_name == "__builtin_input__") {
            // Get user input
            // Optional prompt argument
            if (!arguments.empty()) {
                std::cout << valueToString(arguments[0]);
            }
            
            std::string input_line;
            std::getline(std::cin, input_line);
            last_value = input_line;
        } else if (func_name == "__builtin_read_file__") {
            // Read file contents
            if (arguments.empty()) {
                throw TypeError("read_file() requires 1 argument (filename)");
            }
            
            std::string filename = std::get<std::string>(arguments[0]);
            std::ifstream file(filename);
            
            if (!file.is_open()) {
                throw FileNotFoundError("Failed to open file: " + filename);
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            
            last_value = buffer.str();
        } else if (func_name == "__builtin_write_file__") {
            // Write content to file
            if (arguments.size() < 2) {
                throw TypeError("write_file() requires 2 arguments (filename, content)");
            }
            
            std::string filename = std::get<std::string>(arguments[0]);
            std::string content = valueToString(arguments[1]);
            
            std::ofstream file(filename);
            
            if (!file.is_open()) {
                throw PermissionError("Failed to open file for writing: " + filename);
            }
            
            file << content;
            file.close();
            
            last_value = true;  // Return true on success
        }
    }
}

void Interpreter::visitListExpr(ListExpr& expr) {
    auto array = std::make_shared<ArrayValue>();
    
    for (auto& element : expr.elements) {
        array->elements.push_back(evaluateExpr(*element));
    }
    
    last_value = array;
}

void Interpreter::visitIndexExpr(IndexExpr& expr) {
    Value object = evaluateExpr(*expr.object);
    Value index = evaluateExpr(*expr.index);
    
    if (std::holds_alternative<std::shared_ptr<ArrayValue>>(object)) {
        auto array = std::get<std::shared_ptr<ArrayValue>>(object);
        
        if (!std::holds_alternative<int>(index)) {
            throw TypeError("Array index must be an integer");
        }
        
        int idx = std::get<int>(index);
        
        if (idx < 0 || idx >= static_cast<int>(array->elements.size())) {
            throw IndexError("Array index out of bounds: " + std::to_string(idx));
        }
        
        last_value = array->elements[idx];
    } else if (std::holds_alternative<std::string>(object)) {
        // String indexing
        std::string str = std::get<std::string>(object);
        
        if (!std::holds_alternative<int>(index)) {
            throw TypeError("String index must be an integer");
        }
        
        int idx = std::get<int>(index);
        
        if (idx < 0 || idx >= static_cast<int>(str.length())) {
            throw IndexError("String index out of bounds: " + std::to_string(idx));
        }
        
        last_value = std::string(1, str[idx]);
    } else {
        throw TypeError("Cannot index non-array/non-string type");
    }
}

void Interpreter::visitGetExpr(GetExpr& expr) {
    Value object = evaluateExpr(*expr.object);
    
    if (std::holds_alternative<std::shared_ptr<Instance>>(object)) {
        auto instance = std::get<std::shared_ptr<Instance>>(object);
        
        // Check instance fields first
        auto it = instance->fields.find(expr.name);
        if (it != instance->fields.end()) {
            last_value = it->second;
            return;
        }
        
        // Then check methods on the class (and superclasses)
        auto method = instance->klass->findMethod(expr.name);
        if (method) {
            last_value = std::make_shared<BoundMethod>(instance, method);
            return;
        }
        
        throw RuntimeError("Undefined property '" + expr.name + "' on instance of '" + instance->klass->name + "'");
    }
    
    throw TypeError("Only instances have properties");
}

void Interpreter::visitSetExpr(SetExpr& expr) {
    Value object = evaluateExpr(*expr.object);
    Value value = evaluateExpr(*expr.value);
    
    if (std::holds_alternative<std::shared_ptr<Instance>>(object)) {
        auto instance = std::get<std::shared_ptr<Instance>>(object);
        instance->fields[expr.name] = value;
        last_value = value;
        return;
    }
    
    throw TypeError("Only instances have settable properties");
}

// Statement visitors

void Interpreter::visitExprStmt(ExprStmt& stmt) {
    evaluateExpr(*stmt.expression);
}

void Interpreter::visitVarDeclStmt(VarDeclStmt& stmt) {
    Value value = nullptr;
    if (stmt.initializer) {
        value = evaluateExpr(*stmt.initializer);
    }
    environment->define(stmt.name, value);
}

void Interpreter::visitFunctionDecl(FunctionDecl& stmt) {
    // Create a function object and store it in the environment
    auto func = std::make_shared<Function>(
        stmt.name,
        stmt.parameters,
        stmt.return_type,
        &stmt.body,
        environment  // Capture current environment as closure
    );
    
    environment->define(stmt.name, func);
}

void Interpreter::visitReturnStmt(ReturnStmt& stmt) {
    Value value = nullptr;
    if (stmt.value) {
        value = evaluateExpr(*stmt.value);
    }
    throw ReturnException(value);
}

void Interpreter::visitIfStmt(IfStmt& stmt) {
    Value condition = evaluateExpr(*stmt.condition);
    
    if (isTruthy(condition)) {
        for (auto& s : stmt.then_branch) {
            s->accept(*this);
        }
    } else if (!stmt.else_branch.empty()) {
        for (auto& s : stmt.else_branch) {
            s->accept(*this);
        }
    }
}

void Interpreter::visitWhileStmt(WhileStmt& stmt) {
    while (isTruthy(evaluateExpr(*stmt.condition))) {
        for (auto& s : stmt.body) {
            s->accept(*this);
        }
    }
}

void Interpreter::visitForStmt(ForStmt& stmt) {
    Value iterable = evaluateExpr(*stmt.iterable);
    
    // Simplified: assume iterable is an integer (range)
    if (std::holds_alternative<int>(iterable)) {
        int end = std::get<int>(iterable);
        for (int i = 0; i < end; i++) {
            environment->define(stmt.variable, i);
            for (auto& s : stmt.body) {
                s->accept(*this);
            }
        }
    }
}

void Interpreter::visitTryStmt(TryStmt& stmt) {
    std::exception_ptr pending_exception;
    
    try {
        for (auto& s : stmt.try_body) {
            s->accept(*this);
        }
    } catch (const SapphireException& ex) {
        bool handled = false;
        
        for (auto& clause : stmt.catch_clauses) {
            if (!clause.exception_type.empty() &&
                clause.exception_type != ex.getTypeName()) {
                continue;
            }
            
            // Bind exception variable if present
            if (!clause.variable_name.empty()) {
                std::string value = ex.getTypeName() + ": " + ex.getMessage();
                environment->define(clause.variable_name, value);
            }
            
            for (auto& s : clause.body) {
                s->accept(*this);
            }
            
            handled = true;
            break;
        }
        
        if (!handled) {
            pending_exception = std::current_exception();
        }
    } catch (const ReturnException&) {
        pending_exception = std::current_exception();
    }
    
    // Finally block always runs
    if (!stmt.finally_body.empty()) {
        for (auto& s : stmt.finally_body) {
            s->accept(*this);
        }
    }
    
    // Rethrow if not handled
    if (pending_exception) {
        std::rethrow_exception(pending_exception);
    }
}

void Interpreter::visitThrowStmt(ThrowStmt& stmt) {
    std::string message;
    if (stmt.message) {
        Value value = evaluateExpr(*stmt.message);
        message = valueToString(value);
    }
    
    throwException(stmt.exception_type, message);
}

void Interpreter::visitClassDecl(ClassDecl& stmt) {
    std::shared_ptr<Class> superclass = nullptr;
    
    if (!stmt.superclass_name.empty()) {
        Value value = environment->get(stmt.superclass_name);
        if (!std::holds_alternative<std::shared_ptr<Class>>(value)) {
            throw TypeError("Superclass '" + stmt.superclass_name + "' is not a class");
        }
        superclass = std::get<std::shared_ptr<Class>>(value);
    }
    
    // Map methods by name
    std::map<std::string, std::shared_ptr<Function>> methods;
    for (auto& method : stmt.methods) {
        auto func = std::make_shared<Function>(
            method->name,
            method->parameters,
            method->return_type,
            &method->body,
            environment  // methods share the current environment as closure
        );
        methods[method->name] = func;
    }
    
    auto cls = std::make_shared<Class>(stmt.name, superclass, methods);
    environment->define(stmt.name, cls);
}

void Interpreter::visitImportStmt(ImportStmt& stmt) {
    // Simple implementation for now - just print what would be imported
    // In a real implementation, this would:
    // 1. Resolve module path
    // 2. Load and parse module file
    // 3. Execute module in its own environment
    // 4. Import symbols into current environment
    
    if (stmt.is_from_import) {
        // from module import name1, name2
        // For now, just define empty values for imported names
        for (const auto& name : stmt.imported_names) {
            environment->define(name, Value{});
        }
    } else {
        // import module
        // import module as alias
        std::string import_name = stmt.alias.empty() ? stmt.module_name : stmt.alias;
        
        // Create a simple module object (empty for now)
        // In a real implementation, this would be a proper module object
        // with the module's exported symbols
        environment->define(import_name, Value{});
    }
}

} // namespace sapphire