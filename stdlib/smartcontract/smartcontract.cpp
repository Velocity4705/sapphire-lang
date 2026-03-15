#include "smartcontract.h"

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <chrono>
#include <cstring>

namespace Sapphire {
namespace SmartContract {

// ===== StackValue helpers =====

std::string stack_value_to_string(const StackValue& v) {
    return std::visit([](auto&& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int64_t>)
            return std::to_string(val);
        else if constexpr (std::is_same_v<T, double>) {
            std::ostringstream ss;
            ss << val;
            return ss.str();
        } else if constexpr (std::is_same_v<T, std::string>)
            return val;
        else if constexpr (std::is_same_v<T, bool>)
            return val ? "true" : "false";
        return "";
    }, v);
}

StackValue stack_value_add(const StackValue& a, const StackValue& b) {
    if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b))
        return std::get<int64_t>(a) + std::get<int64_t>(b);
    if (std::holds_alternative<double>(a) || std::holds_alternative<double>(b)) {
        double da = std::holds_alternative<double>(a) ? std::get<double>(a)
                  : static_cast<double>(std::get<int64_t>(a));
        double db = std::holds_alternative<double>(b) ? std::get<double>(b)
                  : static_cast<double>(std::get<int64_t>(b));
        return da + db;
    }
    if (std::holds_alternative<std::string>(a) || std::holds_alternative<std::string>(b))
        return stack_value_to_string(a) + stack_value_to_string(b);
    return int64_t(0);
}

static int64_t to_int(const StackValue& v) {
    if (std::holds_alternative<int64_t>(v)) return std::get<int64_t>(v);
    if (std::holds_alternative<double>(v))  return static_cast<int64_t>(std::get<double>(v));
    if (std::holds_alternative<bool>(v))    return std::get<bool>(v) ? 1 : 0;
    return 0;
}

static bool to_bool(const StackValue& v) {
    if (std::holds_alternative<bool>(v))    return std::get<bool>(v);
    if (std::holds_alternative<int64_t>(v)) return std::get<int64_t>(v) != 0;
    if (std::holds_alternative<double>(v))  return std::get<double>(v) != 0.0;
    if (std::holds_alternative<std::string>(v)) return !std::get<std::string>(v).empty();
    return false;
}

// ===== Bytecode =====

void Bytecode::emit(Opcode op) {
    instructions.push_back({op, int64_t(0), ""});
}

void Bytecode::emit(Opcode op, int64_t val) {
    instructions.push_back({op, val, ""});
}

void Bytecode::emit(Opcode op, double val) {
    instructions.push_back({op, val, ""});
}

void Bytecode::emit(Opcode op, const std::string& val) {
    instructions.push_back({op, val, ""});
}

void Bytecode::emit(Opcode op, bool val) {
    instructions.push_back({op, val, ""});
}

void Bytecode::label(const std::string& name) {
    labels[name] = instructions.size();
}

// ===== VM =====

VM::VM(const GasTable& gas) : gas_table_(gas) {}

void VM::consume_gas(uint64_t& gas_used, uint64_t limit, uint64_t amount) {
    gas_used += amount;
    if (gas_used > limit)
        throw std::runtime_error("out of gas");
}

void VM::set_storage(const std::string& contract, const std::string& key,
                     const StackValue& value) {
    storage_[contract][key] = value;
}

StackValue VM::get_storage(const std::string& contract, const std::string& key) const {
    auto ci = storage_.find(contract);
    if (ci == storage_.end()) return int64_t(0);
    auto ki = ci->second.find(key);
    if (ki == ci->second.end()) return int64_t(0);
    return ki->second;
}

void VM::set_balance(const std::string& address, int64_t balance) {
    balances_[address] = balance;
}

int64_t VM::get_balance(const std::string& address) const {
    auto it = balances_.find(address);
    return it != balances_.end() ? it->second : 0;
}

void VM::register_host_fn(const std::string& name, HostFn fn) {
    host_fns_[name] = std::move(fn);
}

std::string VM::state_dump() const {
    std::ostringstream ss;
    ss << "{\"storage\":{";
    bool first_c = true;
    for (auto& [contract, kv] : storage_) {
        if (!first_c) ss << ",";
        first_c = false;
        ss << "\"" << contract << "\":{";
        bool first_k = true;
        for (auto& [k, v] : kv) {
            if (!first_k) ss << ",";
            first_k = false;
            ss << "\"" << k << "\":\"" << stack_value_to_string(v) << "\"";
        }
        ss << "}";
    }
    ss << "},\"balances\":{";
    bool first_b = true;
    for (auto& [addr, bal] : balances_) {
        if (!first_b) ss << ",";
        first_b = false;
        ss << "\"" << addr << "\":" << bal;
    }
    ss << "}}";
    return ss.str();
}

