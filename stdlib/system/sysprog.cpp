#include "sysprog.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <algorithm>

// ============================================================
// Phase 1: Inline Assembly
// ============================================================

struct AsmContext {
    std::string arch;
    std::unordered_map<std::string, int64_t> regs;
    std::vector<std::string> history;

    void init_regs() {
        if (arch == "x86_64") {
            for (auto& r : {"rax","rbx","rcx","rdx","rsi","rdi","rsp","rbp",
                            "r8","r9","r10","r11","r12","r13","r14","r15","rip","rflags"})
                regs[r] = 0;
            regs["rsp"] = 0x7fff0000;
        } else if (arch == "arm64") {
            for (int i=0;i<31;i++) regs["x"+std::to_string(i)] = 0;
            regs["sp"] = 0x7fff0000; regs["pc"] = 0; regs["lr"] = 0;
        } else { // arm32
            for (int i=0;i<16;i++) regs["r"+std::to_string(i)] = 0;
            regs["sp"] = 0x7fff0000; regs["pc"] = 0; regs["lr"] = 0;
        }
    }
};

// Simple x86_64 instruction simulator
static void sim_x86(AsmContext* ctx, const std::string& mn) {
    // Parse: "MOV RAX, 42" / "ADD RAX, RBX" / "PUSH RAX" / "POP RBX" / "NOP" / "RET"
    std::string s = mn;
    for (auto& c : s) c = tolower(c);
    // strip leading spaces
    while (!s.empty() && s[0]==' ') s.erase(0,1);

    auto split = [](const std::string& str, char delim) {
        std::vector<std::string> parts;
        std::string cur;
        for (char c : str) {
            if (c == delim) { if (!cur.empty()) { parts.push_back(cur); cur.clear(); } }
            else cur += c;
        }
        if (!cur.empty()) parts.push_back(cur);
        return parts;
    };
    auto trim = [](std::string t) {
        while (!t.empty() && (t[0]==' '||t[0]=='\t')) t.erase(0,1);
        while (!t.empty() && (t.back()==' '||t.back()=='\t')) t.pop_back();
        return t;
    };

    size_t sp = s.find(' ');
    std::string op = (sp==std::string::npos) ? s : s.substr(0,sp);
    std::string rest = (sp==std::string::npos) ? "" : trim(s.substr(sp+1));
    auto operands = split(rest, ',');
    for (auto& o : operands) o = trim(o);

    auto get_val = [&](const std::string& o) -> int64_t {
        if (ctx->regs.count(o)) return ctx->regs[o];
        try { return std::stoll(o); } catch(...) { return 0; }
    };

    if (op == "nop") { /* nothing */ }
    else if (op == "mov" && operands.size()==2) {
        ctx->regs[operands[0]] = get_val(operands[1]);
    } else if (op == "add" && operands.size()==2) {
        ctx->regs[operands[0]] += get_val(operands[1]);
    } else if (op == "sub" && operands.size()==2) {
        ctx->regs[operands[0]] -= get_val(operands[1]);
    } else if (op == "mul" && operands.size()==2) {
        ctx->regs[operands[0]] *= get_val(operands[1]);
    } else if (op == "imul" && operands.size()==2) {
        ctx->regs[operands[0]] *= get_val(operands[1]);
    } else if (op == "xor" && operands.size()==2) {
        ctx->regs[operands[0]] ^= get_val(operands[1]);
    } else if (op == "and" && operands.size()==2) {
        ctx->regs[operands[0]] &= get_val(operands[1]);
    } else if (op == "or" && operands.size()==2) {
        ctx->regs[operands[0]] |= get_val(operands[1]);
    } else if (op == "shl" && operands.size()==2) {
        ctx->regs[operands[0]] <<= get_val(operands[1]);
    } else if (op == "shr" && operands.size()==2) {
        ctx->regs[operands[0]] >>= get_val(operands[1]);
    } else if (op == "inc" && operands.size()==1) {
        ctx->regs[operands[0]]++;
    } else if (op == "dec" && operands.size()==1) {
        ctx->regs[operands[0]]--;
    } else if (op == "push" && operands.size()==1) {
        ctx->regs["rsp"] -= 8;
    } else if (op == "pop" && operands.size()==1) {
        ctx->regs["rsp"] += 8;
    } else if (op == "ret") {
        ctx->regs["rip"] = ctx->regs["rsp"];
    }
    ctx->regs["rip"] += 4;
}

