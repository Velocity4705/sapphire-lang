#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <variant>
#include <stdexcept>

namespace Sapphire {
namespace SmartContract {

// ===== Opcodes =====
enum class Opcode : uint8_t {
    // Stack
    PUSH = 0x01,   // PUSH <value>
    POP  = 0x02,
    DUP  = 0x03,
    SWAP = 0x04,
    // Arithmetic
    ADD  = 0x10,
    SUB  = 0x11,
    MUL  = 0x12,
    DIV  = 0x13,
    MOD  = 0x14,
    // Comparison
    EQ   = 0x20,
    NEQ  = 0x21,
    LT   = 0x22,
    GT   = 0x23,
    LTE  = 0x24,
    GTE  = 0x25,
    // Logic
    AND  = 0x30,
    OR   = 0x31,
    NOT  = 0x32,
    // Control flow
    JUMP    = 0x40,
    JUMPI   = 0x41,  // conditional jump
    CALL    = 0x42,
    RETURN  = 0x43,
    REVERT  = 0x44,
    HALT    = 0x45,
    // State
    SLOAD   = 0x50,  // load from storage
    SSTORE  = 0x51,  // store to storage
    MLOAD   = 0x52,  // load from memory
    MSTORE  = 0x53,  // store to memory
    // Blockchain context
    CALLER  = 0x60,  // push caller address
    ORIGIN  = 0x61,  // push origin address
    VALUE   = 0x62,  // push call value
    BALANCE = 0x63,  // push balance of address
    BLOCKNUMBER = 0x64,
    TIMESTAMP   = 0x65,
    // Events
    LOG0 = 0x70,
    LOG1 = 0x71,
    LOG2 = 0x72,
    // Crypto
    SHA256OP = 0x80,
    // String ops
    CONCAT = 0x90,
    STRLEN = 0x91,
};

// ===== Value type on the VM stack =====
using StackValue = std::variant<int64_t, double, std::string, bool>;

std::string stack_value_to_string(const StackValue& v);
StackValue stack_value_add(const StackValue& a, const StackValue& b);

// ===== Gas costs =====
struct GasTable {
    uint64_t base       = 1;
    uint64_t sload      = 200;
    uint64_t sstore     = 5000;
    uint64_t log        = 375;
    uint64_t sha256op   = 60;
    uint64_t call       = 700;
    uint64_t arithmetic = 3;
    uint64_t jump       = 8;
};

// ===== Event =====
struct ContractEvent {
    std::string name;
    std::vector<std::string> topics;
    std::string data;
    uint64_t block_number;
    std::string contract_address;
};

// ===== Execution context =====
struct ExecutionContext {
    std::string caller;
    std::string origin;
    std::string contract_address;
    int64_t     value;       // tokens sent with call
    uint64_t    block_number;
    uint64_t    timestamp;
    uint64_t    gas_limit;
    uint64_t    gas_used;
};

// ===== Bytecode instruction =====
struct Instruction {
    Opcode opcode;
    StackValue operand;  // used by PUSH, JUMP targets, etc.
    std::string label;   // optional label for JUMP targets
};

// ===== Contract bytecode =====
struct Bytecode {
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, size_t> labels; // label -> instruction index

    void emit(Opcode op);
    void emit(Opcode op, int64_t val);
    void emit(Opcode op, double val);
    void emit(Opcode op, const std::string& val);
    void emit(Opcode op, bool val);
    void label(const std::string& name);
    size_t size() const { return instructions.size(); }
};

// ===== VM Execution Result =====
struct ExecResult {
    bool success;
    std::string return_value;
    std::string revert_reason;
    uint64_t gas_used;
    std::vector<ContractEvent> events;
};

// ===== Smart Contract VM =====
class VM {
public:
    explicit VM(const GasTable& gas = GasTable{});
    ~VM() = default;

    // Execute bytecode
    ExecResult execute(const Bytecode& code, const ExecutionContext& ctx);

