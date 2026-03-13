#include "interpreter.h"
#include "../error/exception.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstdlib>

// Milestone 1 Libraries
#include "../stdlib/collections/array.h"
#include "../stdlib/math/math.h"
#include "../stdlib/io/csv.h"

// Milestone 2 Libraries
#include "../stdlib/json/json.h"
#include "../stdlib/random/random.h"

// Milestone 3 Libraries
#include "../stdlib/plotting/plotting.h"

// Milestone 4 Libraries
#include "../stdlib/http/http.h"
#include "../stdlib/websocket/websocket.h"
#include "../stdlib/template/template.h"
#include "../stdlib/database/database.h"

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
    environment->define("time", std::string("__builtin_time__"));
    environment->define("sleep", std::string("__builtin_sleep__"));
    environment->define("sort", std::string("__builtin_sort__"));
    environment->define("make_array", std::string("__builtin_make_array__"));
    environment->define("WaitGroup", std::string("__builtin_waitgroup__"));
    
    // Milestone 1: Dynamic Arrays
    environment->define("array_create", std::string("__builtin_array_create__"));
    environment->define("array_push", std::string("__builtin_array_push__"));
    environment->define("array_get", std::string("__builtin_array_get__"));
    environment->define("array_set", std::string("__builtin_array_set__"));
    environment->define("array_pop", std::string("__builtin_array_pop__"));
    environment->define("array_size", std::string("__builtin_array_size__"));
    environment->define("array_clear", std::string("__builtin_array_clear__"));
    
    // Milestone 1: Math Library
    environment->define("math_sqrt", std::string("__builtin_math_sqrt__"));
    environment->define("math_pow", std::string("__builtin_math_pow__"));
    environment->define("math_sin", std::string("__builtin_math_sin__"));
    environment->define("math_cos", std::string("__builtin_math_cos__"));
    environment->define("math_tan", std::string("__builtin_math_tan__"));
    environment->define("math_exp", std::string("__builtin_math_exp__"));
    environment->define("math_log", std::string("__builtin_math_log__"));
    environment->define("math_log10", std::string("__builtin_math_log10__"));
    environment->define("math_abs", std::string("__builtin_math_abs__"));
    environment->define("math_floor", std::string("__builtin_math_floor__"));
    environment->define("math_ceil", std::string("__builtin_math_ceil__"));
    environment->define("math_round", std::string("__builtin_math_round__"));
    environment->define("math_pi", std::string("__builtin_math_pi__"));
    environment->define("math_e", std::string("__builtin_math_e__"));
    
    // Milestone 1: CSV I/O
    environment->define("csv_create", std::string("__builtin_csv_create__"));
    environment->define("csv_read_file", std::string("__builtin_csv_read_file__"));
    environment->define("csv_write_file", std::string("__builtin_csv_write_file__"));
    environment->define("csv_get", std::string("__builtin_csv_get__"));
    environment->define("csv_set", std::string("__builtin_csv_set__"));
    environment->define("csv_row_count", std::string("__builtin_csv_row_count__"));
    
    // Milestone 2: JSON Support
    environment->define("json_parse", std::string("__builtin_json_parse__"));
    environment->define("json_stringify", std::string("__builtin_json_stringify__"));
    environment->define("json_create_object", std::string("__builtin_json_create_object__"));
    environment->define("json_create_array", std::string("__builtin_json_create_array__"));
    environment->define("json_get", std::string("__builtin_json_get__"));
    environment->define("json_set", std::string("__builtin_json_set__"));
    environment->define("json_push", std::string("__builtin_json_push__"));
    environment->define("json_size", std::string("__builtin_json_size__"));
    
    // Milestone 2: Random Numbers
    environment->define("random_seed", std::string("__builtin_random_seed__"));
    environment->define("random", std::string("__builtin_random__"));
    environment->define("random_int", std::string("__builtin_random_int__"));
    environment->define("random_normal", std::string("__builtin_random_normal__"));
    environment->define("random_boolean", std::string("__builtin_random_boolean__"));
    environment->define("crypto_random", std::string("__builtin_crypto_random__"));
    environment->define("crypto_random_int", std::string("__builtin_crypto_random_int__"));
    
    // Milestone 3: Data Visualization
    environment->define("plot_create", std::string("__builtin_plot_create__"));
    environment->define("plot_add_line", std::string("__builtin_plot_add_line__"));
    environment->define("plot_add_scatter", std::string("__builtin_plot_add_scatter__"));
    environment->define("plot_add_bar", std::string("__builtin_plot_add_bar__"));
    environment->define("plot_add_histogram", std::string("__builtin_plot_add_histogram__"));
    environment->define("plot_set_title", std::string("__builtin_plot_set_title__"));
    environment->define("plot_set_xlabel", std::string("__builtin_plot_set_xlabel__"));
    environment->define("plot_set_ylabel", std::string("__builtin_plot_set_ylabel__"));
    environment->define("plot_show", std::string("__builtin_plot_show__"));
    environment->define("plot_save_svg", std::string("__builtin_plot_save_svg__"));
    environment->define("quick_plot", std::string("__builtin_quick_plot__"));
    environment->define("quick_scatter", std::string("__builtin_quick_scatter__"));
    environment->define("quick_histogram", std::string("__builtin_quick_histogram__"));
    environment->define("csv_col_count", std::string("__builtin_csv_col_count__"));
    environment->define("csv_add_row", std::string("__builtin_csv_add_row__"));
    
    // Milestone 4: HTTP Server/Client
    environment->define("http_server_create", std::string("__builtin_http_server_create__"));
    environment->define("http_server_start", std::string("__builtin_http_server_start__"));
    environment->define("http_server_stop", std::string("__builtin_http_server_stop__"));
    environment->define("http_server_get", std::string("__builtin_http_server_get__"));
    environment->define("http_server_post", std::string("__builtin_http_server_post__"));
    environment->define("http_server_put", std::string("__builtin_http_server_put__"));
    environment->define("http_server_delete", std::string("__builtin_http_server_delete__"));
    environment->define("http_client_create", std::string("__builtin_http_client_create__"));
    environment->define("http_client_get", std::string("__builtin_http_client_get__"));
    environment->define("http_client_post", std::string("__builtin_http_client_post__"));
    environment->define("http_response_status", std::string("__builtin_http_response_status__"));
    environment->define("http_response_body", std::string("__builtin_http_response_body__"));
    environment->define("http_response_header", std::string("__builtin_http_response_header__"));
    environment->define("http_url_encode", std::string("__builtin_http_url_encode__"));
    environment->define("http_url_decode", std::string("__builtin_http_url_decode__"));
    environment->define("http_html_escape", std::string("__builtin_http_html_escape__"));
    
    // WebSocket Support
    environment->define("websocket_server_create", std::string("__builtin_websocket_server_create__"));
    environment->define("websocket_server_start", std::string("__builtin_websocket_server_start__"));
    environment->define("websocket_server_stop", std::string("__builtin_websocket_server_stop__"));
    environment->define("websocket_server_broadcast_text", std::string("__builtin_websocket_server_broadcast_text__"));
    environment->define("websocket_server_broadcast_binary", std::string("__builtin_websocket_server_broadcast_binary__"));
    environment->define("websocket_server_connection_count", std::string("__builtin_websocket_server_connection_count__"));
    environment->define("websocket_client_create", std::string("__builtin_websocket_client_create__"));
    environment->define("websocket_client_connect", std::string("__builtin_websocket_client_connect__"));
    environment->define("websocket_client_disconnect", std::string("__builtin_websocket_client_disconnect__"));
    environment->define("websocket_client_send_text", std::string("__builtin_websocket_client_send_text__"));
    environment->define("websocket_client_send_binary", std::string("__builtin_websocket_client_send_binary__"));
    environment->define("websocket_client_is_connected", std::string("__builtin_websocket_client_is_connected__"));
    environment->define("websocket_base64_encode", std::string("__builtin_websocket_base64_encode__"));
    environment->define("websocket_base64_decode", std::string("__builtin_websocket_base64_decode__"));
    
    // Template Engine
    environment->define("html_template_engine_create", std::string("__builtin_html_template_engine_create__"));
    environment->define("html_template_render", std::string("__builtin_html_template_render__"));
    environment->define("html_template_render_file", std::string("__builtin_html_template_render_file__"));
    environment->define("html_template_set_dir", std::string("__builtin_html_template_set_dir__"));
    environment->define("html_template_cache", std::string("__builtin_html_template_cache__"));
    
    // Database Drivers
    environment->define("postgresql_create", std::string("__builtin_postgresql_create__"));
    environment->define("postgresql_connect", std::string("__builtin_postgresql_connect__"));
    environment->define("postgresql_disconnect", std::string("__builtin_postgresql_disconnect__"));
    environment->define("postgresql_query", std::string("__builtin_postgresql_query__"));
    environment->define("postgresql_execute", std::string("__builtin_postgresql_execute__"));
    environment->define("mysql_create", std::string("__builtin_mysql_create__"));
    environment->define("mysql_connect", std::string("__builtin_mysql_connect__"));
    environment->define("mysql_disconnect", std::string("__builtin_mysql_disconnect__"));
    environment->define("mysql_query", std::string("__builtin_mysql_query__"));
    environment->define("mysql_execute", std::string("__builtin_mysql_execute__"));
    
    // Option type constructors
    environment->define("Some", std::string("__builtin_some__"));
    environment->define("None", std::string("__builtin_none__"));
    
    // Result type constructors
    environment->define("Ok", std::string("__builtin_ok__"));
    environment->define("Err", std::string("__builtin_err__"));
    
    // Built-in decorators
    environment->define("cache", std::string("__builtin_cache__"));
    environment->define("timing", std::string("__builtin_timing__"));
    environment->define("deprecated", std::string("__builtin_deprecated__"));
    environment->define("dataclass", std::string("__builtin_dataclass__"));
    environment->define("singleton", std::string("__builtin_singleton__"));
    environment->define("staticmethod", std::string("__builtin_staticmethod__"));
    environment->define("classmethod", std::string("__builtin_classmethod__"));
    environment->define("property", std::string("__builtin_property__"));
    
    // Collections module
    environment->define("Set", std::string("__builtin_set__"));
    environment->define("Queue", std::string("__builtin_queue__"));
    environment->define("Stack", std::string("__builtin_stack__"));
    environment->define("Deque", std::string("__builtin_deque__"));
    
    // Itertools module
    environment->define("map", std::string("__builtin_map__"));
    environment->define("filter", std::string("__builtin_filter__"));
    environment->define("reduce", std::string("__builtin_reduce__"));
    environment->define("take", std::string("__builtin_take__"));
    environment->define("drop", std::string("__builtin_drop__"));
    environment->define("zip", std::string("__builtin_zip__"));
    environment->define("enumerate", std::string("__builtin_enumerate__"));
    environment->define("chunk", std::string("__builtin_chunk__"));
    environment->define("flatten", std::string("__builtin_flatten__"));
    environment->define("all", std::string("__builtin_all__"));
    environment->define("any", std::string("__builtin_any__"));
    environment->define("find", std::string("__builtin_find__"));
    environment->define("unique", std::string("__builtin_unique__"));
    environment->define("reverse", std::string("__builtin_reverse__"));
    
    // Testing module
    environment->define("assert_true", std::string("__builtin_assert_true__"));
    environment->define("assert_false", std::string("__builtin_assert_false__"));
    environment->define("assert_equal", std::string("__builtin_assert_equal__"));
    
    // Reflection module
    environment->define("typeof", std::string("__builtin_typeof__"));
    environment->define("type_name", std::string("__builtin_type_name__"));
    environment->define("fields", std::string("__builtin_fields__"));
    environment->define("methods", std::string("__builtin_methods__"));
    environment->define("is_type", std::string("__builtin_is_type__"));
    environment->define("has_field", std::string("__builtin_has_field__"));
    environment->define("has_method", std::string("__builtin_has_method__"));
}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>& statements) {
    try {
        for (auto& stmt : statements) {
            stmt->accept(*this);
        }
        
        // Execute any pending goroutines
        executePendingTasks();
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
        
        // If the class is a dataclass, generate a nice string representation
        if (inst->klass->is_dataclass) {
            std::string result = inst->klass->name + "(";
            bool first = true;
            for (const auto& [field_name, field_value] : inst->fields) {
                if (!first) result += ", ";
                result += field_name + "=" + valueToString(field_value);
                first = false;
            }
            result += ")";
            return result;
        }
        
        return "<" + inst->klass->name + " instance>";
    } else if (std::holds_alternative<std::shared_ptr<BoundMethod>>(value)) {
        auto bm = std::get<std::shared_ptr<BoundMethod>>(value);
        return "<bound method " + bm->method->name + " of " + bm->instance->klass->name + ">";
    } else if (std::holds_alternative<std::shared_ptr<StaticMethod>>(value)) {
        auto sm = std::get<std::shared_ptr<StaticMethod>>(value);
        if (sm->is_classmethod) {
            return "<class method " + sm->method->name + " of " + sm->klass->name + ">";
        } else {
            return "<static method " + sm->method->name + " of " + sm->klass->name + ">";
        }
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
    } else if (std::holds_alternative<std::shared_ptr<ChannelValue>>(value)) {
        auto channel = std::get<std::shared_ptr<ChannelValue>>(value);
        return "<channel capacity=" + std::to_string(channel->capacity) + 
               " size=" + std::to_string(channel->buffer.size()) + 
               (channel->is_closed() ? " closed" : "") + ">";
    } else if (std::holds_alternative<std::shared_ptr<ChannelMethod>>(value)) {
        auto cm = std::get<std::shared_ptr<ChannelMethod>>(value);
        return "<channel method " + cm->method_name + ">";
    } else if (std::holds_alternative<std::shared_ptr<WaitGroupValue>>(value)) {
        auto wg = std::get<std::shared_ptr<WaitGroupValue>>(value);
        return "<WaitGroup counter=" + std::to_string(wg->counter) + ">";
    } else if (std::holds_alternative<std::shared_ptr<WaitGroupMethod>>(value)) {
        auto wgm = std::get<std::shared_ptr<WaitGroupMethod>>(value);
        return "<WaitGroup method " + wgm->method_name + ">";
    } else if (std::holds_alternative<std::shared_ptr<OptionValue>>(value)) {
        auto option = std::get<std::shared_ptr<OptionValue>>(value);
        if (option->isNone()) {
            return "None";
        } else {
            return "Some(" + valueToString(option->value) + ")";
        }
    } else if (std::holds_alternative<std::shared_ptr<OptionMethod>>(value)) {
        auto om = std::get<std::shared_ptr<OptionMethod>>(value);
        return "<Option method " + om->method_name + ">";
    } else if (std::holds_alternative<std::shared_ptr<ResultValue>>(value)) {
        auto result = std::get<std::shared_ptr<ResultValue>>(value);
        if (result->isOk()) {
            return "Ok(" + valueToString(result->value) + ")";
        } else {
            return "Err(" + valueToString(result->value) + ")";
        }
    } else if (std::holds_alternative<std::shared_ptr<ResultMethod>>(value)) {
        auto rm = std::get<std::shared_ptr<ResultMethod>>(value);
        return "<Result method " + rm->method_name + ">";
    } else if (std::holds_alternative<std::shared_ptr<ArrayMethod>>(value)) {
        auto am = std::get<std::shared_ptr<ArrayMethod>>(value);
        return "<Array method " + am->method_name + ">";
    } else if (std::holds_alternative<std::shared_ptr<StringMethod>>(value)) {
        auto sm = std::get<std::shared_ptr<StringMethod>>(value);
        return "<String method " + sm->method_name + ">";
    } else if (std::holds_alternative<std::shared_ptr<HashMapValue>>(value)) {
        auto hashmap = std::get<std::shared_ptr<HashMapValue>>(value);
        std::string result = "{";
        bool first = true;
        for (const auto& pair : hashmap->pairs) {
            if (!first) result += ", ";
            result += "\"" + pair.first + "\": " + valueToString(pair.second);
            first = false;
        }
        result += "}";
        return result;
    } else if (std::holds_alternative<std::shared_ptr<HashMapMethod>>(value)) {
        auto hm = std::get<std::shared_ptr<HashMapMethod>>(value);
        return "<HashMap method " + hm->method_name + ">";
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
        if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
            last_value = std::get<int>(left) < std::get<int>(right);
        } else {
            double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
            double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
            last_value = l < r;
        }
    } else if (expr.op == "<=") {
        if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
            last_value = std::get<int>(left) <= std::get<int>(right);
        } else {
            double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
            double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
            last_value = l <= r;
        }
    } else if (expr.op == ">") {
        if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
            last_value = std::get<int>(left) > std::get<int>(right);
        } else {
            double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
            double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
            last_value = l > r;
        }
    } else if (expr.op == ">=") {
        if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
            last_value = std::get<int>(left) >= std::get<int>(right);
        } else {
            double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
            double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
            last_value = l >= r;
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
    
    // Handle channel methods
    if (std::holds_alternative<std::shared_ptr<ChannelMethod>>(callee)) {
        auto cm = std::get<std::shared_ptr<ChannelMethod>>(callee);
        
        if (cm->method_name == "close") {
            if (!arguments.empty()) {
                throw TypeError("Channel.close() takes no arguments");
            }
            cm->channel->close();
            last_value = nullptr;
            return;
        } else if (cm->method_name == "is_closed") {
            if (!arguments.empty()) {
                throw TypeError("Channel.is_closed() takes no arguments");
            }
            last_value = cm->channel->is_closed();
            return;
        } else if (cm->method_name == "empty") {
            if (!arguments.empty()) {
                throw TypeError("Channel.empty() takes no arguments");
            }
            last_value = cm->channel->empty();
            return;
        } else if (cm->method_name == "size") {
            if (!arguments.empty()) {
                throw TypeError("Channel.size() takes no arguments");
            }
            last_value = static_cast<int>(cm->channel->buffer.size());
            return;
        }
        
        throw RuntimeError("Unknown channel method: " + cm->method_name);
    }
    
    // Handle array methods
    if (std::holds_alternative<std::shared_ptr<ArrayMethod>>(callee)) {
        auto am = std::get<std::shared_ptr<ArrayMethod>>(callee);
        
        if (am->method_name == "push") {
            if (arguments.size() != 1) {
                throw TypeError("Array.push() takes exactly 1 argument");
            }
            am->array->elements.push_back(arguments[0]);
            last_value = static_cast<int>(am->array->elements.size());
            return;
        } else if (am->method_name == "pop") {
            if (!arguments.empty()) {
                throw TypeError("Array.pop() takes no arguments");
            }
            if (am->array->elements.empty()) {
                throw RuntimeError("Cannot pop from empty array");
            }
            Value popped = am->array->elements.back();
            am->array->elements.pop_back();
            last_value = popped;
            return;
        }
        
        throw RuntimeError("Unknown array method: " + am->method_name);
    }
    
    // Handle string methods
    if (std::holds_alternative<std::shared_ptr<StringMethod>>(callee)) {
        auto sm = std::get<std::shared_ptr<StringMethod>>(callee);
        
        if (sm->method_name == "upper") {
            if (!arguments.empty()) {
                throw TypeError("String.upper() takes no arguments");
            }
            std::string result = sm->string_value;
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
            last_value = result;
            return;
        } else if (sm->method_name == "lower") {
            if (!arguments.empty()) {
                throw TypeError("String.lower() takes no arguments");
            }
            std::string result = sm->string_value;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            last_value = result;
            return;
        } else if (sm->method_name == "split") {
            if (arguments.size() != 1) {
                throw TypeError("String.split() takes exactly 1 argument");
            }
            if (!std::holds_alternative<std::string>(arguments[0])) {
                throw TypeError("String.split() argument must be a string");
            }
            
            std::string delimiter = std::get<std::string>(arguments[0]);
            std::string text = sm->string_value;
            auto result_array = std::make_shared<ArrayValue>();
            
            if (delimiter.empty()) {
                // Split into individual characters
                for (char c : text) {
                    result_array->elements.push_back(std::string(1, c));
                }
            } else {
                // Split by delimiter
                size_t start = 0;
                size_t end = text.find(delimiter);
                
                while (end != std::string::npos) {
                    result_array->elements.push_back(text.substr(start, end - start));
                    start = end + delimiter.length();
                    end = text.find(delimiter, start);
                }
                result_array->elements.push_back(text.substr(start));
            }
            
            last_value = result_array;
            return;
        }
        
        throw RuntimeError("Unknown string method: " + sm->method_name);
    }
    
    // Handle hash map methods
    if (std::holds_alternative<std::shared_ptr<HashMapMethod>>(callee)) {
        auto hm = std::get<std::shared_ptr<HashMapMethod>>(callee);
        
        if (hm->method_name == "get") {
            if (arguments.size() != 1) {
                throw TypeError("HashMap.get() takes exactly 1 argument");
            }
            std::string key = valueToString(arguments[0]);
            last_value = hm->hashmap->get(key);
            return;
        } else if (hm->method_name == "set") {
            if (arguments.size() != 2) {
                throw TypeError("HashMap.set() takes exactly 2 arguments");
            }
            std::string key = valueToString(arguments[0]);
            hm->hashmap->pairs[key] = arguments[1];
            last_value = arguments[1];
            return;
        } else if (hm->method_name == "has") {
            if (arguments.size() != 1) {
                throw TypeError("HashMap.has() takes exactly 1 argument");
            }
            std::string key = valueToString(arguments[0]);
            last_value = hm->hashmap->has(key);
            return;
        } else if (hm->method_name == "remove") {
            if (arguments.size() != 1) {
                throw TypeError("HashMap.remove() takes exactly 1 argument");
            }
            std::string key = valueToString(arguments[0]);
            auto it = hm->hashmap->pairs.find(key);
            if (it != hm->hashmap->pairs.end()) {
                Value removed = it->second;
                hm->hashmap->pairs.erase(it);
                last_value = removed;
            } else {
                last_value = Value(nullptr);
            }
            return;
        } else if (hm->method_name == "keys") {
            if (!arguments.empty()) {
                throw TypeError("HashMap.keys() takes no arguments");
            }
            auto result_array = std::make_shared<ArrayValue>();
            for (const auto& pair : hm->hashmap->pairs) {
                result_array->elements.push_back(pair.first);
            }
            last_value = result_array;
            return;
        } else if (hm->method_name == "values") {
            if (!arguments.empty()) {
                throw TypeError("HashMap.values() takes no arguments");
            }
            auto result_array = std::make_shared<ArrayValue>();
            for (const auto& pair : hm->hashmap->pairs) {
                result_array->elements.push_back(pair.second);
            }
            last_value = result_array;
            return;
        }
        
        throw RuntimeError("Unknown hash map method: " + hm->method_name);
    }
    
    // Handle user-defined functions
    if (std::holds_alternative<std::shared_ptr<Function>>(callee)) {
        auto func = std::get<std::shared_ptr<Function>>(callee);
        
        // Extract original function name and check for built-in decorator markers
        std::string func_name = func->name;
        std::string original_name = func->name;
        bool is_cached = false;
        bool is_timed = false;
        bool is_deprecated = false;
        std::string deprecation_message;
        
        // Strip decorator prefixes in order (they're applied bottom-to-top, so we strip top-to-bottom)
        // Check for @timing decorator
        if (func_name.find("__timed__") == 0) {
            is_timed = true;
            func_name = func_name.substr(9);  // Remove "__timed__" prefix
        }
        
        // Check for @cache decorator
        if (func_name.find("__cached__") == 0) {
            is_cached = true;
            func_name = func_name.substr(10);  // Remove "__cached__" prefix
        }
        
        // Check for @deprecated decorator
        if (func_name.find("__deprecated__") == 0) {
            is_deprecated = true;
            size_t msg_start = func_name.find("__", 14);
            if (msg_start != std::string::npos) {
                deprecation_message = func_name.substr(msg_start + 2);
                func_name = func_name.substr(14, msg_start - 14);
            } else {
                func_name = func_name.substr(14);
            }
        }
        
        original_name = func_name;
        
        // Print deprecation warning if needed
        if (is_deprecated) {
            std::cout << "Warning: " << deprecation_message << std::endl;
        }
        
        // Check cache if @cache decorator is applied
        if (is_cached) {
            // Create cache key from function name and stringified arguments
            std::string args_str;
            for (size_t i = 0; i < arguments.size(); i++) {
                args_str += valueToString(arguments[i]);
                if (i < arguments.size() - 1) {
                    args_str += ",";
                }
            }
            auto cache_key = std::make_pair(original_name, args_str);
            auto it = function_cache.find(cache_key);
            if (it != function_cache.end()) {
                last_value = it->second;
                return;
            }
        }
        
        // Start timing if @timing decorator is applied
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Check argument count
        if (arguments.size() != func->parameters.size()) {
            throw TypeError("Function '" + original_name + "' expects " + 
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
            for (auto& stmt : *func->getBody()) {
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
        
        // Store result in cache if @cache decorator is applied
        if (is_cached) {
            // Create cache key from function name and stringified arguments
            std::string args_str;
            for (size_t i = 0; i < arguments.size(); i++) {
                args_str += valueToString(arguments[i]);
                if (i < arguments.size() - 1) {
                    args_str += ",";
                }
            }
            auto cache_key = std::make_pair(original_name, args_str);
            function_cache[cache_key] = last_value;
        }
        
        // Print timing if @timing decorator is applied
        if (is_timed) {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            std::cout << "Function '" << original_name << "' took " 
                      << duration.count() << " microseconds" << std::endl;
        }
        
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
            for (auto& stmt : *func->getBody()) {
                stmt->accept(*this);
            }
            last_value = nullptr;
        } catch (const ReturnException& ret) {
            last_value = ret.value;
        }
        
        environment = previous_env;
        return;
    }
    
    // Handle static methods
    if (std::holds_alternative<std::shared_ptr<StaticMethod>>(callee)) {
        auto sm = std::get<std::shared_ptr<StaticMethod>>(callee);
        auto func = sm->method;
        
        if (sm->is_classmethod) {
            // @classmethod: first parameter is the class itself
            if (func->parameters.empty()) {
                throw TypeError("Class method '" + func->name + "' must have at least a 'cls' parameter");
            }
            
            // Expected arguments: all parameters except 'cls'
            if (arguments.size() != func->parameters.size() - 1) {
                throw TypeError("Class method '" + func->name + "' expects " + 
                                std::to_string(func->parameters.size() - 1) + 
                                " arguments but got " + 
                                std::to_string(arguments.size()));
            }
            
            auto func_env = std::make_shared<Environment>(func->closure);
            
            // Bind cls (first parameter) to the class
            func_env->define(func->parameters[0].first, sm->klass);
            // Bind remaining parameters
            for (size_t i = 1; i < func->parameters.size(); i++) {
                func_env->define(func->parameters[i].first, arguments[i - 1]);
            }
            
            auto previous_env = environment;
            environment = func_env;
            
            try {
                for (auto& stmt : *func->getBody()) {
                    stmt->accept(*this);
                }
                last_value = nullptr;
            } catch (const ReturnException& ret) {
                last_value = ret.value;
            }
            
            environment = previous_env;
            return;
        } else {
            // @staticmethod: no implicit first parameter
            if (arguments.size() != func->parameters.size()) {
                throw TypeError("Static method '" + func->name + "' expects " + 
                                std::to_string(func->parameters.size()) + 
                                " arguments but got " + 
                                std::to_string(arguments.size()));
            }
            
            auto func_env = std::make_shared<Environment>(func->closure);
            
            // Bind all parameters
            for (size_t i = 0; i < func->parameters.size(); i++) {
                func_env->define(func->parameters[i].first, arguments[i]);
            }
            
            auto previous_env = environment;
            environment = func_env;
            
            try {
                for (auto& stmt : *func->getBody()) {
                    stmt->accept(*this);
                }
                last_value = nullptr;
            } catch (const ReturnException& ret) {
                last_value = ret.value;
            }
            
            environment = previous_env;
            return;
        }
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
                for (auto& stmt : *init->getBody()) {
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
        } else if (func_name == "__builtin_time__") {
            // Return current time in seconds since epoch as double
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(duration);
            last_value = seconds.count();
        } else if (func_name == "__builtin_sleep__") {
            // Sleep for specified seconds (accepts int or double)
            if (arguments.empty()) {
                throw TypeError("sleep() requires 1 argument (seconds)");
            }
            
            double seconds = 0.0;
            if (std::holds_alternative<int>(arguments[0])) {
                seconds = static_cast<double>(std::get<int>(arguments[0]));
            } else if (std::holds_alternative<double>(arguments[0])) {
                seconds = std::get<double>(arguments[0]);
            } else {
                throw TypeError("sleep() requires numeric argument");
            }
            
            auto duration = std::chrono::duration<double>(seconds);
            std::this_thread::sleep_for(duration);
            last_value = nullptr;
        } else if (func_name == "__builtin_sort__") {
            // Fast native sort - O(n log n)
            if (arguments.empty()) {
                throw TypeError("sort() requires 1 argument (array)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("sort() requires an array");
            }
            
            auto array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            
            // Sort using C++ std::sort (highly optimized)
            std::sort(array->elements.begin(), array->elements.end(), 
                [](const Value& a, const Value& b) {
                    if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
                        return std::get<int>(a) < std::get<int>(b);
                    } else if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b)) {
                        return std::get<double>(a) < std::get<double>(b);
                    } else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
                        return std::get<std::string>(a) < std::get<std::string>(b);
                    }
                    return false;
                });
            
            last_value = array;
        } else if (func_name == "__builtin_make_array__") {
            // Fast array generation for benchmarking
            // make_array(size, mode) where mode: "random", "sorted", "reverse"
            if (arguments.empty()) {
                throw TypeError("make_array() requires at least 1 argument (size)");
            }
            
            int size = std::get<int>(arguments[0]);
            std::string mode = "random";
            if (arguments.size() > 1) {
                mode = std::get<std::string>(arguments[1]);
            }
            
            auto array = std::make_shared<ArrayValue>();
            array->elements.reserve(size);
            
            if (mode == "reverse") {
                for (int i = size; i > 0; i--) {
                    array->elements.push_back(i);
                }
            } else if (mode == "sorted") {
                for (int i = 1; i <= size; i++) {
                    array->elements.push_back(i);
                }
            } else {
                // Random
                srand(42);
                for (int i = 0; i < size; i++) {
                    array->elements.push_back(rand() % 100000);
                }
            }
            
            last_value = array;
        } else if (func_name == "__builtin_waitgroup__") {
            // Create a new WaitGroup
            auto wg = std::make_shared<WaitGroupValue>();
            last_value = wg;
            return;
        } else if (func_name == "__builtin_some__") {
            // Create Some(value) - Option with a value
            if (arguments.size() != 1) {
                throw TypeError("Some() requires exactly 1 argument");
            }
            auto option = std::make_shared<OptionValue>(arguments[0]);
            last_value = option;
            return;
        } else if (func_name == "__builtin_none__") {
            // Create None - empty Option
            if (!arguments.empty()) {
                throw TypeError("None() takes no arguments");
            }
            auto option = std::make_shared<OptionValue>();
            last_value = option;
            return;
        } else if (func_name == "__builtin_ok__") {
            // Create Ok(value) - successful Result
            if (arguments.size() != 1) {
                throw TypeError("Ok() requires exactly 1 argument");
            }
            auto result = std::make_shared<ResultValue>(true, arguments[0]);
            last_value = result;
            return;
        } else if (func_name == "__builtin_err__") {
            // Create Err(error) - error Result
            if (arguments.size() != 1) {
                throw TypeError("Err() requires exactly 1 argument");
            }
            auto result = std::make_shared<ResultValue>(false, arguments[0]);
            last_value = result;
            return;
        } else if (func_name == "__builtin_map__") {
            // map(array, function) - Transform each element
            if (arguments.size() != 2) {
                throw TypeError("map() requires 2 arguments (array, function)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("map() first argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto func = arguments[1];
            
            auto result_array = std::make_shared<ArrayValue>();
            for (const auto& elem : input_array->elements) {
                // Call the function with the element
                std::vector<Value> args = {elem};
                
                // Save current last_value
                Value saved = last_value;
                
                // Create a CallExpr to call the function
                if (std::holds_alternative<std::shared_ptr<Function>>(func)) {
                    auto fn = std::get<std::shared_ptr<Function>>(func);
                    
                    // Create new environment for function call
                    auto fn_env = std::make_shared<Environment>(fn->closure);
                    
                    // Bind parameters
                    if (fn->parameters.size() != 1) {
                        throw TypeError("map() function must take exactly 1 argument");
                    }
                    fn_env->define(fn->parameters[0].first, elem);
                    
                    // Execute function body
                    auto previous_env = environment;
                    environment = fn_env;
                    
                    try {
                        for (auto& stmt : *fn->getBody()) {
                            stmt->accept(*this);
                        }
                    } catch (const ReturnException& e) {
                        result_array->elements.push_back(e.value);
                        environment = previous_env;
                        continue;
                    }
                    
                    environment = previous_env;
                    result_array->elements.push_back(last_value);
                } else {
                    throw TypeError("map() second argument must be a function");
                }
                
                last_value = saved;
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_filter__") {
            // filter(array, predicate) - Keep elements that match
            if (arguments.size() != 2) {
                throw TypeError("filter() requires 2 arguments (array, predicate)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("filter() first argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto func = arguments[1];
            
            auto result_array = std::make_shared<ArrayValue>();
            
            if (std::holds_alternative<std::shared_ptr<Function>>(func)) {
                auto fn = std::get<std::shared_ptr<Function>>(func);
                
                for (const auto& elem : input_array->elements) {
                    // Create new environment for function call
                    auto fn_env = std::make_shared<Environment>(fn->closure);
                    
                    // Bind parameters
                    if (fn->parameters.size() != 1) {
                        throw TypeError("filter() predicate must take exactly 1 argument");
                    }
                    fn_env->define(fn->parameters[0].first, elem);
                    
                    // Execute function body
                    auto previous_env = environment;
                    environment = fn_env;
                    
                    Value result_val;
                    try {
                        for (auto& stmt : *fn->getBody()) {
                            stmt->accept(*this);
                        }
                        result_val = last_value;
                    } catch (const ReturnException& e) {
                        result_val = e.value;
                    }
                    
                    environment = previous_env;
                    
                    // Check if result is truthy
                    if (isTruthy(result_val)) {
                        result_array->elements.push_back(elem);
                    }
                }
            } else {
                throw TypeError("filter() second argument must be a function");
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_reduce__") {
            // reduce(array, initial, function) - Combine elements
            if (arguments.size() != 3) {
                throw TypeError("reduce() requires 3 arguments (array, initial, function)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("reduce() first argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            Value accumulator = arguments[1];
            auto func = arguments[2];
            
            if (std::holds_alternative<std::shared_ptr<Function>>(func)) {
                auto fn = std::get<std::shared_ptr<Function>>(func);
                
                if (fn->parameters.size() != 2) {
                    throw TypeError("reduce() function must take exactly 2 arguments");
                }
                
                for (const auto& elem : input_array->elements) {
                    // Create new environment for function call
                    auto fn_env = std::make_shared<Environment>(fn->closure);
                    
                    // Bind parameters (accumulator, element)
                    fn_env->define(fn->parameters[0].first, accumulator);
                    fn_env->define(fn->parameters[1].first, elem);
                    
                    // Execute function body
                    auto previous_env = environment;
                    environment = fn_env;
                    
                    try {
                        for (auto& stmt : *fn->getBody()) {
                            stmt->accept(*this);
                        }
                        accumulator = last_value;
                    } catch (const ReturnException& e) {
                        accumulator = e.value;
                    }
                    
                    environment = previous_env;
                }
            } else {
                throw TypeError("reduce() third argument must be a function");
            }
            
            last_value = accumulator;
            return;
        } else if (func_name == "__builtin_take__") {
            // take(array, n) - Take first n elements
            if (arguments.size() != 2) {
                throw TypeError("take() requires 2 arguments (array, n)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("take() first argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            int n = std::get<int>(arguments[1]);
            
            auto result_array = std::make_shared<ArrayValue>();
            size_t count = std::min(static_cast<size_t>(n), input_array->elements.size());
            
            for (size_t i = 0; i < count; i++) {
                result_array->elements.push_back(input_array->elements[i]);
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_drop__") {
            // drop(array, n) - Skip first n elements
            if (arguments.size() != 2) {
                throw TypeError("drop() requires 2 arguments (array, n)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("drop() first argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            int n = std::get<int>(arguments[1]);
            
            auto result_array = std::make_shared<ArrayValue>();
            size_t start = std::min(static_cast<size_t>(n), input_array->elements.size());
            
            for (size_t i = start; i < input_array->elements.size(); i++) {
                result_array->elements.push_back(input_array->elements[i]);
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_reverse__") {
            // reverse(array) - Reverse array order
            if (arguments.size() != 1) {
                throw TypeError("reverse() requires 1 argument (array)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("reverse() argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto result_array = std::make_shared<ArrayValue>();
            
            for (auto it = input_array->elements.rbegin(); it != input_array->elements.rend(); ++it) {
                result_array->elements.push_back(*it);
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_unique__") {
            // unique(array) - Remove duplicates
            if (arguments.size() != 1) {
                throw TypeError("unique() requires 1 argument (array)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("unique() argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto result_array = std::make_shared<ArrayValue>();
            std::set<std::string> seen;  // Use string representation for comparison
            
            for (const auto& elem : input_array->elements) {
                std::string key = valueToString(elem);
                if (seen.find(key) == seen.end()) {
                    result_array->elements.push_back(elem);
                    seen.insert(key);
                }
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_enumerate__") {
            // enumerate(array) - Add indices to elements, returns array of [index, value]
            if (arguments.size() != 1) {
                throw TypeError("enumerate() requires 1 argument (array)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("enumerate() argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto result_array = std::make_shared<ArrayValue>();
            
            for (size_t i = 0; i < input_array->elements.size(); i++) {
                auto pair = std::make_shared<ArrayValue>();
                pair->elements.push_back(static_cast<int>(i));
                pair->elements.push_back(input_array->elements[i]);
                result_array->elements.push_back(pair);
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_zip__") {
            // zip(array1, array2) - Combine two arrays into pairs
            if (arguments.size() != 2) {
                throw TypeError("zip() requires 2 arguments (array1, array2)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0]) ||
                !std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[1])) {
                throw TypeError("zip() arguments must be arrays");
            }
            
            auto array1 = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto array2 = std::get<std::shared_ptr<ArrayValue>>(arguments[1]);
            auto result_array = std::make_shared<ArrayValue>();
            
            size_t min_size = std::min(array1->elements.size(), array2->elements.size());
            for (size_t i = 0; i < min_size; i++) {
                auto pair = std::make_shared<ArrayValue>();
                pair->elements.push_back(array1->elements[i]);
                pair->elements.push_back(array2->elements[i]);
                result_array->elements.push_back(pair);
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_find__") {
            // find(array, predicate) - Find first element matching predicate, returns Option
            if (arguments.size() != 2) {
                throw TypeError("find() requires 2 arguments (array, predicate)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("find() first argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto func = arguments[1];
            
            if (std::holds_alternative<std::shared_ptr<Function>>(func)) {
                auto fn = std::get<std::shared_ptr<Function>>(func);
                
                for (const auto& elem : input_array->elements) {
                    // Create new environment for function call
                    auto fn_env = std::make_shared<Environment>(fn->closure);
                    
                    if (fn->parameters.size() != 1) {
                        throw TypeError("find() predicate must take exactly 1 argument");
                    }
                    fn_env->define(fn->parameters[0].first, elem);
                    
                    // Execute function body
                    auto previous_env = environment;
                    environment = fn_env;
                    
                    Value result_val;
                    try {
                        for (auto& stmt : *fn->getBody()) {
                            stmt->accept(*this);
                        }
                        result_val = last_value;
                    } catch (const ReturnException& e) {
                        result_val = e.value;
                    }
                    
                    environment = previous_env;
                    
                    // If predicate returns true, return Some(elem)
                    if (isTruthy(result_val)) {
                        auto option = std::make_shared<OptionValue>(elem);
                        last_value = option;
                        return;
                    }
                }
            } else {
                throw TypeError("find() second argument must be a function");
            }
            
            // Not found - return None
            auto option = std::make_shared<OptionValue>();
            last_value = option;
            return;
        } else if (func_name == "__builtin_all__") {
            // all(array, predicate) - Check if all elements match
            if (arguments.size() != 2) {
                throw TypeError("all() requires 2 arguments (array, predicate)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("all() first argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto func = arguments[1];
            
            if (std::holds_alternative<std::shared_ptr<Function>>(func)) {
                auto fn = std::get<std::shared_ptr<Function>>(func);
                
                for (const auto& elem : input_array->elements) {
                    auto fn_env = std::make_shared<Environment>(fn->closure);
                    
                    if (fn->parameters.size() != 1) {
                        throw TypeError("all() predicate must take exactly 1 argument");
                    }
                    fn_env->define(fn->parameters[0].first, elem);
                    
                    auto previous_env = environment;
                    environment = fn_env;
                    
                    Value result_val;
                    try {
                        for (auto& stmt : *fn->getBody()) {
                            stmt->accept(*this);
                        }
                        result_val = last_value;
                    } catch (const ReturnException& e) {
                        result_val = e.value;
                    }
                    
                    environment = previous_env;
                    
                    // If any element doesn't match, return false
                    if (!isTruthy(result_val)) {
                        last_value = false;
                        return;
                    }
                }
            } else {
                throw TypeError("all() second argument must be a function");
            }
            
            last_value = true;
            return;
        } else if (func_name == "__builtin_any__") {
            // any(array, predicate) - Check if any element matches
            if (arguments.size() != 2) {
                throw TypeError("any() requires 2 arguments (array, predicate)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("any() first argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto func = arguments[1];
            
            if (std::holds_alternative<std::shared_ptr<Function>>(func)) {
                auto fn = std::get<std::shared_ptr<Function>>(func);
                
                for (const auto& elem : input_array->elements) {
                    auto fn_env = std::make_shared<Environment>(fn->closure);
                    
                    if (fn->parameters.size() != 1) {
                        throw TypeError("any() predicate must take exactly 1 argument");
                    }
                    fn_env->define(fn->parameters[0].first, elem);
                    
                    auto previous_env = environment;
                    environment = fn_env;
                    
                    Value result_val;
                    try {
                        for (auto& stmt : *fn->getBody()) {
                            stmt->accept(*this);
                        }
                        result_val = last_value;
                    } catch (const ReturnException& e) {
                        result_val = e.value;
                    }
                    
                    environment = previous_env;
                    
                    // If any element matches, return true
                    if (isTruthy(result_val)) {
                        last_value = true;
                        return;
                    }
                }
            } else {
                throw TypeError("any() second argument must be a function");
            }
            
            last_value = false;
            return;
        } else if (func_name == "__builtin_chunk__") {
            // chunk(array, n) - Split into chunks of size n
            if (arguments.size() != 2) {
                throw TypeError("chunk() requires 2 arguments (array, n)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("chunk() first argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            int n = std::get<int>(arguments[1]);
            
            if (n <= 0) {
                throw ValueError("chunk() size must be positive");
            }
            
            auto result_array = std::make_shared<ArrayValue>();
            
            for (size_t i = 0; i < input_array->elements.size(); i += n) {
                auto chunk = std::make_shared<ArrayValue>();
                size_t end = std::min(i + n, input_array->elements.size());
                
                for (size_t j = i; j < end; j++) {
                    chunk->elements.push_back(input_array->elements[j]);
                }
                
                result_array->elements.push_back(chunk);
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_flatten__") {
            // flatten(array) - Flatten nested arrays
            if (arguments.size() != 1) {
                throw TypeError("flatten() requires 1 argument (array)");
            }
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(arguments[0])) {
                throw TypeError("flatten() argument must be an array");
            }
            
            auto input_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto result_array = std::make_shared<ArrayValue>();
            
            for (const auto& elem : input_array->elements) {
                if (std::holds_alternative<std::shared_ptr<ArrayValue>>(elem)) {
                    auto inner = std::get<std::shared_ptr<ArrayValue>>(elem);
                    for (const auto& inner_elem : inner->elements) {
                        result_array->elements.push_back(inner_elem);
                    }
                } else {
                    result_array->elements.push_back(elem);
                }
            }
            
            last_value = result_array;
            return;
        } else if (func_name == "__builtin_assert_true__") {
            // assert_true(condition, message?)
            if (arguments.empty()) {
                throw TypeError("assert_true() requires at least 1 argument");
            }
            
            bool condition = isTruthy(arguments[0]);
            std::string message = arguments.size() > 1 ? valueToString(arguments[1]) : "Assertion failed";
            
            if (!condition) {
                throw RuntimeError("AssertionError: " + message);
            }
            
            last_value = nullptr;
            return;
        } else if (func_name == "__builtin_assert_false__") {
            // assert_false(condition, message?)
            if (arguments.empty()) {
                throw TypeError("assert_false() requires at least 1 argument");
            }
            
            bool condition = isTruthy(arguments[0]);
            std::string message = arguments.size() > 1 ? valueToString(arguments[1]) : "Assertion failed";
            
            if (condition) {
                throw RuntimeError("AssertionError: " + message);
            }
            
            last_value = nullptr;
            return;
        } else if (func_name == "__builtin_assert_equal__") {
            // assert_equal(actual, expected, message?)
            if (arguments.size() < 2) {
                throw TypeError("assert_equal() requires at least 2 arguments");
            }
            
            bool equal = valuesEqual(arguments[0], arguments[1]);
            std::string message = arguments.size() > 2 ? valueToString(arguments[2]) : 
                "Expected " + valueToString(arguments[1]) + " but got " + valueToString(arguments[0]);
            
            if (!equal) {
                throw RuntimeError("AssertionError: " + message);
            }
            
            last_value = nullptr;
            return;
        } else if (func_name == "__builtin_typeof__") {
            // typeof(value) - Get type as string
            if (arguments.size() != 1) {
                throw TypeError("typeof() requires exactly 1 argument");
            }
            
            std::string type_str;
            const auto& val = arguments[0];
            
            if (std::holds_alternative<int>(val)) {
                type_str = "int";
            } else if (std::holds_alternative<double>(val)) {
                type_str = "float";
            } else if (std::holds_alternative<std::string>(val)) {
                type_str = "str";
            } else if (std::holds_alternative<bool>(val)) {
                type_str = "bool";
            } else if (std::holds_alternative<std::nullptr_t>(val)) {
                type_str = "none";
            } else if (std::holds_alternative<std::shared_ptr<ArrayValue>>(val)) {
                type_str = "array";
            } else if (std::holds_alternative<std::shared_ptr<Function>>(val)) {
                type_str = "function";
            } else if (std::holds_alternative<std::shared_ptr<Class>>(val)) {
                type_str = "class";
            } else if (std::holds_alternative<std::shared_ptr<Instance>>(val)) {
                auto inst = std::get<std::shared_ptr<Instance>>(val);
                type_str = inst->klass->name;
            } else if (std::holds_alternative<std::shared_ptr<ChannelValue>>(val)) {
                type_str = "channel";
            } else if (std::holds_alternative<std::shared_ptr<WaitGroupValue>>(val)) {
                type_str = "waitgroup";
            } else if (std::holds_alternative<std::shared_ptr<OptionValue>>(val)) {
                type_str = "option";
            } else if (std::holds_alternative<std::shared_ptr<ResultValue>>(val)) {
                type_str = "result";
            } else {
                type_str = "unknown";
            }
            
            last_value = type_str;
            return;
        } else if (func_name == "__builtin_type_name__") {
            // type_name(value) - Alias for typeof
            if (arguments.size() != 1) {
                throw TypeError("type_name() requires exactly 1 argument");
            }
            
            // Reuse typeof implementation
            std::vector<Value> args = {arguments[0]};
            arguments = args;
            func_name = "__builtin_typeof__";
            // Fall through to typeof implementation above
            
            std::string type_str;
            const auto& val = arguments[0];
            
            if (std::holds_alternative<int>(val)) {
                type_str = "int";
            } else if (std::holds_alternative<double>(val)) {
                type_str = "float";
            } else if (std::holds_alternative<std::string>(val)) {
                type_str = "str";
            } else if (std::holds_alternative<bool>(val)) {
                type_str = "bool";
            } else if (std::holds_alternative<std::nullptr_t>(val)) {
                type_str = "none";
            } else if (std::holds_alternative<std::shared_ptr<ArrayValue>>(val)) {
                type_str = "array";
            } else if (std::holds_alternative<std::shared_ptr<Function>>(val)) {
                type_str = "function";
            } else if (std::holds_alternative<std::shared_ptr<Class>>(val)) {
                type_str = "class";
            } else if (std::holds_alternative<std::shared_ptr<Instance>>(val)) {
                auto inst = std::get<std::shared_ptr<Instance>>(val);
                type_str = inst->klass->name;
            } else if (std::holds_alternative<std::shared_ptr<ChannelValue>>(val)) {
                type_str = "channel";
            } else if (std::holds_alternative<std::shared_ptr<WaitGroupValue>>(val)) {
                type_str = "waitgroup";
            } else if (std::holds_alternative<std::shared_ptr<OptionValue>>(val)) {
                type_str = "option";
            } else if (std::holds_alternative<std::shared_ptr<ResultValue>>(val)) {
                type_str = "result";
            } else {
                type_str = "unknown";
            }
            
            last_value = type_str;
            return;
        } else if (func_name == "__builtin_fields__") {
            // fields(object) - Get object fields as array
            if (arguments.size() != 1) {
                throw TypeError("fields() requires exactly 1 argument");
            }
            
            if (!std::holds_alternative<std::shared_ptr<Instance>>(arguments[0])) {
                throw TypeError("fields() requires an object instance");
            }
            
            auto inst = std::get<std::shared_ptr<Instance>>(arguments[0]);
            auto result = std::make_shared<ArrayValue>();
            
            for (const auto& [field_name, field_value] : inst->fields) {
                result->elements.push_back(field_name);
            }
            
            last_value = result;
            return;
        } else if (func_name == "__builtin_methods__") {
            // methods(object) - Get object methods as array
            if (arguments.size() != 1) {
                throw TypeError("methods() requires exactly 1 argument");
            }
            
            if (!std::holds_alternative<std::shared_ptr<Instance>>(arguments[0])) {
                throw TypeError("methods() requires an object instance");
            }
            
            auto inst = std::get<std::shared_ptr<Instance>>(arguments[0]);
            auto result = std::make_shared<ArrayValue>();
            
            for (const auto& [method_name, method] : inst->klass->methods) {
                result->elements.push_back(method_name);
            }
            
            last_value = result;
            return;
        } else if (func_name == "__builtin_is_type__") {
            // is_type(value, type_name) - Check if value is of given type
            if (arguments.size() != 2) {
                throw TypeError("is_type() requires exactly 2 arguments");
            }
            
            if (!std::holds_alternative<std::string>(arguments[1])) {
                throw TypeError("is_type() second argument must be a string");
            }
            
            std::string expected_type = std::get<std::string>(arguments[1]);
            const auto& val = arguments[0];
            bool matches = false;
            
            if (expected_type == "int" && std::holds_alternative<int>(val)) {
                matches = true;
            } else if (expected_type == "float" && std::holds_alternative<double>(val)) {
                matches = true;
            } else if (expected_type == "str" && std::holds_alternative<std::string>(val)) {
                matches = true;
            } else if (expected_type == "bool" && std::holds_alternative<bool>(val)) {
                matches = true;
            } else if (expected_type == "none" && std::holds_alternative<std::nullptr_t>(val)) {
                matches = true;
            } else if (expected_type == "array" && std::holds_alternative<std::shared_ptr<ArrayValue>>(val)) {
                matches = true;
            } else if (expected_type == "function" && std::holds_alternative<std::shared_ptr<Function>>(val)) {
                matches = true;
            } else if (expected_type == "class" && std::holds_alternative<std::shared_ptr<Class>>(val)) {
                matches = true;
            } else if (std::holds_alternative<std::shared_ptr<Instance>>(val)) {
                auto inst = std::get<std::shared_ptr<Instance>>(val);
                matches = (inst->klass->name == expected_type);
            }
            
            last_value = matches;
            return;
        } else if (func_name == "__builtin_has_field__") {
            // has_field(object, field_name) - Check if object has field
            if (arguments.size() != 2) {
                throw TypeError("has_field() requires exactly 2 arguments");
            }
            
            if (!std::holds_alternative<std::shared_ptr<Instance>>(arguments[0])) {
                last_value = false;
                return;
            }
            
            if (!std::holds_alternative<std::string>(arguments[1])) {
                throw TypeError("has_field() second argument must be a string");
            }
            
            auto inst = std::get<std::shared_ptr<Instance>>(arguments[0]);
            std::string field_name = std::get<std::string>(arguments[1]);
            
            last_value = (inst->fields.find(field_name) != inst->fields.end());
            return;
        } else if (func_name == "__builtin_has_method__") {
            // has_method(object, method_name) - Check if object has method
            if (arguments.size() != 2) {
                throw TypeError("has_method() requires exactly 2 arguments");
            }
            
            if (!std::holds_alternative<std::shared_ptr<Instance>>(arguments[0])) {
                last_value = false;
                return;
            }
            
            if (!std::holds_alternative<std::string>(arguments[1])) {
                throw TypeError("has_method() second argument must be a string");
            }
            
            auto inst = std::get<std::shared_ptr<Instance>>(arguments[0]);
            std::string method_name = std::get<std::string>(arguments[1]);
            
            last_value = (inst->klass->methods.find(method_name) != inst->klass->methods.end());
            return;
        
        // ===== MILESTONE 1: DYNAMIC ARRAYS =====
        } else if (func_name == "__builtin_array_create__") {
            // Create a new dynamic array
            void* array = sapphire_array_create();
            // Store as a raw pointer wrapped as an int (this is a temporary solution)
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(array));
            return;
            
        } else if (func_name == "__builtin_array_push__") {
            // Push element to array
            if (arguments.size() != 2) {
                throw TypeError("array_push() requires 2 arguments (array, value)");
            }
            
            void* array = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            
            if (std::holds_alternative<int>(arguments[1])) {
                sapphire_array_push_int(array, std::get<int>(arguments[1]));
            } else if (std::holds_alternative<double>(arguments[1])) {
                sapphire_array_push_double(array, std::get<double>(arguments[1]));
            } else if (std::holds_alternative<std::string>(arguments[1])) {
                sapphire_array_push_string(array, std::get<std::string>(arguments[1]).c_str());
            }
            
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_array_get__") {
            // Get element from array
            if (arguments.size() != 2) {
                throw TypeError("array_get() requires 2 arguments (array, index)");
            }
            
            void* array = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            int index = std::get<int>(arguments[1]);
            
            // For now, assume integer arrays - we'll need type information later
            last_value = static_cast<int>(sapphire_array_get_int(array, index));
            return;
            
        } else if (func_name == "__builtin_array_size__") {
            // Get array size
            if (arguments.size() != 1) {
                throw TypeError("array_size() requires 1 argument (array)");
            }
            
            void* array = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            last_value = static_cast<int>(sapphire_array_size(array));
            return;
            
        // ===== MILESTONE 1: MATH LIBRARY =====
        } else if (func_name == "__builtin_math_sqrt__") {
            if (arguments.size() != 1) {
                throw TypeError("math_sqrt() requires 1 argument");
            }
            double x = std::holds_alternative<int>(arguments[0]) ? 
                      std::get<int>(arguments[0]) : std::get<double>(arguments[0]);
            last_value = sapphire_math_sqrt(x);
            return;
            
        } else if (func_name == "__builtin_math_pow__") {
            if (arguments.size() != 2) {
                throw TypeError("math_pow() requires 2 arguments");
            }
            double x = std::holds_alternative<int>(arguments[0]) ? 
                      std::get<int>(arguments[0]) : std::get<double>(arguments[0]);
            double y = std::holds_alternative<int>(arguments[1]) ? 
                      std::get<int>(arguments[1]) : std::get<double>(arguments[1]);
            last_value = sapphire_math_pow(x, y);
            return;
            
        } else if (func_name == "__builtin_math_sin__") {
            if (arguments.size() != 1) {
                throw TypeError("math_sin() requires 1 argument");
            }
            double x = std::holds_alternative<int>(arguments[0]) ? 
                      std::get<int>(arguments[0]) : std::get<double>(arguments[0]);
            last_value = sapphire_math_sin(x);
            return;
            
        } else if (func_name == "__builtin_math_cos__") {
            if (arguments.size() != 1) {
                throw TypeError("math_cos() requires 1 argument");
            }
            double x = std::holds_alternative<int>(arguments[0]) ? 
                      std::get<int>(arguments[0]) : std::get<double>(arguments[0]);
            last_value = sapphire_math_cos(x);
            return;
            
        } else if (func_name == "__builtin_math_abs__") {
            if (arguments.size() != 1) {
                throw TypeError("math_abs() requires 1 argument");
            }
            double x = std::holds_alternative<int>(arguments[0]) ? 
                      std::get<int>(arguments[0]) : std::get<double>(arguments[0]);
            last_value = sapphire_math_abs(x);
            return;
            
        } else if (func_name == "__builtin_math_floor__") {
            if (arguments.size() != 1) {
                throw TypeError("math_floor() requires 1 argument");
            }
            double x = std::holds_alternative<int>(arguments[0]) ? 
                      std::get<int>(arguments[0]) : std::get<double>(arguments[0]);
            last_value = sapphire_math_floor(x);
            return;
            
        } else if (func_name == "__builtin_math_ceil__") {
            if (arguments.size() != 1) {
                throw TypeError("math_ceil() requires 1 argument");
            }
            double x = std::holds_alternative<int>(arguments[0]) ? 
                      std::get<int>(arguments[0]) : std::get<double>(arguments[0]);
            last_value = sapphire_math_ceil(x);
            return;
            
        } else if (func_name == "__builtin_math_pi__") {
            if (arguments.size() != 0) {
                throw TypeError("math_pi() requires 0 arguments");
            }
            last_value = sapphire_math_pi();
            return;
            
        } else if (func_name == "__builtin_math_e__") {
            if (arguments.size() != 0) {
                throw TypeError("math_e() requires 0 arguments");
            }
            last_value = sapphire_math_e();
            return;
            
        // ===== MILESTONE 1: CSV I/O =====
        } else if (func_name == "__builtin_csv_create__") {
            // Create a new CSV data structure
            void* csv = sapphire_csv_create();
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(csv));
            return;
            
        } else if (func_name == "__builtin_csv_read_file__") {
            if (arguments.size() != 1) {
                throw TypeError("csv_read_file() requires 1 argument (filename)");
            }
            
            std::string filename = std::get<std::string>(arguments[0]);
            void* csv = sapphire_csv_read_file(filename.c_str(), true);
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(csv));
            return;
            
        } else if (func_name == "__builtin_csv_row_count__") {
            if (arguments.size() != 1) {
                throw TypeError("csv_row_count() requires 1 argument (csv)");
            }
            
            void* csv = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            last_value = static_cast<int>(sapphire_csv_row_count(csv));
            return;
            
        } else if (func_name == "__builtin_csv_get__") {
            if (arguments.size() != 3) {
                throw TypeError("csv_get() requires 3 arguments (csv, row, col)");
            }
            
            void* csv = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            int row = std::get<int>(arguments[1]);
            int col = std::get<int>(arguments[2]);
            
            const char* value = sapphire_csv_get(csv, row, col);
            last_value = std::string(value ? value : "");
            return;
            
        // ===== MILESTONE 2: JSON SUPPORT =====
        } else if (func_name == "__builtin_json_parse__") {
            if (arguments.size() != 1) {
                throw TypeError("json_parse() requires 1 argument (json_string)");
            }
            
            std::string json_text = std::get<std::string>(arguments[0]);
            void* json_value = sapphire_json_parse(json_text.c_str());
            if (json_value == nullptr) {
                throw RuntimeError("Failed to parse JSON string");
            }
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(json_value));
            return;
            
        } else if (func_name == "__builtin_json_stringify__") {
            if (arguments.size() < 1 || arguments.size() > 2) {
                throw TypeError("json_stringify() requires 1-2 arguments (json_value, pretty=false)");
            }
            
            void* json_value = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            bool pretty = arguments.size() > 1 ? std::get<bool>(arguments[1]) : false;
            
            const char* json_string = sapphire_json_stringify(json_value, pretty);
            last_value = std::string(json_string ? json_string : "null");
            return;
            
        } else if (func_name == "__builtin_json_create_object__") {
            if (arguments.size() != 0) {
                throw TypeError("json_create_object() requires 0 arguments");
            }
            
            void* json_object = sapphire_json_create_object();
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(json_object));
            return;
            
        } else if (func_name == "__builtin_json_create_array__") {
            if (arguments.size() != 0) {
                throw TypeError("json_create_array() requires 0 arguments");
            }
            
            void* json_array = sapphire_json_create_array();
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(json_array));
            return;
            
        } else if (func_name == "__builtin_json_get__") {
            if (arguments.size() != 2) {
                throw TypeError("json_get() requires 2 arguments (json_object, key)");
            }
            
            void* json_object = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string key = std::get<std::string>(arguments[1]);
            
            void* result = sapphire_json_object_get(json_object, key.c_str());
            if (result == nullptr) {
                last_value = nullptr;
            } else {
                last_value = static_cast<int>(reinterpret_cast<intptr_t>(result));
            }
            return;
            
        } else if (func_name == "__builtin_json_set__") {
            if (arguments.size() != 3) {
                throw TypeError("json_set() requires 3 arguments (json_object, key, value)");
            }
            
            void* json_object = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string key = std::get<std::string>(arguments[1]);
            
            // Convert Sapphire value to JSON value
            void* json_value = nullptr;
            if (std::holds_alternative<std::nullptr_t>(arguments[2])) {
                json_value = sapphire_json_create_null();
            } else if (std::holds_alternative<bool>(arguments[2])) {
                json_value = sapphire_json_create_bool(std::get<bool>(arguments[2]));
            } else if (std::holds_alternative<int>(arguments[2])) {
                json_value = sapphire_json_create_number(static_cast<double>(std::get<int>(arguments[2])));
            } else if (std::holds_alternative<double>(arguments[2])) {
                json_value = sapphire_json_create_number(std::get<double>(arguments[2]));
            } else if (std::holds_alternative<std::string>(arguments[2])) {
                json_value = sapphire_json_create_string(std::get<std::string>(arguments[2]).c_str());
            } else {
                throw TypeError("Unsupported value type for JSON");
            }
            
            sapphire_json_object_set(json_object, key.c_str(), json_value);
            last_value = arguments[2];
            return;
            
        // ===== MILESTONE 2: RANDOM NUMBERS =====
        } else if (func_name == "__builtin_random_seed__") {
            if (arguments.size() != 1) {
                throw TypeError("random_seed() requires 1 argument (seed)");
            }
            
            uint64_t seed = static_cast<uint64_t>(std::get<int>(arguments[0]));
            sapphire_random_seed(seed);
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_random__") {
            if (arguments.size() > 2) {
                throw TypeError("random() requires 0-2 arguments (min=0.0, max=1.0)");
            }
            
            if (arguments.size() == 0) {
                last_value = sapphire_random_uniform();
            } else if (arguments.size() == 2) {
                double min = std::get<double>(arguments[0]);
                double max = std::get<double>(arguments[1]);
                last_value = sapphire_random_uniform_range(min, max);
            } else {
                throw TypeError("random() requires 0 or 2 arguments");
            }
            return;
            
        } else if (func_name == "__builtin_random_int__") {
            if (arguments.size() != 2) {
                throw TypeError("random_int() requires 2 arguments (min, max)");
            }
            
            int64_t min = static_cast<int64_t>(std::get<int>(arguments[0]));
            int64_t max = static_cast<int64_t>(std::get<int>(arguments[1]));
            last_value = static_cast<int>(sapphire_random_int(min, max));
            return;
            
        } else if (func_name == "__builtin_random_normal__") {
            if (arguments.size() > 2) {
                throw TypeError("random_normal() requires 0-2 arguments (mean=0.0, stddev=1.0)");
            }
            
            if (arguments.size() == 0) {
                last_value = sapphire_random_normal();
            } else if (arguments.size() == 2) {
                double mean = std::get<double>(arguments[0]);
                double stddev = std::get<double>(arguments[1]);
                last_value = sapphire_random_normal_params(mean, stddev);
            } else {
                throw TypeError("random_normal() requires 0 or 2 arguments");
            }
            return;
            
        } else if (func_name == "__builtin_random_boolean__") {
            if (arguments.size() > 1) {
                throw TypeError("random_boolean() requires 0-1 arguments (probability=0.5)");
            }
            
            if (arguments.size() == 0) {
                last_value = sapphire_random_boolean();
            } else {
                double probability = std::get<double>(arguments[0]);
                last_value = sapphire_random_boolean_prob(probability);
            }
            return;
            
        } else if (func_name == "__builtin_crypto_random__") {
            if (arguments.size() != 0) {
                throw TypeError("crypto_random() requires 0 arguments");
            }
            
            last_value = sapphire_crypto_random_uniform();
            return;
            
        } else if (func_name == "__builtin_crypto_random_int__") {
            if (arguments.size() != 2) {
                throw TypeError("crypto_random_int() requires 2 arguments (min, max)");
            }
            
            int64_t min = static_cast<int64_t>(std::get<int>(arguments[0]));
            int64_t max = static_cast<int64_t>(std::get<int>(arguments[1]));
            last_value = static_cast<int>(sapphire_crypto_random_int(min, max));
            return;
            
        // ===== MILESTONE 3: DATA VISUALIZATION =====
        } else if (func_name == "__builtin_plot_create__") {
            if (arguments.size() < 1 || arguments.size() > 3) {
                throw TypeError("plot_create() requires 1-3 arguments (chart_type, width=800, height=600)");
            }
            
            int chart_type = std::get<int>(arguments[0]);
            int width = arguments.size() > 1 ? std::get<int>(arguments[1]) : 800;
            int height = arguments.size() > 2 ? std::get<int>(arguments[2]) : 600;
            
            void* plot = sapphire_plot_create(width, height, chart_type);
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(plot));
            return;
            
        } else if (func_name == "__builtin_plot_add_line__") {
            if (arguments.size() < 3 || arguments.size() > 7) {
                throw TypeError("plot_add_line() requires 3-7 arguments (plot, x_array, y_array, label=\"\", r=0.0, g=0.0, b=1.0)");
            }
            
            void* plot = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            
            // Get arrays
            auto x_array = std::get<std::shared_ptr<ArrayValue>>(arguments[1]);
            auto y_array = std::get<std::shared_ptr<ArrayValue>>(arguments[2]);
            
            // Convert to double arrays
            std::vector<double> x_data, y_data;
            for (const auto& val : x_array->elements) {
                if (std::holds_alternative<int>(val)) {
                    x_data.push_back(static_cast<double>(std::get<int>(val)));
                } else if (std::holds_alternative<double>(val)) {
                    x_data.push_back(std::get<double>(val));
                }
            }
            for (const auto& val : y_array->elements) {
                if (std::holds_alternative<int>(val)) {
                    y_data.push_back(static_cast<double>(std::get<int>(val)));
                } else if (std::holds_alternative<double>(val)) {
                    y_data.push_back(std::get<double>(val));
                }
            }
            
            std::string label = arguments.size() > 3 ? std::get<std::string>(arguments[3]) : "";
            double r = arguments.size() > 4 ? std::get<double>(arguments[4]) : 0.0;
            double g = arguments.size() > 5 ? std::get<double>(arguments[5]) : 0.0;
            double b = arguments.size() > 6 ? std::get<double>(arguments[6]) : 1.0;
            
            sapphire_plot_add_line(plot, x_data.data(), y_data.data(), 
                                  static_cast<int>(std::min(x_data.size(), y_data.size())), 
                                  label.c_str(), r, g, b);
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_plot_add_scatter__") {
            if (arguments.size() < 3 || arguments.size() > 8) {
                throw TypeError("plot_add_scatter() requires 3-8 arguments (plot, x_array, y_array, label=\"\", r=1.0, g=0.0, b=0.0, marker=1)");
            }
            
            void* plot = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            
            // Get arrays
            auto x_array = std::get<std::shared_ptr<ArrayValue>>(arguments[1]);
            auto y_array = std::get<std::shared_ptr<ArrayValue>>(arguments[2]);
            
            // Convert to double arrays
            std::vector<double> x_data, y_data;
            for (const auto& val : x_array->elements) {
                if (std::holds_alternative<int>(val)) {
                    x_data.push_back(static_cast<double>(std::get<int>(val)));
                } else if (std::holds_alternative<double>(val)) {
                    y_data.push_back(std::get<double>(val));
                }
            }
            for (const auto& val : y_array->elements) {
                if (std::holds_alternative<int>(val)) {
                    y_data.push_back(static_cast<double>(std::get<int>(val)));
                } else if (std::holds_alternative<double>(val)) {
                    y_data.push_back(std::get<double>(val));
                }
            }
            
            std::string label = arguments.size() > 3 ? std::get<std::string>(arguments[3]) : "";
            double r = arguments.size() > 4 ? std::get<double>(arguments[4]) : 1.0;
            double g = arguments.size() > 5 ? std::get<double>(arguments[5]) : 0.0;
            double b = arguments.size() > 6 ? std::get<double>(arguments[6]) : 0.0;
            int marker = arguments.size() > 7 ? std::get<int>(arguments[7]) : 1;
            
            sapphire_plot_add_scatter(plot, x_data.data(), y_data.data(), 
                                     static_cast<int>(std::min(x_data.size(), y_data.size())), 
                                     label.c_str(), r, g, b, marker);
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_plot_add_histogram__") {
            if (arguments.size() < 2 || arguments.size() > 6) {
                throw TypeError("plot_add_histogram() requires 2-6 arguments (plot, data_array, bins=20, label=\"\", r=1.0, g=0.5, b=0.0)");
            }
            
            void* plot = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            
            // Get data array
            auto data_array = std::get<std::shared_ptr<ArrayValue>>(arguments[1]);
            
            // Convert to double array
            std::vector<double> data;
            for (const auto& val : data_array->elements) {
                if (std::holds_alternative<int>(val)) {
                    data.push_back(static_cast<double>(std::get<int>(val)));
                } else if (std::holds_alternative<double>(val)) {
                    data.push_back(std::get<double>(val));
                }
            }
            
            int bins = arguments.size() > 2 ? std::get<int>(arguments[2]) : 20;
            std::string label = arguments.size() > 3 ? std::get<std::string>(arguments[3]) : "";
            double r = arguments.size() > 4 ? std::get<double>(arguments[4]) : 1.0;
            double g = arguments.size() > 5 ? std::get<double>(arguments[5]) : 0.5;
            double b = arguments.size() > 6 ? std::get<double>(arguments[6]) : 0.0;
            
            sapphire_plot_add_histogram(plot, data.data(), static_cast<int>(data.size()), 
                                       bins, label.c_str(), r, g, b);
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_plot_set_title__") {
            if (arguments.size() != 2) {
                throw TypeError("plot_set_title() requires 2 arguments (plot, title)");
            }
            
            void* plot = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string title = std::get<std::string>(arguments[1]);
            
            sapphire_plot_set_title(plot, title.c_str());
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_plot_set_xlabel__") {
            if (arguments.size() != 2) {
                throw TypeError("plot_set_xlabel() requires 2 arguments (plot, label)");
            }
            
            void* plot = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string label = std::get<std::string>(arguments[1]);
            
            sapphire_plot_set_xlabel(plot, label.c_str());
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_plot_set_ylabel__") {
            if (arguments.size() != 2) {
                throw TypeError("plot_set_ylabel() requires 2 arguments (plot, label)");
            }
            
            void* plot = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string label = std::get<std::string>(arguments[1]);
            
            sapphire_plot_set_ylabel(plot, label.c_str());
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_plot_show__") {
            if (arguments.size() != 1) {
                throw TypeError("plot_show() requires 1 argument (plot)");
            }
            
            void* plot = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            sapphire_plot_show(plot);
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_plot_save_svg__") {
            if (arguments.size() != 2) {
                throw TypeError("plot_save_svg() requires 2 arguments (plot, filename)");
            }
            
            void* plot = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string filename = std::get<std::string>(arguments[1]);
            
            sapphire_plot_save_svg(plot, filename.c_str());
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_quick_plot__") {
            if (arguments.size() < 1 || arguments.size() > 2) {
                throw TypeError("quick_plot() requires 1-2 arguments (y_array, title=\"\")");
            }
            
            // Get data array
            auto y_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            
            // Convert to double array
            std::vector<double> y_data;
            for (const auto& val : y_array->elements) {
                if (std::holds_alternative<int>(val)) {
                    y_data.push_back(static_cast<double>(std::get<int>(val)));
                } else if (std::holds_alternative<double>(val)) {
                    y_data.push_back(std::get<double>(val));
                }
            }
            
            std::string title = arguments.size() > 1 ? std::get<std::string>(arguments[1]) : "";
            
            void* plot = sapphire_quick_plot(y_data.data(), static_cast<int>(y_data.size()), title.c_str());
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(plot));
            return;
            
        } else if (func_name == "__builtin_quick_scatter__") {
            if (arguments.size() < 2 || arguments.size() > 3) {
                throw TypeError("quick_scatter() requires 2-3 arguments (x_array, y_array, title=\"\")");
            }
            
            // Get arrays
            auto x_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            auto y_array = std::get<std::shared_ptr<ArrayValue>>(arguments[1]);
            
            // Convert to double arrays
            std::vector<double> x_data, y_data;
            for (const auto& val : x_array->elements) {
                if (std::holds_alternative<int>(val)) {
                    x_data.push_back(static_cast<double>(std::get<int>(val)));
                } else if (std::holds_alternative<double>(val)) {
                    x_data.push_back(std::get<double>(val));
                }
            }
            for (const auto& val : y_array->elements) {
                if (std::holds_alternative<int>(val)) {
                    y_data.push_back(static_cast<double>(std::get<int>(val)));
                } else if (std::holds_alternative<double>(val)) {
                    y_data.push_back(std::get<double>(val));
                }
            }
            
            std::string title = arguments.size() > 2 ? std::get<std::string>(arguments[2]) : "";
            
            void* plot = sapphire_quick_scatter(x_data.data(), y_data.data(), 
                                              static_cast<int>(std::min(x_data.size(), y_data.size())), 
                                              title.c_str());
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(plot));
            return;
            
        } else if (func_name == "__builtin_quick_histogram__") {
            if (arguments.size() < 1 || arguments.size() > 3) {
                throw TypeError("quick_histogram() requires 1-3 arguments (data_array, bins=20, title=\"\")");
            }
            
            // Get data array
            auto data_array = std::get<std::shared_ptr<ArrayValue>>(arguments[0]);
            
            // Convert to double array
            std::vector<double> data;
            for (const auto& val : data_array->elements) {
                if (std::holds_alternative<int>(val)) {
                    data.push_back(static_cast<double>(std::get<int>(val)));
                } else if (std::holds_alternative<double>(val)) {
                    data.push_back(std::get<double>(val));
                }
            }
            
            int bins = arguments.size() > 1 ? std::get<int>(arguments[1]) : 20;
            std::string title = arguments.size() > 2 ? std::get<std::string>(arguments[2]) : "";
            
            void* plot = sapphire_quick_histogram(data.data(), static_cast<int>(data.size()), 
                                                bins, title.c_str());
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(plot));
            return;
            
        // ===== MILESTONE 4: HTTP SERVER/CLIENT =====
        } else if (func_name == "__builtin_http_server_create__") {
            // Create HTTP server
            if (arguments.size() != 1) {
                throw TypeError("http_server_create() requires 1 argument (port)");
            }
            
            int port = std::get<int>(arguments[0]);
            void* server = http_server_create(port);
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(server));
            return;
            
        } else if (func_name == "__builtin_http_server_start__") {
            // Start HTTP server
            if (arguments.size() != 1) {
                throw TypeError("http_server_start() requires 1 argument (server)");
            }
            
            void* server = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            int result = http_server_start(server);
            last_value = (result == 1);
            return;
            
        } else if (func_name == "__builtin_http_server_stop__") {
            // Stop HTTP server
            if (arguments.size() != 1) {
                throw TypeError("http_server_stop() requires 1 argument (server)");
            }
            
            void* server = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            http_server_stop(server);
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_http_client_create__") {
            // Create HTTP client
            if (arguments.size() != 0) {
                throw TypeError("http_client_create() requires 0 arguments");
            }
            
            void* client = http_client_create();
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(client));
            return;
            
        } else if (func_name == "__builtin_http_client_get__") {
            // HTTP GET request
            if (arguments.size() != 2) {
                throw TypeError("http_client_get() requires 2 arguments (client, url)");
            }
            
            void* client = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string url = std::get<std::string>(arguments[1]);
            
            void* response = http_client_get(client, url.c_str());
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(response));
            return;
            
        } else if (func_name == "__builtin_http_client_post__") {
            // HTTP POST request
            if (arguments.size() != 3) {
                throw TypeError("http_client_post() requires 3 arguments (client, url, body)");
            }
            
            void* client = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string url = std::get<std::string>(arguments[1]);
            std::string body = std::get<std::string>(arguments[2]);
            
            void* response = http_client_post(client, url.c_str(), body.c_str());
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(response));
            return;
            
        } else if (func_name == "__builtin_http_response_status__") {
            // Get response status code
            if (arguments.size() != 1) {
                throw TypeError("http_response_status() requires 1 argument (response)");
            }
            
            void* response = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            int status = http_response_get_status(response);
            last_value = status;
            return;
            
        } else if (func_name == "__builtin_http_response_body__") {
            // Get response body
            if (arguments.size() != 1) {
                throw TypeError("http_response_body() requires 1 argument (response)");
            }
            
            void* response = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            const char* body = http_response_get_body(response);
            last_value = std::string(body ? body : "");
            return;
            
        } else if (func_name == "__builtin_http_response_header__") {
            // Get response header
            if (arguments.size() != 2) {
                throw TypeError("http_response_header() requires 2 arguments (response, header_name)");
            }
            
            void* response = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string header_name = std::get<std::string>(arguments[1]);
            
            const char* header_value = http_response_get_header(response, header_name.c_str());
            last_value = std::string(header_value ? header_value : "");
            return;
            
        } else if (func_name == "__builtin_http_url_encode__") {
            // URL encode string
            if (arguments.size() != 1) {
                throw TypeError("http_url_encode() requires 1 argument (string)");
            }
            
            std::string input = std::get<std::string>(arguments[0]);
            char* encoded = http_url_encode(input.c_str());
            last_value = std::string(encoded);
            http_free_string(encoded);
            return;
            
        } else if (func_name == "__builtin_http_url_decode__") {
            // URL decode string
            if (arguments.size() != 1) {
                throw TypeError("http_url_decode() requires 1 argument (string)");
            }
            
            std::string input = std::get<std::string>(arguments[0]);
            char* decoded = http_url_decode(input.c_str());
            last_value = std::string(decoded);
            http_free_string(decoded);
            return;
            
        } else if (func_name == "__builtin_http_html_escape__") {
            // HTML escape string
            if (arguments.size() != 1) {
                throw TypeError("http_html_escape() requires 1 argument (string)");
            }
            
            std::string input = std::get<std::string>(arguments[0]);
            char* escaped = http_html_escape(input.c_str());
            last_value = std::string(escaped);
            http_free_string(escaped);
            return;
            
        // ===== WEBSOCKET SUPPORT =====
        } else if (func_name == "__builtin_websocket_server_create__") {
            // Create WebSocket server
            if (arguments.size() != 1) {
                throw TypeError("websocket_server_create() requires 1 argument (port)");
            }
            
            int port = std::get<int>(arguments[0]);
            void* server = sapphire_websocket_server_create(port);
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(server));
            return;
            
        } else if (func_name == "__builtin_websocket_server_start__") {
            // Start WebSocket server
            if (arguments.size() != 1) {
                throw TypeError("websocket_server_start() requires 1 argument (server)");
            }
            
            void* server = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            int result = sapphire_websocket_server_start(server);
            last_value = (result == 1);
            return;
            
        } else if (func_name == "__builtin_websocket_server_stop__") {
            // Stop WebSocket server
            if (arguments.size() != 1) {
                throw TypeError("websocket_server_stop() requires 1 argument (server)");
            }
            
            void* server = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            sapphire_websocket_server_stop(server);
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_websocket_server_broadcast_text__") {
            // Broadcast text to all connections
            if (arguments.size() != 2) {
                throw TypeError("websocket_server_broadcast_text() requires 2 arguments (server, text)");
            }
            
            void* server = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string text = std::get<std::string>(arguments[1]);
            sapphire_websocket_server_broadcast_text(server, text.c_str());
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_websocket_server_broadcast_binary__") {
            // Broadcast binary data to all connections
            if (arguments.size() != 2) {
                throw TypeError("websocket_server_broadcast_binary() requires 2 arguments (server, data)");
            }
            
            void* server = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string data = std::get<std::string>(arguments[1]);
            sapphire_websocket_server_broadcast_binary(server, 
                reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_websocket_server_connection_count__") {
            // Get connection count
            if (arguments.size() != 1) {
                throw TypeError("websocket_server_connection_count() requires 1 argument (server)");
            }
            
            void* server = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            int count = sapphire_websocket_server_connection_count(server);
            last_value = count;
            return;
            
        } else if (func_name == "__builtin_websocket_client_create__") {
            // Create WebSocket client
            if (arguments.size() != 0) {
                throw TypeError("websocket_client_create() requires 0 arguments");
            }
            
            void* client = sapphire_websocket_client_create();
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(client));
            return;
            
        } else if (func_name == "__builtin_websocket_client_connect__") {
            // Connect WebSocket client
            if (arguments.size() != 2) {
                throw TypeError("websocket_client_connect() requires 2 arguments (client, url)");
            }
            
            void* client = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string url = std::get<std::string>(arguments[1]);
            int result = sapphire_websocket_client_connect(client, url.c_str());
            last_value = (result == 1);
            return;
            
        } else if (func_name == "__builtin_websocket_client_disconnect__") {
            // Disconnect WebSocket client
            if (arguments.size() != 1) {
                throw TypeError("websocket_client_disconnect() requires 1 argument (client)");
            }
            
            void* client = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            sapphire_websocket_client_disconnect(client);
            last_value = nullptr;
            return;
            
        } else if (func_name == "__builtin_websocket_client_send_text__") {
            // Send text message
            if (arguments.size() != 2) {
                throw TypeError("websocket_client_send_text() requires 2 arguments (client, text)");
            }
            
            void* client = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string text = std::get<std::string>(arguments[1]);
            int result = sapphire_websocket_client_send_text(client, text.c_str());
            last_value = (result == 1);
            return;
            
        } else if (func_name == "__builtin_websocket_client_send_binary__") {
            // Send binary data
            if (arguments.size() != 2) {
                throw TypeError("websocket_client_send_binary() requires 2 arguments (client, data)");
            }
            
            void* client = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string data = std::get<std::string>(arguments[1]);
            int result = sapphire_websocket_client_send_binary(client, 
                reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
            last_value = (result == 1);
            return;
            
        } else if (func_name == "__builtin_websocket_client_is_connected__") {
            // Check if client is connected
            if (arguments.size() != 1) {
                throw TypeError("websocket_client_is_connected() requires 1 argument (client)");
            }
            
            void* client = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            int result = sapphire_websocket_client_is_connected(client);
            last_value = (result == 1);
            return;
            
        } else if (func_name == "__builtin_websocket_base64_encode__") {
            // Base64 encode data
            if (arguments.size() != 1) {
                throw TypeError("websocket_base64_encode() requires 1 argument (data)");
            }
            
            std::string data = std::get<std::string>(arguments[0]);
            char* encoded = sapphire_websocket_base64_encode(
                reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
            last_value = std::string(encoded);
            sapphire_websocket_free_string(encoded);
            return;
            
        } else if (func_name == "__builtin_websocket_base64_decode__") {
            // Base64 decode data
            if (arguments.size() != 1) {
                throw TypeError("websocket_base64_decode() requires 1 argument (encoded)");
            }
            
            std::string encoded = std::get<std::string>(arguments[0]);
            int size;
            uint8_t* decoded = sapphire_websocket_base64_decode(encoded.c_str(), &size);
            last_value = std::string(reinterpret_cast<char*>(decoded), size);
            sapphire_websocket_free_data(decoded);
            return;
            
        // ===== TEMPLATE ENGINE =====
        } else if (func_name == "__builtin_html_template_engine_create__") {
            // Create template engine
            if (arguments.size() != 0) {
                throw TypeError("html_template_engine_create() requires 0 arguments");
            }
            
            void* engine = sapphire_html_template_engine_create();
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(engine));
            return;
            
        } else if (func_name == "__builtin_html_template_render__") {
            // Render template string
            if (arguments.size() != 3) {
                throw TypeError("html_template_render() requires 3 arguments (engine, template, context)");
            }
            
            void* engine = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string template_str = std::get<std::string>(arguments[1]);
            std::string context_str = std::get<std::string>(arguments[2]);
            
            // Simple context parsing (key=value pairs separated by ;)
            std::vector<const char*> keys, values;
            std::vector<std::string> key_storage, value_storage;
            
            size_t pos = 0;
            while (pos < context_str.length()) {
                size_t eq_pos = context_str.find('=', pos);
                if (eq_pos == std::string::npos) break;
                
                size_t semi_pos = context_str.find(';', eq_pos);
                if (semi_pos == std::string::npos) semi_pos = context_str.length();
                
                key_storage.push_back(context_str.substr(pos, eq_pos - pos));
                value_storage.push_back(context_str.substr(eq_pos + 1, semi_pos - eq_pos - 1));
                
                keys.push_back(key_storage.back().c_str());
                values.push_back(value_storage.back().c_str());
                
                pos = semi_pos + 1;
            }
            
            char* result = sapphire_html_template_render(engine, template_str.c_str(), 
                keys.data(), values.data(), keys.size());
            last_value = std::string(result);
            sapphire_html_template_free_string(result);
            return;
            
        // ===== DATABASE DRIVERS =====
        } else if (func_name == "__builtin_postgresql_create__") {
            // Create PostgreSQL connection
            if (arguments.size() != 0) {
                throw TypeError("postgresql_create() requires 0 arguments");
            }
            
            void* conn = sapphire_postgresql_create();
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(conn));
            return;
            
        } else if (func_name == "__builtin_postgresql_connect__") {
            // Connect to PostgreSQL
            if (arguments.size() != 2) {
                throw TypeError("postgresql_connect() requires 2 arguments (connection, connection_string)");
            }
            
            void* conn = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string conn_str = std::get<std::string>(arguments[1]);
            int result = sapphire_postgresql_connect(conn, conn_str.c_str());
            last_value = (result == 1);
            return;
            
        } else if (func_name == "__builtin_postgresql_query__") {
            // Execute PostgreSQL query
            if (arguments.size() != 2) {
                throw TypeError("postgresql_query() requires 2 arguments (connection, query)");
            }
            
            void* conn = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string query = std::get<std::string>(arguments[1]);
            char* result = sapphire_postgresql_query(conn, query.c_str());
            last_value = std::string(result);
            sapphire_database_free_result(result);
            return;
            
        } else if (func_name == "__builtin_mysql_create__") {
            // Create MySQL connection
            if (arguments.size() != 0) {
                throw TypeError("mysql_create() requires 0 arguments");
            }
            
            void* conn = sapphire_mysql_create();
            last_value = static_cast<int>(reinterpret_cast<intptr_t>(conn));
            return;
            
        } else if (func_name == "__builtin_mysql_connect__") {
            // Connect to MySQL
            if (arguments.size() != 2) {
                throw TypeError("mysql_connect() requires 2 arguments (connection, connection_string)");
            }
            
            void* conn = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string conn_str = std::get<std::string>(arguments[1]);
            int result = sapphire_mysql_connect(conn, conn_str.c_str());
            last_value = (result == 1);
            return;
            
        } else if (func_name == "__builtin_mysql_query__") {
            // Execute MySQL query
            if (arguments.size() != 2) {
                throw TypeError("mysql_query() requires 2 arguments (connection, query)");
            }
            
            void* conn = reinterpret_cast<void*>(static_cast<intptr_t>(std::get<int>(arguments[0])));
            std::string query = std::get<std::string>(arguments[1]);
            char* result = sapphire_mysql_query(conn, query.c_str());
            last_value = std::string(result);
            sapphire_database_free_result(result);
            return;
        }
    }
    
    // Handle WaitGroup methods
    if (std::holds_alternative<std::shared_ptr<WaitGroupMethod>>(callee)) {
        auto wgm = std::get<std::shared_ptr<WaitGroupMethod>>(callee);
        
        if (wgm->method_name == "add") {
            if (arguments.size() != 1) {
                throw TypeError("WaitGroup.add() requires 1 argument");
            }
            int delta = std::get<int>(arguments[0]);
            wgm->waitgroup->add(delta);
            last_value = nullptr;
            return;
        } else if (wgm->method_name == "done") {
            if (!arguments.empty()) {
                throw TypeError("WaitGroup.done() takes no arguments");
            }
            wgm->waitgroup->done();
            last_value = nullptr;
            return;
        } else if (wgm->method_name == "wait") {
            if (!arguments.empty()) {
                throw TypeError("WaitGroup.wait() takes no arguments");
            }
            wgm->waitgroup->wait();
            last_value = nullptr;
            return;
        } else if (wgm->method_name == "count") {
            if (!arguments.empty()) {
                throw TypeError("WaitGroup.count() takes no arguments");
            }
            last_value = wgm->waitgroup->count();
            return;
        }
        
        throw RuntimeError("Unknown WaitGroup method: " + wgm->method_name);
    }
    
    // Handle Option methods
    if (std::holds_alternative<std::shared_ptr<OptionMethod>>(callee)) {
        auto om = std::get<std::shared_ptr<OptionMethod>>(callee);
        
        if (om->method_name == "is_some") {
            if (!arguments.empty()) {
                throw TypeError("Option.is_some() takes no arguments");
            }
            last_value = om->option->isSome();
            return;
        } else if (om->method_name == "is_none") {
            if (!arguments.empty()) {
                throw TypeError("Option.is_none() takes no arguments");
            }
            last_value = om->option->isNone();
            return;
        } else if (om->method_name == "unwrap") {
            if (!arguments.empty()) {
                throw TypeError("Option.unwrap() takes no arguments");
            }
            last_value = om->option->unwrap();
            return;
        } else if (om->method_name == "unwrap_or") {
            if (arguments.size() != 1) {
                throw TypeError("Option.unwrap_or() requires 1 argument");
            }
            last_value = om->option->unwrapOr(arguments[0]);
            return;
        } else if (om->method_name == "map") {
            if (arguments.size() != 1) {
                throw TypeError("Option.map() requires 1 argument (function)");
            }
            
            if (om->option->isNone()) {
                // None.map(f) = None
                last_value = om->option;
                return;
            }
            
            // Some(x).map(f) = Some(f(x))
            if (!std::holds_alternative<std::shared_ptr<Function>>(arguments[0])) {
                throw TypeError("Option.map() requires a function argument");
            }
            
            auto func = std::get<std::shared_ptr<Function>>(arguments[0]);
            std::vector<Value> func_args = {om->option->value};
            
            // Call the function with the wrapped value
            if (func_args.size() != func->parameters.size()) {
                throw TypeError("Function expects " + std::to_string(func->parameters.size()) + 
                                " arguments but got " + std::to_string(func_args.size()));
            }
            
            auto func_env = std::make_shared<Environment>(func->closure);
            for (size_t i = 0; i < func->parameters.size(); i++) {
                func_env->define(func->parameters[i].first, func_args[i]);
            }
            
            auto previous_env = environment;
            environment = func_env;
            
            Value result = nullptr;
            try {
                for (auto& stmt : *func->getBody()) {
                    stmt->accept(*this);
                }
            } catch (const ReturnException& ret) {
                result = ret.value;
            }
            
            environment = previous_env;
            
            // Wrap result in Some
            auto new_option = std::make_shared<OptionValue>(result);
            last_value = new_option;
            return;
        } else if (om->method_name == "and_then") {
            if (arguments.size() != 1) {
                throw TypeError("Option.and_then() requires 1 argument (function)");
            }
            
            if (om->option->isNone()) {
                // None.and_then(f) = None
                last_value = om->option;
                return;
            }
            
            // Some(x).and_then(f) = f(x) (f must return Option)
            if (!std::holds_alternative<std::shared_ptr<Function>>(arguments[0])) {
                throw TypeError("Option.and_then() requires a function argument");
            }
            
            auto func = std::get<std::shared_ptr<Function>>(arguments[0]);
            std::vector<Value> func_args = {om->option->value};
            
            if (func_args.size() != func->parameters.size()) {
                throw TypeError("Function expects " + std::to_string(func->parameters.size()) + 
                                " arguments but got " + std::to_string(func_args.size()));
            }
            
            auto func_env = std::make_shared<Environment>(func->closure);
            for (size_t i = 0; i < func->parameters.size(); i++) {
                func_env->define(func->parameters[i].first, func_args[i]);
            }
            
            auto previous_env = environment;
            environment = func_env;
            
            Value result = nullptr;
            try {
                for (auto& stmt : *func->getBody()) {
                    stmt->accept(*this);
                }
            } catch (const ReturnException& ret) {
                result = ret.value;
            }
            
            environment = previous_env;
            
            // Result should already be an Option
            last_value = result;
            return;
        }
        
        throw RuntimeError("Unknown Option method: " + om->method_name);
    }
    
    // Handle Result methods
    if (std::holds_alternative<std::shared_ptr<ResultMethod>>(callee)) {
        auto rm = std::get<std::shared_ptr<ResultMethod>>(callee);
        
        if (rm->method_name == "is_ok") {
            if (!arguments.empty()) {
                throw TypeError("Result.is_ok() takes no arguments");
            }
            last_value = rm->result->isOk();
            return;
        } else if (rm->method_name == "is_err") {
            if (!arguments.empty()) {
                throw TypeError("Result.is_err() takes no arguments");
            }
            last_value = rm->result->isErr();
            return;
        } else if (rm->method_name == "unwrap") {
            if (!arguments.empty()) {
                throw TypeError("Result.unwrap() takes no arguments");
            }
            last_value = rm->result->unwrap();
            return;
        } else if (rm->method_name == "unwrap_or") {
            if (arguments.size() != 1) {
                throw TypeError("Result.unwrap_or() requires 1 argument");
            }
            last_value = rm->result->unwrapOr(arguments[0]);
            return;
        } else if (rm->method_name == "unwrap_err") {
            if (!arguments.empty()) {
                throw TypeError("Result.unwrap_err() takes no arguments");
            }
            last_value = rm->result->unwrapErr();
            return;
        } else if (rm->method_name == "map") {
            if (arguments.size() != 1) {
                throw TypeError("Result.map() requires 1 argument (function)");
            }
            
            if (rm->result->isErr()) {
                // Err.map(f) = Err (unchanged)
                last_value = rm->result;
                return;
            }
            
            // Ok(x).map(f) = Ok(f(x))
            if (!std::holds_alternative<std::shared_ptr<Function>>(arguments[0])) {
                throw TypeError("Result.map() requires a function argument");
            }
            
            auto func = std::get<std::shared_ptr<Function>>(arguments[0]);
            std::vector<Value> func_args = {rm->result->value};
            
            if (func_args.size() != func->parameters.size()) {
                throw TypeError("Function expects " + std::to_string(func->parameters.size()) + 
                                " arguments but got " + std::to_string(func_args.size()));
            }
            
            auto func_env = std::make_shared<Environment>(func->closure);
            for (size_t i = 0; i < func->parameters.size(); i++) {
                func_env->define(func->parameters[i].first, func_args[i]);
            }
            
            auto previous_env = environment;
            environment = func_env;
            
            Value result = nullptr;
            try {
                for (auto& stmt : *func->getBody()) {
                    stmt->accept(*this);
                }
            } catch (const ReturnException& ret) {
                result = ret.value;
            }
            
            environment = previous_env;
            
            // Wrap result in Ok
            auto new_result = std::make_shared<ResultValue>(true, result);
            last_value = new_result;
            return;
        } else if (rm->method_name == "map_err") {
            if (arguments.size() != 1) {
                throw TypeError("Result.map_err() requires 1 argument (function)");
            }
            
            if (rm->result->isOk()) {
                // Ok.map_err(f) = Ok (unchanged)
                last_value = rm->result;
                return;
            }
            
            // Err(e).map_err(f) = Err(f(e))
            if (!std::holds_alternative<std::shared_ptr<Function>>(arguments[0])) {
                throw TypeError("Result.map_err() requires a function argument");
            }
            
            auto func = std::get<std::shared_ptr<Function>>(arguments[0]);
            std::vector<Value> func_args = {rm->result->value};
            
            if (func_args.size() != func->parameters.size()) {
                throw TypeError("Function expects " + std::to_string(func->parameters.size()) + 
                                " arguments but got " + std::to_string(func_args.size()));
            }
            
            auto func_env = std::make_shared<Environment>(func->closure);
            for (size_t i = 0; i < func->parameters.size(); i++) {
                func_env->define(func->parameters[i].first, func_args[i]);
            }
            
            auto previous_env = environment;
            environment = func_env;
            
            Value result = nullptr;
            try {
                for (auto& stmt : *func->getBody()) {
                    stmt->accept(*this);
                }
            } catch (const ReturnException& ret) {
                result = ret.value;
            }
            
            environment = previous_env;
            
            // Wrap result in Err
            auto new_result = std::make_shared<ResultValue>(false, result);
            last_value = new_result;
            return;
        } else if (rm->method_name == "and_then") {
            if (arguments.size() != 1) {
                throw TypeError("Result.and_then() requires 1 argument (function)");
            }
            
            if (rm->result->isErr()) {
                // Err.and_then(f) = Err (unchanged)
                last_value = rm->result;
                return;
            }
            
            // Ok(x).and_then(f) = f(x) (f must return Result)
            if (!std::holds_alternative<std::shared_ptr<Function>>(arguments[0])) {
                throw TypeError("Result.and_then() requires a function argument");
            }
            
            auto func = std::get<std::shared_ptr<Function>>(arguments[0]);
            std::vector<Value> func_args = {rm->result->value};
            
            if (func_args.size() != func->parameters.size()) {
                throw TypeError("Function expects " + std::to_string(func->parameters.size()) + 
                                " arguments but got " + std::to_string(func_args.size()));
            }
            
            auto func_env = std::make_shared<Environment>(func->closure);
            for (size_t i = 0; i < func->parameters.size(); i++) {
                func_env->define(func->parameters[i].first, func_args[i]);
            }
            
            auto previous_env = environment;
            environment = func_env;
            
            Value result = nullptr;
            try {
                for (auto& stmt : *func->getBody()) {
                    stmt->accept(*this);
                }
            } catch (const ReturnException& ret) {
                result = ret.value;
            }
            
            environment = previous_env;
            
            // Result should already be a Result
            last_value = result;
            return;
        }
        
        throw RuntimeError("Unknown Result method: " + rm->method_name);
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
    
    // Handle channel methods
    if (std::holds_alternative<std::shared_ptr<ChannelValue>>(object)) {
        auto channel = std::get<std::shared_ptr<ChannelValue>>(object);
        
        if (expr.name == "close" || expr.name == "is_closed" || 
            expr.name == "empty" || expr.name == "size") {
            // Return a bound channel method
            last_value = std::make_shared<ChannelMethod>(channel, expr.name);
            return;
        }
        
        throw RuntimeError("Undefined method '" + expr.name + "' on channel");
    }
    
    // Handle WaitGroup methods
    if (std::holds_alternative<std::shared_ptr<WaitGroupValue>>(object)) {
        auto wg = std::get<std::shared_ptr<WaitGroupValue>>(object);
        
        if (expr.name == "add" || expr.name == "done" || 
            expr.name == "wait" || expr.name == "count") {
            // Return a bound WaitGroup method
            last_value = std::make_shared<WaitGroupMethod>(wg, expr.name);
            return;
        }
        
        throw RuntimeError("Undefined method '" + expr.name + "' on WaitGroup");
    }
    
    // Handle Option methods
    if (std::holds_alternative<std::shared_ptr<OptionValue>>(object)) {
        auto option = std::get<std::shared_ptr<OptionValue>>(object);
        
        if (expr.name == "is_some" || expr.name == "is_none" || 
            expr.name == "unwrap" || expr.name == "unwrap_or" ||
            expr.name == "map" || expr.name == "and_then") {
            // Return a bound Option method
            last_value = std::make_shared<OptionMethod>(option, expr.name);
            return;
        }
        
        throw RuntimeError("Undefined method '" + expr.name + "' on Option");
    }
    
    // Handle Result methods
    if (std::holds_alternative<std::shared_ptr<ResultValue>>(object)) {
        auto result = std::get<std::shared_ptr<ResultValue>>(object);
        
        if (expr.name == "is_ok" || expr.name == "is_err" || 
            expr.name == "unwrap" || expr.name == "unwrap_or" ||
            expr.name == "unwrap_err" || expr.name == "map" || 
            expr.name == "map_err" || expr.name == "and_then") {
            // Return a bound Result method
            last_value = std::make_shared<ResultMethod>(result, expr.name);
            return;
        }
        
        throw RuntimeError("Undefined method '" + expr.name + "' on Result");
    }
    
    // Handle static method access on Class (e.g., MyClass.static_method)
    if (std::holds_alternative<std::shared_ptr<Class>>(object)) {
        auto cls = std::get<std::shared_ptr<Class>>(object);
        
        // Look for method
        auto method = cls->findMethod(expr.name);
        if (method) {
            // Check if it's a static or class method
            bool is_static = cls->static_methods.find(expr.name) != cls->static_methods.end();
            bool is_classmethod = cls->class_methods.find(expr.name) != cls->class_methods.end();
            
            if (is_static || is_classmethod) {
                last_value = std::make_shared<StaticMethod>(cls, method, is_classmethod);
                return;
            }
            
            throw RuntimeError("Cannot access instance method '" + expr.name + "' on class '" + cls->name + "'");
        }
        
        throw RuntimeError("Undefined method '" + expr.name + "' on class '" + cls->name + "'");
    }
    
    // Handle array methods
    if (std::holds_alternative<std::shared_ptr<ArrayValue>>(object)) {
        auto array = std::get<std::shared_ptr<ArrayValue>>(object);
        
        if (expr.name == "push") {
            last_value = std::make_shared<ArrayMethod>(array, "push");
            return;
        } else if (expr.name == "pop") {
            last_value = std::make_shared<ArrayMethod>(array, "pop");
            return;
        } else if (expr.name == "length") {
            last_value = static_cast<int>(array->elements.size());
            return;
        } else if (expr.name == "size") {
            last_value = static_cast<int>(array->elements.size());
            return;
        }
        
        throw RuntimeError("Unknown array property '" + expr.name + "'");
    }
    
    // Handle string methods
    if (std::holds_alternative<std::string>(object)) {
        std::string str = std::get<std::string>(object);
        
        if (expr.name == "length") {
            last_value = static_cast<int>(str.length());
            return;
        } else if (expr.name == "size") {
            last_value = static_cast<int>(str.length());
            return;
        } else if (expr.name == "upper") {
            last_value = std::make_shared<StringMethod>(str, "upper");
            return;
        } else if (expr.name == "lower") {
            last_value = std::make_shared<StringMethod>(str, "lower");
            return;
        } else if (expr.name == "split") {
            last_value = std::make_shared<StringMethod>(str, "split");
            return;
        }
        
        throw RuntimeError("Unknown string property '" + expr.name + "'");
    }
    
    // Handle hash map methods
    if (std::holds_alternative<std::shared_ptr<HashMapValue>>(object)) {
        auto hashmap = std::get<std::shared_ptr<HashMapValue>>(object);
        
        if (expr.name == "get") {
            last_value = std::make_shared<HashMapMethod>(hashmap, "get");
            return;
        } else if (expr.name == "set") {
            last_value = std::make_shared<HashMapMethod>(hashmap, "set");
            return;
        } else if (expr.name == "has") {
            last_value = std::make_shared<HashMapMethod>(hashmap, "has");
            return;
        } else if (expr.name == "remove") {
            last_value = std::make_shared<HashMapMethod>(hashmap, "remove");
            return;
        } else if (expr.name == "keys") {
            last_value = std::make_shared<HashMapMethod>(hashmap, "keys");
            return;
        } else if (expr.name == "values") {
            last_value = std::make_shared<HashMapMethod>(hashmap, "values");
            return;
        } else if (expr.name == "size") {
            last_value = static_cast<int>(hashmap->pairs.size());
            return;
        } else if (expr.name == "length") {
            last_value = static_cast<int>(hashmap->pairs.size());
            return;
        }
        
        throw RuntimeError("Unknown hash map property '" + expr.name + "'");
    }
    
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
            // Check if it's a static or class method
            bool is_static = instance->klass->static_methods.find(expr.name) != instance->klass->static_methods.end();
            bool is_classmethod = instance->klass->class_methods.find(expr.name) != instance->klass->class_methods.end();
            
            if (is_static || is_classmethod) {
                // Static/class methods can be called on instances too
                last_value = std::make_shared<StaticMethod>(instance->klass, method, is_classmethod);
                return;
            }
            
            // Regular instance method
            last_value = std::make_shared<BoundMethod>(instance, method);
            return;
        }
        
        // Finally check trait methods
        auto trait_method = findTraitMethod(instance->klass->name, expr.name);
        if (trait_method) {
            last_value = std::make_shared<BoundMethod>(instance, trait_method);
            return;
        }
        
        throw RuntimeError("Undefined property '" + expr.name + "' on instance of '" + instance->klass->name + "'");
    }
    
    throw TypeError("Only instances and classes have properties");
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

void Interpreter::visitIndexAssignExpr(IndexAssignExpr& expr) {
    Value object = evaluateExpr(*expr.object);
    Value index = evaluateExpr(*expr.index);
    Value value = evaluateExpr(*expr.value);
    
    // Handle array index assignment
    if (std::holds_alternative<std::shared_ptr<ArrayValue>>(object)) {
        auto array = std::get<std::shared_ptr<ArrayValue>>(object);
        
        if (std::holds_alternative<int>(index)) {
            int idx = std::get<int>(index);
            if (idx >= 0 && idx < static_cast<int>(array->elements.size())) {
                array->elements[idx] = value;
                last_value = value;
                return;
            } else {
                throw RuntimeError("Array index out of bounds");
            }
        } else {
            throw TypeError("Array index must be an integer");
        }
    }
    
    throw TypeError("Only arrays support index assignment");
}

void Interpreter::visitHashMapExpr(HashMapExpr& expr) {
    auto hashmap = std::make_shared<HashMapValue>();
    
    // Evaluate all key-value pairs
    for (const auto& pair : expr.pairs) {
        Value key = evaluateExpr(*pair.first);
        Value value = evaluateExpr(*pair.second);
        
        // Convert key to string
        std::string keyStr;
        if (std::holds_alternative<std::string>(key)) {
            keyStr = std::get<std::string>(key);
        } else {
            keyStr = valueToString(key);
        }
        
        hashmap->pairs[keyStr] = value;
    }
    
    last_value = hashmap;
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
        environment,  // Capture current environment as closure
        stmt.is_async  // Pass async flag
    );
    
    // If no decorators, just define the function
    if (stmt.decorators.empty()) {
        environment->define(stmt.name, func);
        return;
    }
    
    // Apply decorators in reverse order (bottom to top)
    Value decorated_func = func;
    for (auto it = stmt.decorators.rbegin(); it != stmt.decorators.rend(); ++it) {
        auto& decorator = *it;
        
        // Look up decorator function
        Value decorator_func = environment->get(decorator.name);
        
        // Handle built-in decorators
        if (std::holds_alternative<std::string>(decorator_func)) {
            std::string builtin_name = std::get<std::string>(decorator_func);
            
            if (builtin_name == "__builtin_cache__") {
                // Mark function as cached - we'll handle caching in visitCallExpr
                if (!std::holds_alternative<std::shared_ptr<Function>>(decorated_func)) {
                    throw RuntimeError("@cache can only be applied to functions");
                }
                auto f = std::get<std::shared_ptr<Function>>(decorated_func);
                // Store a marker that this function should be cached
                // We'll use a special naming convention: prepend "__cached__" to the name
                f->name = "__cached__" + f->name;
                continue;
            }
            
            if (builtin_name == "__builtin_timing__") {
                // Mark function for timing - we'll handle timing in visitCallExpr
                if (!std::holds_alternative<std::shared_ptr<Function>>(decorated_func)) {
                    throw RuntimeError("@timing can only be applied to functions");
                }
                auto f = std::get<std::shared_ptr<Function>>(decorated_func);
                f->name = "__timed__" + f->name;
                continue;
            }
            
            if (builtin_name == "__builtin_deprecated__") {
                // Print deprecation warning
                if (!std::holds_alternative<std::shared_ptr<Function>>(decorated_func)) {
                    throw RuntimeError("@deprecated can only be applied to functions");
                }
                auto f = std::get<std::shared_ptr<Function>>(decorated_func);
                
                // Get deprecation message if provided
                std::string message = "Function '" + stmt.name + "' is deprecated";
                if (!decorator.arguments.empty()) {
                    Value msg_val = evaluateExpr(*decorator.arguments[0]);
                    if (std::holds_alternative<std::string>(msg_val)) {
                        message = std::get<std::string>(msg_val);
                    }
                }
                
                // Mark function as deprecated
                f->name = "__deprecated__" + f->name + "__" + message;
                continue;
            }
        }
        
        // Evaluate decorator arguments
        std::vector<Value> args;
        args.push_back(decorated_func);  // First argument is the function being decorated
        
        for (auto& arg_expr : decorator.arguments) {
            args.push_back(evaluateExpr(*arg_expr));
        }
        
        // Apply decorator (call it with the function and arguments)
        if (std::holds_alternative<std::shared_ptr<Function>>(decorator_func)) {
            auto dec_fn = std::get<std::shared_ptr<Function>>(decorator_func);
            
            // Create environment for decorator call
            auto dec_env = std::make_shared<Environment>(dec_fn->closure);
            
            // Bind parameters
            if (args.size() != dec_fn->parameters.size()) {
                throw RuntimeError("Decorator '" + decorator.name + "' expects " +
                                   std::to_string(dec_fn->parameters.size()) + " arguments but got " +
                                   std::to_string(args.size()));
            }
            
            for (size_t i = 0; i < dec_fn->parameters.size(); i++) {
                dec_env->define(dec_fn->parameters[i].first, args[i]);
            }
            
            auto previous_env = environment;
            environment = dec_env;
            
            try {
                for (auto& s : *dec_fn->getBody()) {
                    s->accept(*this);
                }
                decorated_func = nullptr;  // No return
            } catch (const ReturnException& ret) {
                decorated_func = ret.value;
            }
            
            environment = previous_env;
        } else {
            throw RuntimeError("'" + decorator.name + "' is not a function and cannot be used as a decorator");
        }
    }
    
    environment->define(stmt.name, decorated_func);
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
    
    // Handle channel iteration
    if (std::holds_alternative<std::shared_ptr<ChannelValue>>(iterable)) {
        auto channel = std::get<std::shared_ptr<ChannelValue>>(iterable);
        
        // Iterate until channel is closed and empty
        while (true) {
            if (channel->empty()) {
                if (channel->is_closed()) {
                    break;  // Channel closed and empty, stop iteration
                }
                // Channel empty but not closed, could wait or break
                // For now, break (non-blocking iteration)
                break;
            }
            
            // Receive value from channel
            Value value = channel->receive();
            environment->define(stmt.variable, value);
            
            // Execute loop body
            for (auto& s : stmt.body) {
                s->accept(*this);
            }
        }
        return;
    }
    
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
    
    // Check for decorators
    bool is_dataclass = false;
    bool is_singleton = false;
    
    for (const auto& decorator : stmt.decorators) {
        Value dec_val = environment->get(decorator.name);
        if (std::holds_alternative<std::string>(dec_val)) {
            std::string builtin_name = std::get<std::string>(dec_val);
            if (builtin_name == "__builtin_dataclass__") {
                is_dataclass = true;
            } else if (builtin_name == "__builtin_singleton__") {
                is_singleton = true;
            }
        }
    }
    
    // For @singleton, print note
    if (is_singleton) {
        std::cout << "Note: @singleton on '" << stmt.name << "' - requires advanced memory management (deferred)" << std::endl;
    }
    
    // Map methods by name
    std::map<std::string, std::shared_ptr<Function>> methods;
    std::set<std::string> static_methods;
    std::set<std::string> class_methods;
    
    for (auto& method : stmt.methods) {
        // Check for method decorators
        bool is_static = false;
        bool is_classmethod = false;
        bool is_property = false;
        
        for (const auto& decorator : method->decorators) {
            Value dec_val = environment->get(decorator.name);
            if (std::holds_alternative<std::string>(dec_val)) {
                std::string builtin_name = std::get<std::string>(dec_val);
                if (builtin_name == "__builtin_staticmethod__") {
                    is_static = true;
                } else if (builtin_name == "__builtin_classmethod__") {
                    is_classmethod = true;
                } else if (builtin_name == "__builtin_property__") {
                    is_property = true;
                }
            }
        }
        
        auto func = std::make_shared<Function>(
            method->name,
            method->parameters,
            method->return_type,
            &method->body,
            environment  // methods share the current environment as closure
        );
        methods[method->name] = func;
        
        if (is_static) {
            static_methods.insert(method->name);
        }
        if (is_classmethod) {
            class_methods.insert(method->name);
        }
        if (is_property) {
            // Properties are just regular methods for now
            // In a full implementation, they would be callable without ()
            std::cout << "Note: @property on '" << method->name << "' - call with () for now" << std::endl;
        }
    }
    
    auto cls = std::make_shared<Class>(stmt.name, superclass, methods, false, is_dataclass);
    cls->static_methods = static_methods;
    cls->class_methods = class_methods;
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

void Interpreter::visitTraitDecl(TraitDecl& stmt) {
    // For now, traits are just registered but not stored in the environment
    // In a full implementation, we'd register them in a trait registry
    // and use them for type checking and method dispatch
    
    // Placeholder: just acknowledge the trait exists
    std::cout << "Trait '" << stmt.name << "' declared" << std::endl;
}

void Interpreter::visitImplBlock(ImplBlock& stmt) {
    // Store trait implementation methods
    // Key: (type_name, trait_name) -> map of method_name -> Function
    
    if (stmt.for_type_name.empty()) {
        // Impl block inside a class - we'll handle this later
        std::cout << "Impl block for trait '" << stmt.trait_name << "' (inside class) registered" << std::endl;
        return;
    }
    
    // Standalone impl block: impl Trait for Type
    std::cout << "Impl block for trait '" << stmt.trait_name << "' for type '" << stmt.for_type_name << "' registered" << std::endl;
    
    // Store each method
    auto key = std::make_pair(stmt.for_type_name, stmt.trait_name);
    
    for (auto& method : stmt.methods) {
        auto func = std::make_shared<Function>(
            method->name,
            method->parameters,
            method->return_type,
            &method->body,
            environment
        );
        
        trait_impls[key][method->name] = func;
    }
}

// Channel send statement: ch <- value
void Interpreter::visitChannelSendStmt(ChannelSendStmt& stmt) {
    Value channel_val = evaluateExpr(*stmt.channel);
    
    if (!std::holds_alternative<std::shared_ptr<ChannelValue>>(channel_val)) {
        throw RuntimeError("Can only send to a channel");
    }
    
    auto channel = std::get<std::shared_ptr<ChannelValue>>(channel_val);
    Value value = evaluateExpr(*stmt.value);
    
    try {
        channel->send(value);
    } catch (const std::runtime_error& e) {
        throw RuntimeError(e.what());
    }
}

// Select statement implementation
void Interpreter::visitSelectStmt(SelectStmt& stmt) {
    // Simple implementation: try each case in order
    // In a full implementation, this would use non-blocking operations
    // and wait for the first available channel
    
    // Try each case
    for (auto& select_case : stmt.cases) {
        Value channel_val = evaluateExpr(*select_case.channel);
        
        if (!std::holds_alternative<std::shared_ptr<ChannelValue>>(channel_val)) {
            throw RuntimeError("Select case must use a channel");
        }
        
        auto channel = std::get<std::shared_ptr<ChannelValue>>(channel_val);
        
        // Try to receive (non-blocking)
        if (!channel->empty()) {
            Value received = channel->receive();
            
            // Bind variable if specified
            if (!select_case.variable.empty()) {
                environment->define(select_case.variable, received);
            }
            
            // Execute case body
            for (auto& stmt : select_case.body) {
                stmt->accept(*this);
            }
            
            return;  // Exit after first successful case
        }
    }
    
    // If no case succeeded, execute default case if present
    if (!stmt.default_case.empty()) {
        for (auto& stmt : stmt.default_case) {
            stmt->accept(*this);
        }
    }
}

// Go statement implementation: spawn a goroutine
void Interpreter::visitGoStmt(GoStmt& stmt) {
    // For now, execute immediately (synchronous)
    // In a full implementation, this would spawn a real thread or add to a thread pool
    
    // Execute the function call immediately
    stmt.call->accept(*this);
    
    // Alternative: Add to task queue for later execution
    // This would require a more sophisticated scheduler
}

// Execute all pending tasks
void Interpreter::executePendingTasks() {
    while (!pending_tasks.empty()) {
        auto task = std::move(pending_tasks.front());
        pending_tasks.erase(pending_tasks.begin());
        task();
    }
}

// Pattern matching implementation

void Interpreter::visitMatchExpr(MatchExpr& expr) {
    // Evaluate the scrutinee (the value being matched)
    Value scrutinee = evaluateExpr(*expr.scrutinee);
    
    // Try each arm in order
    for (auto& arm : expr.arms) {
        // Try each pattern in the arm (patterns are OR'd together)
        bool pattern_matched = false;
        std::map<std::string, Value> bindings;
        
        for (auto& pat : arm.patterns) {
            std::map<std::string, Value> temp_bindings;
            if (matchPattern(*pat, scrutinee, temp_bindings)) {
                pattern_matched = true;
                bindings = std::move(temp_bindings);
                break;  // Found a matching pattern, no need to try others
            }
        }
        
        if (!pattern_matched) {
            continue;  // None of the patterns matched, try next arm
        }
        
        // Check guard if present
        if (arm.guard) {
            // Create temporary environment with pattern bindings
            auto guard_env = std::make_shared<Environment>(environment);
            for (const auto& [name, value] : bindings) {
                guard_env->define(name, value);
            }
            
            // Evaluate guard in temporary environment
            auto saved_env = environment;
            environment = guard_env;
            Value guard_result = evaluateExpr(*arm.guard);
            environment = saved_env;
            
            // If guard fails, try next arm
            if (!isTruthy(guard_result)) {
                continue;
            }
        }
        
        // Pattern matched (and guard passed if present)
        // Create environment with bindings and evaluate body
        auto match_env = std::make_shared<Environment>(environment);
        for (const auto& [name, value] : bindings) {
            match_env->define(name, value);
        }
        
        auto saved_env = environment;
        environment = match_env;
        last_value = evaluateExpr(*arm.body);
        environment = saved_env;
        
        return;
    }
    
    // No pattern matched
    throw std::runtime_error("Non-exhaustive pattern match");
}

bool Interpreter::matchPattern(Pattern& pattern, const Value& value, 
                                std::map<std::string, Value>& bindings) {
    switch (pattern.type) {
        case Pattern::Type::WILDCARD:
            // Wildcard always matches, no bindings
            return true;
        
        case Pattern::Type::VARIABLE: {
            // Variable pattern always matches and binds the value
            auto& var_pattern = static_cast<VariablePattern&>(pattern);
            bindings[var_pattern.name] = value;
            return true;
        }
        
        case Pattern::Type::LITERAL: {
            // Literal pattern matches if values are equal
            auto& lit_pattern = static_cast<LiteralPattern&>(pattern);
            Value pattern_value = evaluateExpr(*lit_pattern.value);
            return valuesEqual(value, pattern_value);
        }
        
        case Pattern::Type::ARRAY: {
            // Array pattern matches arrays
            auto& arr_pattern = static_cast<ArrayPattern&>(pattern);
            
            if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(value)) {
                return false;
            }
            
            auto arr = std::get<std::shared_ptr<ArrayValue>>(value);
            
            if (arr_pattern.has_rest) {
                // Pattern like [first, ...rest]
                if (arr->elements.size() < arr_pattern.elements.size()) {
                    return false;
                }
                
                // Match fixed elements
                for (size_t i = 0; i < arr_pattern.elements.size(); i++) {
                    if (!matchPattern(*arr_pattern.elements[i], arr->elements[i], bindings)) {
                        return false;
                    }
                }
                
                // Bind rest elements
                auto rest_arr = std::make_shared<ArrayValue>();
                for (size_t i = arr_pattern.elements.size(); i < arr->elements.size(); i++) {
                    rest_arr->elements.push_back(arr->elements[i]);
                }
                bindings[arr_pattern.rest_name] = rest_arr;
                
                return true;
            } else {
                // Exact length match required
                if (arr->elements.size() != arr_pattern.elements.size()) {
                    return false;
                }
                
                // Match each element
                for (size_t i = 0; i < arr_pattern.elements.size(); i++) {
                    if (!matchPattern(*arr_pattern.elements[i], arr->elements[i], bindings)) {
                        return false;
                    }
                }
                
                return true;
            }
        }
        
        case Pattern::Type::OBJECT: {
            // Object pattern matches instances
            auto& obj_pattern = static_cast<ObjectPattern&>(pattern);
            
            if (!std::holds_alternative<std::shared_ptr<Instance>>(value)) {
                return false;
            }
            
            auto instance = std::get<std::shared_ptr<Instance>>(value);
            
            // Match each field
            for (const auto& [field_name, field_pattern] : obj_pattern.fields) {
                auto it = instance->fields.find(field_name);
                if (it == instance->fields.end()) {
                    return false;  // Field doesn't exist
                }
                
                if (!matchPattern(*field_pattern, it->second, bindings)) {
                    return false;
                }
            }
            
            return true;
        }
        
        case Pattern::Type::CONSTRUCTOR: {
            // Constructor pattern matches Option/Result types
            auto& cons_pattern = static_cast<ConstructorPattern&>(pattern);
            
            if (cons_pattern.constructor_name == "Some") {
                // Some(x) pattern
                if (!std::holds_alternative<std::shared_ptr<OptionValue>>(value)) {
                    return false;
                }
                
                auto option = std::get<std::shared_ptr<OptionValue>>(value);
                if (!option->isSome()) {
                    return false;  // This is None, not Some
                }
                
                // Match inner pattern with the wrapped value
                if (cons_pattern.inner_pattern) {
                    return matchPattern(*cons_pattern.inner_pattern, option->value, bindings);
                }
                
                return true;
            } else if (cons_pattern.constructor_name == "None") {
                // None pattern
                if (!std::holds_alternative<std::shared_ptr<OptionValue>>(value)) {
                    return false;
                }
                
                auto option = std::get<std::shared_ptr<OptionValue>>(value);
                return option->isNone();
            } else if (cons_pattern.constructor_name == "Ok") {
                // Ok(x) pattern
                if (!std::holds_alternative<std::shared_ptr<ResultValue>>(value)) {
                    return false;
                }
                
                auto result = std::get<std::shared_ptr<ResultValue>>(value);
                if (!result->isOk()) {
                    return false;  // This is Err, not Ok
                }
                
                // Match inner pattern with the wrapped value
                if (cons_pattern.inner_pattern) {
                    return matchPattern(*cons_pattern.inner_pattern, result->value, bindings);
                }
                
                return true;
            } else if (cons_pattern.constructor_name == "Err") {
                // Err(e) pattern
                if (!std::holds_alternative<std::shared_ptr<ResultValue>>(value)) {
                    return false;
                }
                
                auto result = std::get<std::shared_ptr<ResultValue>>(value);
                if (!result->isErr()) {
                    return false;  // This is Ok, not Err
                }
                
                // Match inner pattern with the error value
                if (cons_pattern.inner_pattern) {
                    return matchPattern(*cons_pattern.inner_pattern, result->value, bindings);
                }
                
                return true;
            }
            
            return false;
        }
        
        default:
            return false;
    }
}

bool Interpreter::valuesEqual(const Value& a, const Value& b) {
    // Check if both values are the same type and equal
    if (a.index() != b.index()) {
        return false;
    }
    
    if (std::holds_alternative<int>(a)) {
        return std::get<int>(a) == std::get<int>(b);
    }
    if (std::holds_alternative<double>(a)) {
        return std::get<double>(a) == std::get<double>(b);
    }
    if (std::holds_alternative<std::string>(a)) {
        return std::get<std::string>(a) == std::get<std::string>(b);
    }
    if (std::holds_alternative<bool>(a)) {
        return std::get<bool>(a) == std::get<bool>(b);
    }
    if (std::holds_alternative<std::nullptr_t>(a)) {
        return true;  // Both are null
    }
    
    // For complex types, use pointer equality
    return false;
}

// Await expression implementation
void Interpreter::visitAwaitExpr(AwaitExpr& expr) {
    // For now, just evaluate the future expression directly
    // In a full implementation, this would:
    // 1. Check if the expression returns a Future
    // 2. Suspend the current task
    // 3. Resume when the future completes
    // 4. Return the future's value
    
    // Simplified implementation: just evaluate the expression
    last_value = evaluateExpr(*expr.future);
    
    // TODO: Implement proper async/await with:
    // - Future type checking
    // - Task suspension/resumption
    // - Event loop integration
}

// Channel expression implementation: chan<T>(capacity)
void Interpreter::visitChannelExpr(ChannelExpr& expr) {
    // Evaluate capacity if provided
    size_t capacity = 0;
    if (expr.capacity) {
        Value cap_val = evaluateExpr(*expr.capacity);
        if (std::holds_alternative<int>(cap_val)) {
            capacity = std::get<int>(cap_val);
        } else {
            throw RuntimeError("Channel capacity must be an integer");
        }
    }
    
    // Create a new channel
    auto channel = std::make_shared<ChannelValue>(capacity);
    last_value = channel;
}

// Channel receive expression: <-ch
void Interpreter::visitChannelReceiveExpr(ChannelReceiveExpr& expr) {
    Value channel_val = evaluateExpr(*expr.channel);
    
    if (!std::holds_alternative<std::shared_ptr<ChannelValue>>(channel_val)) {
        throw RuntimeError("Can only receive from a channel");
    }
    
    auto channel = std::get<std::shared_ptr<ChannelValue>>(channel_val);
    
    try {
        last_value = channel->receive();
    } catch (const std::runtime_error& e) {
        throw RuntimeError(e.what());
    }
}

// Try expression: expr? for error propagation
void Interpreter::visitTryExpr(TryExpr& expr) {
    Value result_val = evaluateExpr(*expr.operand);
    
    // The ? operator only works with Result types
    if (!std::holds_alternative<std::shared_ptr<ResultValue>>(result_val)) {
        throw RuntimeError("? operator can only be used with Result types");
    }
    
    auto result = std::get<std::shared_ptr<ResultValue>>(result_val);
    
    if (result->isOk()) {
        // Unwrap the Ok value and continue
        last_value = result->value;
    } else {
        // Propagate the error by returning early with Err
        throw ReturnException(result_val);
    }
}

// Stringify expression: stringify(expr) for macros
void Interpreter::visitStringifyExpr(StringifyExpr& expr) {
    // Convert the expression to a string representation
    // For now, we'll use a simple approach - just return the expression as a string
    // In a full implementation, this would reconstruct the source code from the AST
    
    // Try to evaluate the expression and convert to string
    try {
        Value val = evaluateExpr(*expr.operand);
        last_value = valueToString(val);
    } catch (...) {
        // If evaluation fails, return a placeholder
        last_value = std::string("<expression>");
    }
}

// Macro declaration
void Interpreter::visitMacroDecl(MacroDecl& stmt) {
    // For now, macros are stored but not expanded
    // A full implementation would require AST transformation and substitution
    // This is a placeholder that allows macro declarations to be parsed
    
    // Store macro name in environment as a marker
    environment->define(stmt.name, std::string("<macro>"));
}

// Trait method lookup

std::shared_ptr<Function> Interpreter::findTraitMethod(const std::string& type_name, const std::string& method_name) {
    // Search through all trait implementations for this type
    for (const auto& [key, methods] : trait_impls) {
        const auto& [impl_type, trait_name] = key;
        
        if (impl_type == type_name) {
            auto it = methods.find(method_name);
            if (it != methods.end()) {
                return it->second;
            }
        }
    }
    
    return nullptr;
}

} // namespace sapphire