static void sim_arm64(AsmContext* ctx, const std::string& mn) {
    std::string s = mn;
    for (auto& c : s) c = tolower(c);
    while (!s.empty() && s[0]==' ') s.erase(0,1);
    size_t sp = s.find(' ');
    std::string op = (sp==std::string::npos) ? s : s.substr(0,sp);
    std::string rest = (sp==std::string::npos) ? "" : s.substr(sp+1);

    auto trim = [](std::string t) {
        while (!t.empty() && (t[0]==' '||t[0]=='\t')) t.erase(0,1);
        while (!t.empty() && (t.back()==' '||t.back()=='\t')) t.pop_back();
        return t;
    };
    std::vector<std::string> operands;
    std::string cur;
    for (char c : rest) {
        if (c==',') { operands.push_back(trim(cur)); cur.clear(); }
        else cur += c;
    }
    if (!cur.empty()) operands.push_back(trim(cur));

    auto get_val = [&](const std::string& o) -> int64_t {
        if (ctx->regs.count(o)) return ctx->regs[o];
        if (o.size()>1 && o[0]=='#') { try { return std::stoll(o.substr(1)); } catch(...){} }
        try { return std::stoll(o); } catch(...) { return 0; }
    };

    if (op == "nop") {}
    else if (op == "mov" && operands.size()==2) ctx->regs[operands[0]] = get_val(operands[1]);
    else if (op == "add" && operands.size()==3) ctx->regs[operands[0]] = get_val(operands[1]) + get_val(operands[2]);
    else if (op == "sub" && operands.size()==3) ctx->regs[operands[0]] = get_val(operands[1]) - get_val(operands[2]);
    else if (op == "mul" && operands.size()==3) ctx->regs[operands[0]] = get_val(operands[1]) * get_val(operands[2]);
    else if (op == "eor" && operands.size()==3) ctx->regs[operands[0]] = get_val(operands[1]) ^ get_val(operands[2]);
    else if (op == "and" && operands.size()==3) ctx->regs[operands[0]] = get_val(operands[1]) & get_val(operands[2]);
    else if (op == "orr" && operands.size()==3) ctx->regs[operands[0]] = get_val(operands[1]) | get_val(operands[2]);
    else if (op == "lsl" && operands.size()==3) ctx->regs[operands[0]] = get_val(operands[1]) << get_val(operands[2]);
    else if (op == "lsr" && operands.size()==3) ctx->regs[operands[0]] = (uint64_t)get_val(operands[1]) >> get_val(operands[2]);
    ctx->regs["pc"] += 4;
}

// Encode to hex (simplified: just return opcode bytes based on instruction)
static std::string encode_hex(const std::string& arch, const std::string& mn) {
    std::string s = mn;
    for (auto& c : s) c = tolower(c);
    while (!s.empty() && s[0]==' ') s.erase(0,1);
    size_t sp = s.find(' ');
    std::string op = (sp==std::string::npos) ? s : s.substr(0,sp);

    if (arch == "x86_64") {
        if (op=="nop") return "90";
        if (op=="ret") return "C3";
        if (op=="push") return "50";
        if (op=="pop")  return "58";
        if (op=="mov")  return "48 89 C0";
        if (op=="add")  return "48 01 C0";
        if (op=="sub")  return "48 29 C0";
        if (op=="xor")  return "48 31 C0";
        if (op=="inc")  return "48 FF C0";
        if (op=="dec")  return "48 FF C8";
        return "90"; // default nop
    } else { // arm64/arm32
        if (op=="nop") return "1F 20 03 D5";
        if (op=="ret") return "C0 03 5F D6";
        if (op=="mov") return "E0 03 00 AA";
        if (op=="add") return "00 00 00 8B";
        if (op=="sub") return "00 00 00 CB";
        return "1F 20 03 D5";
    }
}

static std::string decode_hex(const std::string& arch, const std::string& hex) {
    if (arch == "x86_64") {
        if (hex=="90" || hex=="90 90") return "NOP";
        if (hex=="C3") return "RET";
        if (hex.find("48 89")!=std::string::npos) return "MOV RAX, RBX";
        if (hex.find("48 01")!=std::string::npos) return "ADD RAX, RBX";
        if (hex.find("48 29")!=std::string::npos) return "SUB RAX, RBX";
        if (hex.find("48 31")!=std::string::npos) return "XOR RAX, RBX";
        if (hex.find("50")!=std::string::npos) return "PUSH RAX";
        if (hex.find("58")!=std::string::npos) return "POP RAX";
        return "NOP";
    } else {
        if (hex.find("1F 20")!=std::string::npos) return "NOP";
        if (hex.find("C0 03 5F")!=std::string::npos) return "RET";
        if (hex.find("AA")!=std::string::npos) return "MOV X0, X0";
        if (hex.find("8B")!=std::string::npos) return "ADD X0, X0, X1";
        return "NOP";
    }
}