    // State access
    void set_storage(const std::string& contract, const std::string& key,
                     const StackValue& value);
    StackValue get_storage(const std::string& contract, const std::string& key) const;
    void set_balance(const std::string& address, int64_t balance);
    int64_t get_balance(const std::string& address) const;

    // Register host function (callable via CALL opcode)
    using HostFn = std::function<StackValue(std::vector<StackValue>)>;
    void register_host_fn(const std::string& name, HostFn fn);

    // Events emitted during last execution
    const std::vector<ContractEvent>& events() const { return events_; }
    void clear_events() { events_.clear(); }

    std::string state_dump() const;

private:
    GasTable gas_table_;
    // contract_address -> key -> value
    std::unordered_map<std::string, std::unordered_map<std::string, StackValue>> storage_;
    std::unordered_map<std::string, int64_t> balances_;
    std::unordered_map<std::string, HostFn> host_fns_;
    std::vector<ContractEvent> events_;

    void consume_gas(uint64_t& gas_used, uint64_t limit, uint64_t amount);
};

// ===== High-level Contract Builder =====
// Lets you define a contract as named functions compiled to Bytecode
class ContractBuilder {
public:
    ContractBuilder(const std::string& name, const std::string& address);

    // Define a function body
    ContractBuilder& function(const std::string& name, const Bytecode& body);

    // Compile to a single Bytecode with a dispatch table
    Bytecode compile() const;

    const std::string& address() const { return address_; }
    const std::string& name() const { return name_; }

private:
    std::string name_;
    std::string address_;
    std::unordered_map<std::string, Bytecode> functions_;
};

} // namespace SmartContract
} // namespace Sapphire

// ===== C API =====
extern "C" {

// VM lifecycle
void* sc_vm_create();
void  sc_vm_destroy(void* vm);

// State management
void sc_vm_set_storage(void* vm, const char* contract, const char* key, const char* value);
const char* sc_vm_get_storage(void* vm, const char* contract, const char* key);
void sc_vm_set_balance(void* vm, const char* address, int64_t balance);
int64_t sc_vm_get_balance(void* vm, const char* address);

// Bytecode builder
void* sc_bytecode_create();
void  sc_bytecode_destroy(void* bc);
void  sc_bytecode_push_int(void* bc, int64_t val);
void  sc_bytecode_push_str(void* bc, const char* val);
void  sc_bytecode_push_bool(void* bc, int val);
void  sc_bytecode_push_double(void* bc, double val);
void  sc_bytecode_emit(void* bc, int opcode);
void  sc_bytecode_label(void* bc, const char* name);
void  sc_bytecode_jump(void* bc, const char* label);   // JUMP to label
void  sc_bytecode_jumpi(void* bc, const char* label);  // JUMPI to label (pops condition)
int   sc_bytecode_size(void* bc);
// Execution context
void* sc_ctx_create(const char* caller, const char* contract,
                    int64_t value, uint64_t gas_limit);
void  sc_ctx_destroy(void* ctx);
void  sc_ctx_set_block(void* ctx, uint64_t block_number, uint64_t timestamp);

// Execute
void* sc_execute(void* vm, void* bytecode, void* ctx);
void  sc_result_destroy(void* result);
int   sc_result_success(void* result);
const char* sc_result_return_value(void* result);
const char* sc_result_revert_reason(void* result);
int64_t sc_result_gas_used(void* result);
int   sc_result_event_count(void* result);
const char* sc_result_event_name(void* result, int index);
const char* sc_result_event_data(void* result, int index);

// Gas table
void* sc_gas_table_create();
void  sc_gas_table_destroy(void* gt);
void  sc_gas_table_set_sstore(void* gt, uint64_t cost);
void  sc_gas_table_set_sload(void* gt, uint64_t cost);

// State dump
const char* sc_vm_state_dump(void* vm);

} // extern "C"