ExecResult VM::execute(const Bytecode& code, const ExecutionContext& ctx) {
    ExecResult result;
    result.success  = false;
    result.gas_used = 0;

    std::vector<StackValue> stack;
    std::unordered_map<std::string, StackValue> memory;
    size_t pc = 0;

    auto pop = [&]() -> StackValue {
        if (stack.empty()) throw std::runtime_error("stack underflow");
        StackValue v = stack.back();
        stack.pop_back();
        return v;
    };

    auto push = [&](StackValue v) {
        stack.push_back(std::move(v));
    };

    try {
        while (pc < code.instructions.size()) {
            const Instruction& instr = code.instructions[pc];
            consume_gas(result.gas_used, ctx.gas_limit, gas_table_.base);
            pc++;

            switch (instr.opcode) {
            case Opcode::PUSH:
                push(instr.operand);
                break;

            case Opcode::POP:
                pop();
                break;

            case Opcode::DUP: {
                if (stack.empty()) throw std::runtime_error("DUP: stack empty");
                push(stack.back());
                break;
            }

            case Opcode::SWAP: {
                if (stack.size() < 2) throw std::runtime_error("SWAP: need 2 items");
                std::swap(stack[stack.size()-1], stack[stack.size()-2]);
                break;
            }

            case Opcode::ADD: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.arithmetic);
                auto b = pop(); auto a = pop();
                push(stack_value_add(a, b));
                break;
            }

            case Opcode::SUB: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.arithmetic);
                auto b = pop(); auto a = pop();
                push(int64_t(to_int(a) - to_int(b)));
                break;
            }

            case Opcode::MUL: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.arithmetic);
                auto b = pop(); auto a = pop();
                push(int64_t(to_int(a) * to_int(b)));
                break;
            }

            case Opcode::DIV: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.arithmetic);
                auto b = pop(); auto a = pop();
                int64_t dv = to_int(b);
                if (dv == 0) throw std::runtime_error("division by zero");
                push(int64_t(to_int(a) / dv));
                break;
            }

            case Opcode::MOD: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.arithmetic);
                auto b = pop(); auto a = pop();
                int64_t dv = to_int(b);
                if (dv == 0) throw std::runtime_error("modulo by zero");
                push(int64_t(to_int(a) % dv));
                break;
            }

            case Opcode::EQ: {
                auto b = pop(); auto a = pop();
                push(bool(stack_value_to_string(a) == stack_value_to_string(b)));
                break;
            }

            case Opcode::NEQ: {
                auto b = pop(); auto a = pop();
                push(bool(stack_value_to_string(a) != stack_value_to_string(b)));
                break;
            }

            case Opcode::LT: {
                auto b = pop(); auto a = pop();
                push(bool(to_int(a) < to_int(b)));
                break;
            }

            case Opcode::GT: {
                auto b = pop(); auto a = pop();
                push(bool(to_int(a) > to_int(b)));
                break;
            }

            case Opcode::LTE: {
                auto b = pop(); auto a = pop();
                push(bool(to_int(a) <= to_int(b)));
                break;
            }

            case Opcode::GTE: {
                auto b = pop(); auto a = pop();
                push(bool(to_int(a) >= to_int(b)));
                break;
            }

            case Opcode::AND: {
                auto b = pop(); auto a = pop();
                push(bool(to_bool(a) && to_bool(b)));
                break;
            }

            case Opcode::OR: {
                auto b = pop(); auto a = pop();
                push(bool(to_bool(a) || to_bool(b)));
                break;
            }

            case Opcode::NOT: {
                auto a = pop();
                push(bool(!to_bool(a)));
                break;
            }

            case Opcode::JUMP: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.jump);
                // operand is label name or direct index
                if (std::holds_alternative<std::string>(instr.operand)) {
                    auto& lbl = std::get<std::string>(instr.operand);
                    auto it = code.labels.find(lbl);
                    if (it == code.labels.end())
                        throw std::runtime_error("JUMP: unknown label: " + lbl);
                    pc = it->second;
                } else {
                    pc = static_cast<size_t>(to_int(instr.operand));
                }
                break;
            }

            case Opcode::JUMPI: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.jump);
                auto cond = pop();
                if (to_bool(cond)) {
                    if (std::holds_alternative<std::string>(instr.operand)) {
                        auto& lbl = std::get<std::string>(instr.operand);
                        auto it = code.labels.find(lbl);
                        if (it == code.labels.end())
                            throw std::runtime_error("JUMPI: unknown label: " + lbl);
                        pc = it->second;
                    } else {
                        pc = static_cast<size_t>(to_int(instr.operand));
                    }
                }
                break;
            }

            case Opcode::CALL: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.call);
                std::string fn_name = stack_value_to_string(pop());
                int64_t argc = to_int(pop());
                std::vector<StackValue> args;
                for (int64_t i = 0; i < argc; i++) args.push_back(pop());
                auto it = host_fns_.find(fn_name);
                if (it == host_fns_.end())
                    throw std::runtime_error("CALL: unknown host function: " + fn_name);
                push(it->second(args));
                break;
            }

            case Opcode::RETURN: {
                if (!stack.empty())
                    result.return_value = stack_value_to_string(stack.back());
                result.success = true;
                result.events  = events_;
                return result;
            }

            case Opcode::REVERT: {
                result.revert_reason = stack.empty() ? "reverted"
                                     : stack_value_to_string(stack.back());
                result.success = false;
                result.events  = events_;
                return result;
            }

            case Opcode::HALT: {
                result.success = true;
                result.events  = events_;
                return result;
            }

            case Opcode::SLOAD: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.sload);
                std::string key = stack_value_to_string(pop());
                push(get_storage(ctx.contract_address, key));
                break;
            }

            case Opcode::SSTORE: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.sstore);
                auto value = pop();
                std::string key = stack_value_to_string(pop());
                set_storage(ctx.contract_address, key, value);
                break;
            }

            case Opcode::MLOAD: {
                std::string key = stack_value_to_string(pop());
                auto it = memory.find(key);
                push(it != memory.end() ? it->second : StackValue{int64_t(0)});
                break;
            }

            case Opcode::MSTORE: {
                auto value = pop();
                std::string key = stack_value_to_string(pop());
                memory[key] = value;
                break;
            }

            case Opcode::CALLER:
                push(ctx.caller);
                break;

            case Opcode::ORIGIN:
                push(ctx.origin);
                break;

            case Opcode::VALUE:
                push(ctx.value);
                break;

            case Opcode::BALANCE: {
                std::string addr = stack_value_to_string(pop());
                push(get_balance(addr));
                break;
            }

            case Opcode::BLOCKNUMBER:
                push(static_cast<int64_t>(ctx.block_number));
                break;

            case Opcode::TIMESTAMP:
                push(static_cast<int64_t>(ctx.timestamp));
                break;

            case Opcode::LOG0:
            case Opcode::LOG1:
            case Opcode::LOG2: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.log);
                ContractEvent ev;
                ev.contract_address = ctx.contract_address;
                ev.block_number     = ctx.block_number;
                ev.name = stack_value_to_string(pop());
                ev.data = stack.empty() ? "" : stack_value_to_string(pop());
                if (instr.opcode == Opcode::LOG1 || instr.opcode == Opcode::LOG2) {
                    if (!stack.empty()) ev.topics.push_back(stack_value_to_string(pop()));
                }
                if (instr.opcode == Opcode::LOG2) {
                    if (!stack.empty()) ev.topics.push_back(stack_value_to_string(pop()));
                }
                events_.push_back(ev);
                break;
            }

            case Opcode::SHA256OP: {
                consume_gas(result.gas_used, ctx.gas_limit, gas_table_.sha256op);
                std::string data = stack_value_to_string(pop());
                uint8_t hash[32];
                SHA256(reinterpret_cast<const uint8_t*>(data.data()), data.size(), hash);
                std::ostringstream ss;
                ss << std::hex << std::setfill('0');
                for (int i = 0; i < 32; i++) ss << std::setw(2) << (int)hash[i];
                push(ss.str());
                break;
            }

            case Opcode::CONCAT: {
                auto b = pop(); auto a = pop();
                push(stack_value_to_string(a) + stack_value_to_string(b));
                break;
            }

            case Opcode::STRLEN: {
                auto s = pop();
                push(int64_t(stack_value_to_string(s).size()));
                break;
            }

            default:
                throw std::runtime_error("unknown opcode");
            }
        }

        // Fell off end — treat as success
        if (!stack.empty())
            result.return_value = stack_value_to_string(stack.back());
        result.success = true;

    } catch (const std::exception& e) {
        result.success       = false;
        result.revert_reason = e.what();
    }

    result.events = events_;
    return result;
}

// ===== ContractBuilder =====

ContractBuilder::ContractBuilder(const std::string& name, const std::string& address)
    : name_(name), address_(address) {}

ContractBuilder& ContractBuilder::function(const std::string& name, const Bytecode& body) {
    functions_[name] = body;
    return *this;
}

Bytecode ContractBuilder::compile() const {
    // Simple concatenation with labels for each function
    Bytecode result;
    for (auto& [fname, body] : functions_) {
        result.labels[fname] = result.instructions.size();
        for (auto& instr : body.instructions)
            result.instructions.push_back(instr);
        // Ensure each function ends with RETURN if not already
        if (result.instructions.empty() ||
            (result.instructions.back().opcode != Opcode::RETURN &&
             result.instructions.back().opcode != Opcode::HALT))
            result.emit(Opcode::RETURN);
    }
    return result;
}

} // namespace SmartContract
} // namespace Sapphire