void* asm_context_create(const char* arch) {
    auto* ctx = new AsmContext(); ctx->arch = arch; ctx->init_regs(); return ctx;
}
void asm_context_destroy(void* p) { delete (AsmContext*)p; }
const char* asm_context_arch(void* p) { static std::string s; s=((AsmContext*)p)->arch; return s.c_str(); }

const char* asm_assemble(void* p, const char* mn) {
    static std::string s; s = encode_hex(((AsmContext*)p)->arch, mn); return s.c_str();
}
const char* asm_disassemble(void* p, const char* hex) {
    static std::string s; s = decode_hex(((AsmContext*)p)->arch, hex); return s.c_str();
}
void asm_execute(void* p, const char* mn) {
    auto* ctx = (AsmContext*)p;
    ctx->history.push_back(mn);
    if (ctx->arch == "x86_64") sim_x86(ctx, mn);
    else sim_arm64(ctx, mn);
}
void asm_set_reg(void* p, const char* reg, int64_t value) { ((AsmContext*)p)->regs[reg] = value; }
int64_t asm_get_reg(void* p, const char* reg) {
    auto* ctx = (AsmContext*)p;
    auto it = ctx->regs.find(reg);
    return it != ctx->regs.end() ? it->second : 0;
}
const char* asm_dump_regs(void* p) {
    auto* ctx = (AsmContext*)p;
    static std::string s;
    std::ostringstream oss;
    oss << "Registers (" << ctx->arch << "):\n";
    for (auto& [k,v] : ctx->regs) oss << "  " << k << " = 0x" << std::hex << v << "\n";
    s = oss.str(); return s.c_str();
}
int asm_instruction_count(void* p) { return (int)((AsmContext*)p)->history.size(); }
const char* asm_instruction_at(void* p, int i) {
    static std::string s; s=((AsmContext*)p)->history[i]; return s.c_str();
}
const char* asm_optimize(void* p, const char* mn) {
    static std::string s;
    std::string m = mn;
    for (auto& c : m) c = tolower(c);
    // Simple peephole: XOR reg,reg -> zero; MOV reg,0 -> XOR reg,reg
    if (((AsmContext*)p)->arch == "x86_64") {
        if (m.find("mov") != std::string::npos && m.find(",0") != std::string::npos) {
            size_t sp = m.find(' ');
            std::string reg = (sp!=std::string::npos) ? m.substr(sp+1, m.find(',')-sp-1) : "rax";
            while (!reg.empty() && reg[0]==' ') reg.erase(0,1);
            while (!reg.empty() && reg.back()==' ') reg.pop_back();
            s = "XOR " + reg + ", " + reg + "  ; optimized";
        } else {
            s = std::string(mn) + "  ; no optimization";
        }
    } else {
        s = std::string(mn) + "  ; no optimization";
    }
    return s.c_str();
}

// ============================================================
// Phase 2: GPIO & Hardware
// ============================================================

struct GpioController {
    std::string board;
    int pin_count;
};

struct GpioPin {
    GpioController* ctrl;
    int pin_num;
    std::string mode;
    std::string pull = "none";
    int value = 0;
};

struct PwmDevice {
    GpioPin* pin;
    int frequency;
    float duty_cycle;
    int running = 0;
};

struct AdcDevice {
    GpioController* ctrl;
    int channel, resolution_bits;
    int raw_value = 0;
};

struct DacDevice {
    GpioController* ctrl;
    int channel, resolution_bits;
    int raw_value = 0;
};

struct SpiBus {
    GpioController* ctrl;
    int bus_num, clock_hz;
    std::string last_rx;
};

struct I2cBus {
    GpioController* ctrl;
    int bus_num, clock_hz;
    std::unordered_map<int, std::string> device_data;
};

struct UartDevice {
    GpioController* ctrl;
    int port, baud_rate;
    std::deque<std::string> rx_buffer;
};

static int board_pin_count(const std::string& b) {
    if (b=="raspberry_pi") return 40;
    if (b=="arduino") return 20;
    if (b=="esp32") return 38;
    if (b=="stm32") return 64;
    return 32;
}

