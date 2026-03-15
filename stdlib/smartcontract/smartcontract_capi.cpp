#include "smartcontract.h"
#include <chrono>
#include <string>

using namespace Sapphire::SmartContract;

extern "C" {

static std::string g_sc_last_string;

// ===== VM =====

void* sc_vm_create() {
    return new VM();
}

void sc_vm_destroy(void* vm) {
    delete static_cast<VM*>(vm);
}

void sc_vm_set_storage(void* vm, const char* contract, const char* key, const char* value) {
    static_cast<VM*>(vm)->set_storage(
        contract ? contract : "",
        key      ? key      : "",
        std::string(value ? value : ""));
}

const char* sc_vm_get_storage(void* vm, const char* contract, const char* key) {
    StackValue v = static_cast<VM*>(vm)->get_storage(
        contract ? contract : "", key ? key : "");
    g_sc_last_string = stack_value_to_string(v);
    return g_sc_last_string.c_str();
}

void sc_vm_set_balance(void* vm, const char* address, int64_t balance) {
    static_cast<VM*>(vm)->set_balance(address ? address : "", balance);
}

int64_t sc_vm_get_balance(void* vm, const char* address) {
    return static_cast<VM*>(vm)->get_balance(address ? address : "");
}

const char* sc_vm_state_dump(void* vm) {
    g_sc_last_string = static_cast<VM*>(vm)->state_dump();
    return g_sc_last_string.c_str();
}

// ===== Bytecode =====

void* sc_bytecode_create() {
    return new Bytecode();
}

void sc_bytecode_destroy(void* bc) {
    delete static_cast<Bytecode*>(bc);
}

void sc_bytecode_push_int(void* bc, int64_t val) {
    static_cast<Bytecode*>(bc)->emit(Opcode::PUSH, val);
}

void sc_bytecode_push_str(void* bc, const char* val) {
    static_cast<Bytecode*>(bc)->emit(Opcode::PUSH, std::string(val ? val : ""));
}

void sc_bytecode_push_bool(void* bc, int val) {
    static_cast<Bytecode*>(bc)->emit(Opcode::PUSH, val != 0);
}

void sc_bytecode_push_double(void* bc, double val) {
    static_cast<Bytecode*>(bc)->emit(Opcode::PUSH, val);
}

void sc_bytecode_emit(void* bc, int opcode) {
    static_cast<Bytecode*>(bc)->emit(static_cast<Opcode>(opcode));
}

void sc_bytecode_label(void* bc, const char* name) {
    static_cast<Bytecode*>(bc)->label(name ? name : "");
}

void sc_bytecode_jump(void* bc, const char* label) {
    static_cast<Bytecode*>(bc)->emit(Opcode::JUMP, std::string(label ? label : ""));
}

void sc_bytecode_jumpi(void* bc, const char* label) {
    static_cast<Bytecode*>(bc)->emit(Opcode::JUMPI, std::string(label ? label : ""));
}

int sc_bytecode_size(void* bc) {
    return static_cast<int>(static_cast<Bytecode*>(bc)->size());
}

// ===== Execution Context =====

void* sc_ctx_create(const char* caller, const char* contract,
                    int64_t value, uint64_t gas_limit) {
    auto* ctx = new ExecutionContext();
    ctx->caller           = caller   ? caller   : "";
    ctx->origin           = caller   ? caller   : "";
    ctx->contract_address = contract ? contract : "";
    ctx->value            = value;
    ctx->gas_limit        = gas_limit;
    ctx->gas_used         = 0;
    ctx->block_number     = 1;
    ctx->timestamp        = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    return ctx;
}

void sc_ctx_destroy(void* ctx) {
    delete static_cast<ExecutionContext*>(ctx);
}

void sc_ctx_set_block(void* ctx, uint64_t block_number, uint64_t timestamp) {
    auto* c = static_cast<ExecutionContext*>(ctx);
    c->block_number = block_number;
    c->timestamp    = timestamp;
}

// ===== Execute =====

void* sc_execute(void* vm, void* bytecode, void* ctx) {
    auto* v  = static_cast<VM*>(vm);
    auto* bc = static_cast<Bytecode*>(bytecode);
    auto* c  = static_cast<ExecutionContext*>(ctx);
    v->clear_events();
    auto* result = new ExecResult(v->execute(*bc, *c));
    return result;
}

void sc_result_destroy(void* result) {
    delete static_cast<ExecResult*>(result);
}

int sc_result_success(void* result) {
    return static_cast<ExecResult*>(result)->success ? 1 : 0;
}

const char* sc_result_return_value(void* result) {
    g_sc_last_string = static_cast<ExecResult*>(result)->return_value;
    return g_sc_last_string.c_str();
}

const char* sc_result_revert_reason(void* result) {
    g_sc_last_string = static_cast<ExecResult*>(result)->revert_reason;
    return g_sc_last_string.c_str();
}

int64_t sc_result_gas_used(void* result) {
    return static_cast<int64_t>(static_cast<ExecResult*>(result)->gas_used);
}

int sc_result_event_count(void* result) {
    return static_cast<int>(static_cast<ExecResult*>(result)->events.size());
}

const char* sc_result_event_name(void* result, int index) {
    auto& events = static_cast<ExecResult*>(result)->events;
    if (index < 0 || index >= static_cast<int>(events.size())) {
        g_sc_last_string = "";
    } else {
        g_sc_last_string = events[static_cast<size_t>(index)].name;
    }
    return g_sc_last_string.c_str();
}

const char* sc_result_event_data(void* result, int index) {
    auto& events = static_cast<ExecResult*>(result)->events;
    if (index < 0 || index >= static_cast<int>(events.size())) {
        g_sc_last_string = "";
    } else {
        g_sc_last_string = events[static_cast<size_t>(index)].data;
    }
    return g_sc_last_string.c_str();
}

// ===== Gas Table =====

void* sc_gas_table_create() {
    return new GasTable();
}

void sc_gas_table_destroy(void* gt) {
    delete static_cast<GasTable*>(gt);
}

void sc_gas_table_set_sstore(void* gt, uint64_t cost) {
    static_cast<GasTable*>(gt)->sstore = cost;
}

void sc_gas_table_set_sload(void* gt, uint64_t cost) {
    static_cast<GasTable*>(gt)->sload = cost;
}

} // extern "C"