void* gpio_controller_create(const char* board) {
    auto* c = new GpioController(); c->board=board; c->pin_count=board_pin_count(board); return c;
}
void gpio_controller_destroy(void* p) { delete (GpioController*)p; }
const char* gpio_controller_board(void* p) { static std::string s; s=((GpioController*)p)->board; return s.c_str(); }
int gpio_controller_pin_count(void* p) { return ((GpioController*)p)->pin_count; }

void* gpio_pin_create(void* ctrl, int pin_num, const char* mode) {
    auto* p = new GpioPin(); p->ctrl=(GpioController*)ctrl; p->pin_num=pin_num; p->mode=mode; return p;
}
void gpio_pin_destroy(void* p) { delete (GpioPin*)p; }
void gpio_pin_write(void* p, int v) { ((GpioPin*)p)->value = v & 1; }
int  gpio_pin_read(void* p) { return ((GpioPin*)p)->value; }
int  gpio_pin_number(void* p) { return ((GpioPin*)p)->pin_num; }
const char* gpio_pin_mode(void* p) { static std::string s; s=((GpioPin*)p)->mode; return s.c_str(); }
void gpio_pin_set_pull(void* p, const char* pull) { ((GpioPin*)p)->pull = pull; }
const char* gpio_pin_to_string(void* p) {
    auto* pin=(GpioPin*)p;
    static std::string s;
    std::ostringstream oss;
    oss << "GPIO(pin=" << pin->pin_num << " mode=" << pin->mode
        << " value=" << pin->value << " pull=" << pin->pull << ")";
    s = oss.str(); return s.c_str();
}

void* pwm_create(void* pin, int freq, float duty) {
    auto* pw = new PwmDevice(); pw->pin=(GpioPin*)pin; pw->frequency=freq; pw->duty_cycle=duty; return pw;
}
void pwm_destroy(void* p) { delete (PwmDevice*)p; }
void pwm_set_frequency(void* p, int f) { ((PwmDevice*)p)->frequency = f; }
void pwm_set_duty_cycle(void* p, float d) { ((PwmDevice*)p)->duty_cycle = d; }
void pwm_start(void* p) { ((PwmDevice*)p)->running = 1; }
void pwm_stop(void* p) { ((PwmDevice*)p)->running = 0; }
int  pwm_frequency(void* p) { return ((PwmDevice*)p)->frequency; }
float pwm_duty_cycle(void* p) { return ((PwmDevice*)p)->duty_cycle; }
int  pwm_is_running(void* p) { return ((PwmDevice*)p)->running; }

void* adc_create(void* ctrl, int ch, int bits) {
    auto* a = new AdcDevice(); a->ctrl=(GpioController*)ctrl; a->channel=ch; a->resolution_bits=bits;
    a->raw_value = (1 << (bits-1)); // mid-scale default
    return a;
}
void adc_destroy(void* p) { delete (AdcDevice*)p; }
int  adc_read_raw(void* p) { return ((AdcDevice*)p)->raw_value; }
float adc_read_voltage(void* p, float vref) {
    auto* a=(AdcDevice*)p;
    return (float)a->raw_value / (float)((1<<a->resolution_bits)-1) * vref;
}
int  adc_resolution(void* p) { return ((AdcDevice*)p)->resolution_bits; }
int  adc_channel(void* p) { return ((AdcDevice*)p)->channel; }

void* dac_create(void* ctrl, int ch, int bits) {
    auto* d = new DacDevice(); d->ctrl=(GpioController*)ctrl; d->channel=ch; d->resolution_bits=bits; return d;
}
void dac_destroy(void* p) { delete (DacDevice*)p; }
void dac_write_raw(void* p, int v) { ((DacDevice*)p)->raw_value = v; }
void dac_write_voltage(void* p, float voltage, float vref) {
    auto* d=(DacDevice*)p;
    d->raw_value = (int)(voltage / vref * ((1<<d->resolution_bits)-1));
}
int  dac_resolution(void* p) { return ((DacDevice*)p)->resolution_bits; }
int  dac_channel(void* p) { return ((DacDevice*)p)->channel; }

void* spi_bus_create(void* ctrl, int bus, int clk) {
    auto* s = new SpiBus(); s->ctrl=(GpioController*)ctrl; s->bus_num=bus; s->clock_hz=clk; return s;
}
void spi_bus_destroy(void* p) { delete (SpiBus*)p; }
const char* spi_transfer(void* p, const char* data) {
    auto* s=(SpiBus*)p;
    // Echo back with bytes incremented by 1 (simulate MISO)
    std::string in = data;
    std::ostringstream oss;
    std::istringstream iss(in);
    std::string byte;
    bool first = true;
    while (iss >> byte) {
        if (!first) oss << " ";
        try {
            int val = std::stoi(byte, nullptr, 16);
            oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << ((val+1)&0xFF);
        } catch(...) { oss << "00"; }
        first = false;
    }
    s->last_rx = oss.str();
    static std::string ret; ret = s->last_rx; return ret.c_str();
}
void spi_set_clock(void* p, int hz) { ((SpiBus*)p)->clock_hz = hz; }
int  spi_clock(void* p) { return ((SpiBus*)p)->clock_hz; }
int  spi_bus_num(void* p) { return ((SpiBus*)p)->bus_num; }

void* i2c_bus_create(void* ctrl, int bus, int clk) {
    auto* i = new I2cBus(); i->ctrl=(GpioController*)ctrl; i->bus_num=bus; i->clock_hz=clk; return i;
}
void i2c_bus_destroy(void* p) { delete (I2cBus*)p; }
int  i2c_write(void* p, int addr, const char* data) {
    ((I2cBus*)p)->device_data[addr] = data; return 1;
}
const char* i2c_read(void* p, int addr, int /*num_bytes*/) {
    static std::string s;
    auto* i=(I2cBus*)p;
    auto it = i->device_data.find(addr);
    s = (it != i->device_data.end()) ? it->second : "00";
    return s.c_str();
}
int i2c_scan(void* p) { return (int)((I2cBus*)p)->device_data.size(); }
int i2c_bus_num(void* p) { return ((I2cBus*)p)->bus_num; }

void* uart_create(void* ctrl, int port, int baud) {
    auto* u = new UartDevice(); u->ctrl=(GpioController*)ctrl; u->port=port; u->baud_rate=baud; return u;
}
void uart_destroy(void* p) { delete (UartDevice*)p; }
int  uart_write(void* p, const char* data) {
    // Loopback: push to rx buffer
    ((UartDevice*)p)->rx_buffer.push_back(data); return (int)strlen(data);
}
const char* uart_read(void* p, int /*max_bytes*/) {
    static std::string s;
    auto* u=(UartDevice*)p;
    if (u->rx_buffer.empty()) { s=""; return s.c_str(); }
    s = u->rx_buffer.front(); u->rx_buffer.pop_front(); return s.c_str();
}
void uart_set_baud(void* p, int b) { ((UartDevice*)p)->baud_rate = b; }
int  uart_baud(void* p) { return ((UartDevice*)p)->baud_rate; }
int  uart_port(void* p) { return ((UartDevice*)p)->port; }
int  uart_bytes_available(void* p) {
    auto* u=(UartDevice*)p;
    int n=0; for (auto& s : u->rx_buffer) n+=(int)s.size();
    return n;
}

// ============================================================
// Phase 3: RTOS Integration
// ============================================================

struct RtosTask {
    std::string name;
    int priority, stack_size;
    std::string state; // "ready"|"running"|"suspended"|"deleted"
};

struct RtosKernel {
    std::string name;
    int running = 0;
    int tick_count = 0;
    std::vector<RtosTask*> tasks;
};

struct RtosSemaphore {
    int count, max_count;
};

struct RtosMutex {
    int locked = 0;
};

struct RtosQueue {
    int capacity, item_size;
    std::deque<std::string> items;
};

struct RtosTimer {
    std::string name;
    int period_ms, auto_reload;
    int active = 0;
    int fire_count = 0;
};

void* rtos_kernel_create(const char* name) {
    auto* k = new RtosKernel(); k->name=name; return k;
}
void rtos_kernel_destroy(void* p) {
    auto* k=(RtosKernel*)p;
    for (auto* t : k->tasks) delete t;
    delete k;
}
void rtos_kernel_start(void* p) { ((RtosKernel*)p)->running = 1; }
void rtos_kernel_stop(void* p) { ((RtosKernel*)p)->running = 0; }
int  rtos_kernel_is_running(void* p) { return ((RtosKernel*)p)->running; }
const char* rtos_kernel_name(void* p) { static std::string s; s=((RtosKernel*)p)->name; return s.c_str(); }
int  rtos_kernel_tick(void* p) { return ++((RtosKernel*)p)->tick_count; }
int  rtos_kernel_task_count(void* p) { return (int)((RtosKernel*)p)->tasks.size(); }

void* rtos_task_create(void* kernel, const char* name, int priority, int stack_size) {
    auto* t = new RtosTask(); t->name=name; t->priority=priority; t->stack_size=stack_size; t->state="ready";
    ((RtosKernel*)kernel)->tasks.push_back(t);
    return t;
}
void rtos_task_destroy(void* p) { delete (RtosTask*)p; }
void rtos_task_start(void* p) { ((RtosTask*)p)->state = "running"; }
void rtos_task_suspend(void* p) { ((RtosTask*)p)->state = "suspended"; }
void rtos_task_resume(void* p) { ((RtosTask*)p)->state = "ready"; }
void rtos_task_delete(void* p) { ((RtosTask*)p)->state = "deleted"; }
const char* rtos_task_name(void* p) { static std::string s; s=((RtosTask*)p)->name; return s.c_str(); }
int  rtos_task_priority(void* p) { return ((RtosTask*)p)->priority; }
const char* rtos_task_state(void* p) { static std::string s; s=((RtosTask*)p)->state; return s.c_str(); }
int  rtos_task_stack_size(void* p) { return ((RtosTask*)p)->stack_size; }

void* rtos_semaphore_create(int init, int max) {
    auto* s = new RtosSemaphore(); s->count=init; s->max_count=max; return s;
}
void rtos_semaphore_destroy(void* p) { delete (RtosSemaphore*)p; }
int  rtos_semaphore_take(void* p, int /*timeout_ms*/) {
    auto* s=(RtosSemaphore*)p;
    if (s->count > 0) { s->count--; return 1; }
    return 0;
}
int  rtos_semaphore_give(void* p) {
    auto* s=(RtosSemaphore*)p;
    if (s->count < s->max_count) { s->count++; return 1; }
    return 0;
}
int  rtos_semaphore_count(void* p) { return ((RtosSemaphore*)p)->count; }

void* rtos_mutex_create() { return new RtosMutex(); }
void  rtos_mutex_destroy(void* p) { delete (RtosMutex*)p; }
int   rtos_mutex_lock(void* p, int /*timeout_ms*/) {
    auto* m=(RtosMutex*)p;
    if (!m->locked) { m->locked=1; return 1; }
    return 0;
}
int   rtos_mutex_unlock(void* p) { ((RtosMutex*)p)->locked=0; return 1; }
int   rtos_mutex_is_locked(void* p) { return ((RtosMutex*)p)->locked; }

void* rtos_queue_create(int cap, int item_size) {
    auto* q = new RtosQueue(); q->capacity=cap; q->item_size=item_size; return q;
}
void rtos_queue_destroy(void* p) { delete (RtosQueue*)p; }
int  rtos_queue_send(void* p, const char* data, int /*timeout_ms*/) {
    auto* q=(RtosQueue*)p;
    if ((int)q->items.size() >= q->capacity) return 0;
    q->items.push_back(data); return 1;
}
const char* rtos_queue_receive(void* p, int /*timeout_ms*/) {
    static std::string s;
    auto* q=(RtosQueue*)p;
    if (q->items.empty()) { s=""; return s.c_str(); }
    s = q->items.front(); q->items.pop_front(); return s.c_str();
}
int  rtos_queue_size(void* p) { return (int)((RtosQueue*)p)->items.size(); }
int  rtos_queue_capacity(void* p) { return ((RtosQueue*)p)->capacity; }
int  rtos_queue_is_full(void* p) { auto* q=(RtosQueue*)p; return (int)q->items.size()>=q->capacity ? 1:0; }
int  rtos_queue_is_empty(void* p) { return ((RtosQueue*)p)->items.empty() ? 1:0; }

void* rtos_timer_create(const char* name, int period_ms, int auto_reload) {
    auto* t = new RtosTimer(); t->name=name; t->period_ms=period_ms; t->auto_reload=auto_reload; return t;
}
void rtos_timer_destroy(void* p) { delete (RtosTimer*)p; }
void rtos_timer_start(void* p) { ((RtosTimer*)p)->active = 1; }
void rtos_timer_stop(void* p) { ((RtosTimer*)p)->active = 0; }
void rtos_timer_reset(void* p) { auto* t=(RtosTimer*)p; t->active=1; t->fire_count=0; }
int  rtos_timer_is_active(void* p) { return ((RtosTimer*)p)->active; }
int  rtos_timer_period(void* p) { return ((RtosTimer*)p)->period_ms; }
int  rtos_timer_fire_count(void* p) { return ((RtosTimer*)p)->fire_count; }
const char* rtos_timer_name(void* p) { static std::string s; s=((RtosTimer*)p)->name; return s.c_str(); }

// ============================================================
// Phase 4: Bootloader & Kernel
// ============================================================

struct MemRegion {
    std::string name, type;
    uint64_t addr, size;
};

struct MemMap {
    uint64_t base_addr, total_size;
    std::vector<MemRegion> regions;
};

struct IvtEntry {
    std::string name, handler;
    int fire_count = 0;
    int registered = 0;
};

struct InterruptTable {
    std::vector<IvtEntry> entries;
};

struct ProcessEntry {
    int pid;
    std::string name, state;
    int priority;
    int alive = 1;
};

struct ProcessTable {
    int max_processes;
    std::vector<ProcessEntry> processes;
    int next_pid = 1;
    int rr_index = 0;
};

struct KernelObj {
    std::string name, arch, state;
    std::vector<std::string> log_entries;
    std::chrono::steady_clock::time_point boot_time;
};

void* mem_map_create(uint64_t base, uint64_t size) {
    auto* m = new MemMap(); m->base_addr=base; m->total_size=size; return m;
}
void mem_map_destroy(void* p) { delete (MemMap*)p; }
void mem_map_add_region(void* p, const char* name, uint64_t addr, uint64_t size, const char* type) {
    auto* m=(MemMap*)p;
    MemRegion r; r.name=name; r.addr=addr; r.size=size; r.type=type;
    m->regions.push_back(r);
}
int  mem_map_region_count(void* p) { return (int)((MemMap*)p)->regions.size(); }
const char* mem_map_region_name(void* p, int i) { static std::string s; s=((MemMap*)p)->regions[i].name; return s.c_str(); }
uint64_t mem_map_region_addr(void* p, int i) { return ((MemMap*)p)->regions[i].addr; }
uint64_t mem_map_region_size(void* p, int i) { return ((MemMap*)p)->regions[i].size; }
const char* mem_map_region_type(void* p, int i) { static std::string s; s=((MemMap*)p)->regions[i].type; return s.c_str(); }
const char* mem_map_to_string(void* p) {
    auto* m=(MemMap*)p;
    static std::string s;
    std::ostringstream oss;
    oss << "MemoryMap(base=0x" << std::hex << m->base_addr << " size=0x" << m->total_size << ")\n";
    for (auto& r : m->regions)
        oss << "  [" << r.type << "] " << r.name << " @ 0x" << r.addr << " size=0x" << r.size << "\n";
    s = oss.str(); return s.c_str();
}

void* interrupt_table_create(int num_vectors) {
    auto* t = new InterruptTable(); t->entries.resize(num_vectors); return t;
}
void interrupt_table_destroy(void* p) { delete (InterruptTable*)p; }
void interrupt_table_register(void* p, int vec, const char* name, const char* handler) {
    auto* t=(InterruptTable*)p;
    if (vec >= 0 && vec < (int)t->entries.size()) {
        t->entries[vec].name=name; t->entries[vec].handler=handler; t->entries[vec].registered=1;
    }
}
int interrupt_table_fire(void* p, int vec) {
    auto* t=(InterruptTable*)p;
    if (vec >= 0 && vec < (int)t->entries.size() && t->entries[vec].registered) {
        t->entries[vec].fire_count++; return 1;
    }
    return 0;
}
const char* interrupt_table_handler(void* p, int vec) {
    static std::string s;
    auto* t=(InterruptTable*)p;
    s = (vec>=0 && vec<(int)t->entries.size()) ? t->entries[vec].handler : "";
    return s.c_str();
}
const char* interrupt_table_name(void* p, int vec) {
    static std::string s;
    auto* t=(InterruptTable*)p;
    s = (vec>=0 && vec<(int)t->entries.size()) ? t->entries[vec].name : "";
    return s.c_str();
}
int interrupt_table_fire_count(void* p, int vec) {
    auto* t=(InterruptTable*)p;
    return (vec>=0 && vec<(int)t->entries.size()) ? t->entries[vec].fire_count : 0;
}
int interrupt_table_size(void* p) { return (int)((InterruptTable*)p)->entries.size(); }

void* process_table_create(int max) {
    auto* t = new ProcessTable(); t->max_processes=max; return t;
}
void process_table_destroy(void* p) { delete (ProcessTable*)p; }
int  process_table_create_process(void* p, const char* name, int priority) {
    auto* t=(ProcessTable*)p;
    if ((int)t->processes.size() >= t->max_processes) return -1;
    ProcessEntry e; e.pid=t->next_pid++; e.name=name; e.priority=priority; e.state="ready"; e.alive=1;
    t->processes.push_back(e);
    return e.pid;
}
void process_table_kill(void* p, int pid) {
    auto* t=(ProcessTable*)p;
    for (auto& e : t->processes) if (e.pid==pid) { e.state="zombie"; e.alive=0; }
}
const char* process_table_name(void* p, int pid) {
    static std::string s; s="";
    for (auto& e : ((ProcessTable*)p)->processes) if (e.pid==pid) { s=e.name; break; }
    return s.c_str();
}
int process_table_priority(void* p, int pid) {
    for (auto& e : ((ProcessTable*)p)->processes) if (e.pid==pid) return e.priority;
    return -1;
}
const char* process_table_state(void* p, int pid) {
    static std::string s; s="";
    for (auto& e : ((ProcessTable*)p)->processes) if (e.pid==pid) { s=e.state; break; }
    return s.c_str();
}
int process_table_count(void* p) { return (int)((ProcessTable*)p)->processes.size(); }
int process_table_schedule(void* p) {
    auto* t=(ProcessTable*)p;
    // Round-robin among alive processes
    int n = (int)t->processes.size();
    if (n == 0) return -1;
    for (int i=0; i<n; i++) {
        int idx = (t->rr_index + i) % n;
        if (t->processes[idx].alive) {
            t->rr_index = (idx+1) % n;
            return t->processes[idx].pid;
        }
    }
    return -1;
}
const char* process_table_to_string(void* p) {
    auto* t=(ProcessTable*)p;
    static std::string s;
    std::ostringstream oss;
    oss << "ProcessTable(" << t->processes.size() << " processes):\n";
    for (auto& e : t->processes)
        oss << "  PID=" << e.pid << " name=" << e.name << " state=" << e.state << " prio=" << e.priority << "\n";
    s = oss.str(); return s.c_str();
}

void* kernel_create(const char* name, const char* arch) {
    auto* k = new KernelObj(); k->name=name; k->arch=arch; k->state="offline"; return k;
}
void kernel_destroy(void* p) { delete (KernelObj*)p; }
void kernel_boot(void* p) {
    auto* k=(KernelObj*)p;
    k->state="booting";
    k->boot_time = std::chrono::steady_clock::now();
    k->log_entries.push_back("[BOOT] Kernel '" + k->name + "' booting on " + k->arch);
    k->log_entries.push_back("[BOOT] Memory subsystem initialized");
    k->log_entries.push_back("[BOOT] Interrupt table loaded");
    k->log_entries.push_back("[BOOT] Scheduler started");
    k->state = "running";
    k->log_entries.push_back("[BOOT] Kernel running");
}
void kernel_panic(void* p, const char* msg) {
    auto* k=(KernelObj*)p;
    k->state = "panic";
    k->log_entries.push_back(std::string("[PANIC] ") + msg);
}
const char* kernel_name(void* p) { static std::string s; s=((KernelObj*)p)->name; return s.c_str(); }
const char* kernel_arch(void* p) { static std::string s; s=((KernelObj*)p)->arch; return s.c_str(); }
const char* kernel_state(void* p) { static std::string s; s=((KernelObj*)p)->state; return s.c_str(); }
int kernel_uptime_ms(void* p) {
    auto* k=(KernelObj*)p;
    if (k->state == "offline") return 0;
    auto now = std::chrono::steady_clock::now();
    return (int)std::chrono::duration_cast<std::chrono::milliseconds>(now - k->boot_time).count();
}
void kernel_syscall(void* p, const char* call, const char* args) {
    auto* k=(KernelObj*)p;
    k->log_entries.push_back(std::string("[SYSCALL] ") + call + "(" + args + ")");
}
const char* kernel_log(void* p) {
    auto* k=(KernelObj*)p;
    static std::string s;
    std::ostringstream oss;
    for (auto& l : k->log_entries) oss << l << "\n";
    s = oss.str(); return s.c_str();
}
int kernel_log_count(void* p) { return (int)((KernelObj*)p)->log_entries.size(); }
