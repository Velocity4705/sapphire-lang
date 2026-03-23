#include "os.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <cstdlib>

// ============================================================
// Bootloader
// ============================================================
struct BootLoader {
    std::string disk_image;
    std::string mode = "real";  // real -> protected -> long
    std::string status = "idle";
    bool mbr_loaded = false;
    bool stage2_loaded = false;
    bool kernel_loaded = false;
    struct MemEntry { uint64_t base, len; std::string type; };
    std::vector<MemEntry> memory_map;
    BootLoader(const char* img) : disk_image(img) {
        // Typical BIOS memory map
        memory_map.push_back({0x0,       0x9FC00,  "usable"});
        memory_map.push_back({0x9FC00,   0x400,    "reserved"});
        memory_map.push_back({0xF0000,   0x10000,  "reserved"});
        memory_map.push_back({0x100000,  0x7EF0000,"usable"});
        memory_map.push_back({0xFEC00000,0x400000, "reserved"});
    }
};

extern "C" {

void* boot_create(const char* disk_image) { return new BootLoader(disk_image); }
void  boot_destroy(void* b) { delete (BootLoader*)b; }

int boot_load_mbr(void* b) {
    auto* bl = (BootLoader*)b;
    bl->mbr_loaded = true;
    bl->status = "MBR loaded (512 bytes, signature 0x55AA)";
    return 1;
}
int boot_load_stage2(void* b, uint32_t lba, uint32_t sectors) {
    auto* bl = (BootLoader*)b;
    bl->stage2_loaded = true;
    bl->status = "Stage2 loaded from LBA " + std::to_string(lba) + " (" + std::to_string(sectors) + " sectors)";
    return 1;
}
int boot_load_kernel(void* b, uint32_t lba, uint32_t size) {
    auto* bl = (BootLoader*)b;
    bl->kernel_loaded = true;
    bl->status = "Kernel loaded from LBA " + std::to_string(lba) + " size=" + std::to_string(size) + " bytes";
    return 1;
}
const char* boot_status(void* b) {
    static std::string s;
    s = ((BootLoader*)b)->status;
    return s.c_str();
}
int boot_get_memory_map_count(void* b) { return (int)((BootLoader*)b)->memory_map.size(); }
const char* boot_get_memory_map_entry(void* b, int idx) {
    static std::string s;
    auto* bl = (BootLoader*)b;
    if (idx < 0 || idx >= (int)bl->memory_map.size()) return "";
    auto& e = bl->memory_map[idx];
    s = std::to_string(e.base) + "," + std::to_string(e.len) + "," + e.type;
    return s.c_str();
}
void boot_enter_protected_mode(void* b) {
    auto* bl = (BootLoader*)b;
    bl->mode = "protected";
    bl->status = "Entered 32-bit protected mode (GDT loaded, PE bit set)";
}
void boot_enter_long_mode(void* b) {
    auto* bl = (BootLoader*)b;
    bl->mode = "long";
    bl->status = "Entered 64-bit long mode (PAE paging enabled, LME bit set)";
}
const char* boot_mode(void* b) { return ((BootLoader*)b)->mode.c_str(); }

// ============================================================
// Kernel
// ============================================================
struct ISR { std::string name; int count = 0; };
struct VGACell { char ch = ' '; int color = 7; };

struct Kernel {
    std::string name;
    bool idt_init = false, gdt_init = false, pic_init = false, pit_init = false;
    uint32_t pit_hz = 0;
    std::map<int, ISR> isrs;
    std::vector<std::vector<VGACell>> vga;
    int os_vga_rows = 25, os_vga_cols = 80;
    std::string status_str;
    Kernel(const char* n) : name(n) {
        vga.assign(os_vga_rows, std::vector<VGACell>(os_vga_cols));
    }
};

void* os_kernel_create(const char* name) { return new Kernel(name); }
void  os_kernel_destroy(void* k) { delete (Kernel*)k; }
void  os_kernel_init_idt(void* k) { ((Kernel*)k)->idt_init = true; }
void  os_kernel_init_gdt(void* k) { ((Kernel*)k)->gdt_init = true; }
void  os_kernel_init_pic(void* k) { ((Kernel*)k)->pic_init = true; }
void  os_kernel_init_pit(void* k, uint32_t hz) { auto* kk=(Kernel*)k; kk->pit_init=true; kk->pit_hz=hz; }
void  os_kernel_register_isr(void* k, int vector, const char* name) {
    ((Kernel*)k)->isrs[vector] = {name, 0};
}
int   os_kernel_isr_count(void* k) { return (int)((Kernel*)k)->isrs.size(); }
const char* os_kernel_isr_name(void* k, int vector) {
    static std::string s;
    auto* kk = (Kernel*)k;
    auto it = kk->isrs.find(vector);
    if (it == kk->isrs.end()) return "";
    s = it->second.name; return s.c_str();
}
void os_kernel_trigger_interrupt(void* k, int vector) {
    auto* kk = (Kernel*)k;
    if (kk->isrs.count(vector)) kk->isrs[vector].count++;
}
int os_kernel_interrupt_count(void* k, int vector) {
    auto* kk = (Kernel*)k;
    auto it = kk->isrs.find(vector);
    return it == kk->isrs.end() ? 0 : it->second.count;
}
void os_kernel_vga_write(void* k, int row, int col, const char* text, int color) {
    auto* kk = (Kernel*)k;
    if (row < 0 || row >= kk->os_vga_rows) return;
    for (int i = 0; text[i] && col+i < kk->os_vga_cols; i++)
        kk->vga[row][col+i] = {text[i], color};
}
const char* os_kernel_vga_read(void* k, int row, int col) {
    static std::string s;
    auto* kk = (Kernel*)k;
    if (row < 0 || row >= kk->os_vga_rows || col < 0 || col >= kk->os_vga_cols) return "";
    s = std::string(1, kk->vga[row][col].ch); return s.c_str();
}
void os_kernel_vga_clear(void* k) {
    auto* kk = (Kernel*)k;
    for (auto& row : kk->vga) for (auto& c : row) c = {' ', 7};
}
const char* os_kernel_status(void* k) {
    static std::string s;
    auto* kk = (Kernel*)k;
    s = "Kernel[" + kk->name + "] GDT=" + (kk->gdt_init?"ok":"no")
      + " IDT=" + (kk->idt_init?"ok":"no")
      + " PIC=" + (kk->pic_init?"ok":"no")
      + " PIT=" + (kk->pit_init ? std::to_string(kk->pit_hz)+"Hz" : "no")
      + " ISRs=" + std::to_string(kk->isrs.size());
    return s.c_str();
}

// ============================================================
// Physical Memory Manager
// ============================================================
struct PMM {
    uint32_t page_sz;
    int total_pages;
    std::vector<bool> free_map;  // true = free
    PMM(uint64_t total_bytes, uint32_t ps) : page_sz(ps) {
        total_pages = (int)(total_bytes / ps);
        free_map.assign(total_pages, false);  // all reserved by default
    }
};

void* pmm_create(uint64_t total_bytes, uint32_t page_size) {
    return new PMM(total_bytes, page_size);
}
void pmm_destroy(void* pmm) { delete (PMM*)pmm; }
void pmm_mark_free(void* pmm, uint64_t base, uint64_t length) {
    auto* p = (PMM*)pmm;
    int start = (int)(base / p->page_sz);
    int count = (int)(length / p->page_sz);
    for (int i = start; i < start+count && i < p->total_pages; i++) p->free_map[i] = true;
}
void pmm_mark_used(void* pmm, uint64_t base, uint64_t length) {
    auto* p = (PMM*)pmm;
    int start = (int)(base / p->page_sz);
    int count = (int)(length / p->page_sz);
    for (int i = start; i < start+count && i < p->total_pages; i++) p->free_map[i] = false;
}
uint64_t pmm_alloc_page(void* pmm) {
    auto* p = (PMM*)pmm;
    for (int i = 0; i < p->total_pages; i++) {
        if (p->free_map[i]) { p->free_map[i] = false; return (uint64_t)i * p->page_sz; }
    }
    return (uint64_t)-1;
}
void pmm_free_page(void* pmm, uint64_t addr) {
    auto* p = (PMM*)pmm;
    int idx = (int)(addr / p->page_sz);
    if (idx >= 0 && idx < p->total_pages) p->free_map[idx] = true;
}
uint64_t pmm_alloc_pages(void* pmm, int count) {
    auto* p = (PMM*)pmm;
    for (int i = 0; i <= p->total_pages - count; i++) {
        bool ok = true;
        for (int j = 0; j < count; j++) if (!p->free_map[i+j]) { ok=false; break; }
        if (ok) {
            for (int j = 0; j < count; j++) p->free_map[i+j] = false;
            return (uint64_t)i * p->page_sz;
        }
    }
    return (uint64_t)-1;
}
int pmm_free_pages_count(void* pmm) {
    auto* p = (PMM*)pmm;
    return (int)std::count(p->free_map.begin(), p->free_map.end(), true);
}
int pmm_used_pages_count(void* pmm) {
    auto* p = (PMM*)pmm;
    return (int)std::count(p->free_map.begin(), p->free_map.end(), false);
}
int pmm_total_pages(void* pmm) { return ((PMM*)pmm)->total_pages; }
uint32_t pmm_page_size(void* pmm) { return ((PMM*)pmm)->page_sz; }

// ============================================================
// Virtual Memory Manager
// ============================================================
struct VMMEntry { uint64_t phys; int flags; };
struct VMM {
    std::map<uint64_t, VMMEntry> table;  // virt -> entry
    int page_faults = 0;
    std::string dump_cache;
};

void* vmm_create(void* /*pmm*/) { return new VMM(); }
void  vmm_destroy(void* vmm) { delete (VMM*)vmm; }
int   vmm_map(void* vmm, uint64_t virt, uint64_t phys, int flags) {
    ((VMM*)vmm)->table[virt] = {phys, flags}; return 1;
}
int   vmm_unmap(void* vmm, uint64_t virt) {
    auto* v = (VMM*)vmm;
    if (!v->table.count(virt)) { v->page_faults++; return 0; }
    v->table.erase(virt); return 1;
}
uint64_t vmm_translate(void* vmm, uint64_t virt) {
    auto* v = (VMM*)vmm;
    auto it = v->table.find(virt);
    if (it == v->table.end()) { v->page_faults++; return 0; }
    return it->second.phys;
}
int vmm_is_mapped(void* vmm, uint64_t virt) { return ((VMM*)vmm)->table.count(virt) ? 1 : 0; }
int vmm_page_fault_count(void* vmm) { return ((VMM*)vmm)->page_faults; }
const char* vmm_dump_table(void* vmm) {
    auto* v = (VMM*)vmm;
    std::ostringstream ss;
    ss << "{\"mappings\":[";
    bool first = true;
    for (auto& [virt, e] : v->table) {
        if (!first) ss << ",";
        ss << "{\"virt\":" << virt << ",\"phys\":" << e.phys << ",\"flags\":" << e.flags << "}";
        first = false;
    }
    ss << "]}";
    v->dump_cache = ss.str();
    return v->dump_cache.c_str();
}

// ============================================================
// Heap Allocator (simple free-list)
// ============================================================
struct HeapBlock { uint64_t addr, size; bool free; };
struct Heap {
    uint64_t base, total;
    std::vector<HeapBlock> blocks;
    Heap(uint64_t b, uint64_t s) : base(b), total(s) {
        blocks.push_back({b, s, true});
    }
};

void* heap_create(uint64_t base, uint64_t size) { return new Heap(base, size); }
void  heap_destroy(void* heap) { delete (Heap*)heap; }
uint64_t heap_alloc(void* heap, uint64_t size) {
    auto* h = (Heap*)heap;
    for (auto& b : h->blocks) {
        if (b.free && b.size >= size) {
            uint64_t addr = b.addr;
            if (b.size > size) {
                HeapBlock rem = {b.addr + size, b.size - size, true};
                b.size = size; b.free = false;
                h->blocks.push_back(rem);
            } else {
                b.free = false;
            }
            return addr;
        }
    }
    return 0;
}
void heap_free(void* heap, uint64_t addr) {
    auto* h = (Heap*)heap;
    for (auto& b : h->blocks) if (b.addr == addr) { b.free = true; return; }
}
uint64_t heap_used(void* heap) {
    auto* h = (Heap*)heap;
    uint64_t used = 0;
    for (auto& b : h->blocks) if (!b.free) used += b.size;
    return used;
}
uint64_t heap_free_space(void* heap) {
    auto* h = (Heap*)heap;
    uint64_t free = 0;
    for (auto& b : h->blocks) if (b.free) free += b.size;
    return free;
}
int heap_block_count(void* heap) { return (int)((Heap*)heap)->blocks.size(); }

// ============================================================
// Scheduler
// ============================================================
struct Process {
    int pid; std::string name, state;
    int priority; uint64_t ticks; uint64_t stack_size;
};
struct Scheduler {
    std::string policy;
    std::vector<Process> procs;
    int current_idx = 0;
    int next_pid = 1;
    Scheduler(const char* p) : policy(p) {}
};

void* scheduler_create(const char* policy) { return new Scheduler(policy); }
void  scheduler_destroy(void* sched) { delete (Scheduler*)sched; }
int   scheduler_add_process(void* sched, const char* name, int priority, uint64_t stack_size) {
    auto* s = (Scheduler*)sched;
    int pid = s->next_pid++;
    s->procs.push_back({pid, name, "ready", priority, 0, stack_size});
    return pid;
}
void scheduler_terminate(void* sched, int pid) {
    auto* s = (Scheduler*)sched;
    for (auto& p : s->procs) if (p.pid == pid) { p.state = "terminated"; return; }
}
int scheduler_tick(void* sched) {
    auto* s = (Scheduler*)sched;
    if (s->procs.empty()) return -1;
    // Find next ready process
    int n = (int)s->procs.size();
    for (int i = 0; i < n; i++) {
        int idx = (s->current_idx + i) % n;
        if (s->procs[idx].state == "ready" || s->procs[idx].state == "running") {
            if (s->current_idx != idx && s->procs[s->current_idx].state == "running")
                s->procs[s->current_idx].state = "ready";
            s->current_idx = idx;
            s->procs[idx].state = "running";
            s->procs[idx].ticks++;
            s->current_idx = (idx + 1) % n;  // advance for next tick
            return s->procs[idx].pid;
        }
    }
    return -1;
}
int scheduler_current_pid(void* sched) {
    auto* s = (Scheduler*)sched;
    for (auto& p : s->procs) if (p.state == "running") return p.pid;
    return -1;
}
const char* scheduler_current_name(void* sched) {
    static std::string st;
    auto* s = (Scheduler*)sched;
    for (auto& p : s->procs) if (p.state == "running") { st = p.name; return st.c_str(); }
    return "idle";
}
int scheduler_process_count(void* sched) { return (int)((Scheduler*)sched)->procs.size(); }
const char* scheduler_process_name(void* sched, int pid) {
    static std::string st;
    for (auto& p : ((Scheduler*)sched)->procs) if (p.pid == pid) { st = p.name; return st.c_str(); }
    return "";
}
const char* scheduler_process_state(void* sched, int pid) {
    static std::string st;
    for (auto& p : ((Scheduler*)sched)->procs) if (p.pid == pid) { st = p.state; return st.c_str(); }
    return "unknown";
}
int scheduler_process_priority(void* sched, int pid) {
    for (auto& p : ((Scheduler*)sched)->procs) if (p.pid == pid) return p.priority;
    return -1;
}
uint64_t scheduler_process_ticks(void* sched, int pid) {
    for (auto& p : ((Scheduler*)sched)->procs) if (p.pid == pid) return p.ticks;
    return 0;
}
void scheduler_block(void* sched, int pid) {
    for (auto& p : ((Scheduler*)sched)->procs) if (p.pid == pid) { p.state = "blocked"; return; }
}
void scheduler_unblock(void* sched, int pid) {
    for (auto& p : ((Scheduler*)sched)->procs) if (p.pid == pid) { p.state = "ready"; return; }
}

// ============================================================
// Syscall Table
// ============================================================
struct Syscall { std::string name; int arg_count; };
struct SyscallTable {
    std::map<int, Syscall> table;
    std::string result_cache;
};

void* syscall_table_create() { return new SyscallTable(); }
void  syscall_table_destroy(void* tbl) { delete (SyscallTable*)tbl; }
void  syscall_table_register(void* tbl, int num, const char* name, int arg_count) {
    ((SyscallTable*)tbl)->table[num] = {name, arg_count};
}
const char* syscall_table_invoke(void* tbl, int num, const char* args) {
    auto* t = (SyscallTable*)tbl;
    auto it = t->table.find(num);
    if (it == t->table.end()) { t->result_cache = "ENOSYS"; return t->result_cache.c_str(); }
    t->result_cache = "syscall[" + it->second.name + "](" + std::string(args) + ")=0";
    return t->result_cache.c_str();
}
int syscall_table_count(void* tbl) { return (int)((SyscallTable*)tbl)->table.size(); }
const char* syscall_table_name(void* tbl, int num) {
    static std::string s;
    auto it = ((SyscallTable*)tbl)->table.find(num);
    if (it == ((SyscallTable*)tbl)->table.end()) return "";
    s = it->second.name; return s.c_str();
}

// ============================================================
// Device Driver
// ============================================================
struct Driver {
    std::string name, type;
    std::map<uint64_t, uint64_t> regs;
    int irq_count = 0;
    bool ready = true;
};

void* driver_create(const char* name, const char* type) { return new Driver{name, type}; }
void  driver_destroy(void* drv) { delete (Driver*)drv; }
void  driver_write(void* drv, uint64_t addr, uint64_t value, int /*width*/) {
    ((Driver*)drv)->regs[addr] = value;
}
uint64_t driver_read(void* drv, uint64_t addr, int /*width*/) {
    auto* d = (Driver*)drv;
    auto it = d->regs.find(addr);
    return it == d->regs.end() ? 0 : it->second;
}
void driver_irq_fire(void* drv) { ((Driver*)drv)->irq_count++; }
int  driver_irq_count(void* drv) { return ((Driver*)drv)->irq_count; }
const char* driver_name(void* drv) { return ((Driver*)drv)->name.c_str(); }
const char* driver_type(void* drv) { return ((Driver*)drv)->type.c_str(); }
int  driver_is_ready(void* drv) { return ((Driver*)drv)->ready ? 1 : 0; }

// ============================================================
// VGA Console
// ============================================================
struct VGA {
    int cols, rows;
    std::vector<std::vector<char>> buf;
    std::vector<std::vector<int>> colors;
    int cur_row = 0, cur_col = 0;
    VGA(int c, int r) : cols(c), rows(r) {
        buf.assign(r, std::vector<char>(c, ' '));
        colors.assign(r, std::vector<int>(c, 7));
    }
};

void* os_vga_create(int cols, int rows) { return new VGA(cols, rows); }
void  os_vga_destroy(void* vga) { delete (VGA*)vga; }
void  os_vga_putchar(void* vga, char c, int color) {
    auto* v = (VGA*)vga;
    if (c == '\n') { v->cur_row++; v->cur_col = 0; return; }
    if (v->cur_row >= v->rows) { v->cur_row = v->rows - 1; }
    if (v->cur_col >= v->cols) { v->cur_col = 0; v->cur_row++; }
    if (v->cur_row >= v->rows) v->cur_row = v->rows - 1;
    v->buf[v->cur_row][v->cur_col] = c;
    v->colors[v->cur_row][v->cur_col] = color;
    v->cur_col++;
}
void os_vga_puts(void* vga, const char* s, int color) {
    for (int i = 0; s[i]; i++) os_vga_putchar(vga, s[i], color);
}
void os_vga_set_cursor(void* vga, int row, int col) {
    auto* v = (VGA*)vga;
    v->cur_row = row; v->cur_col = col;
}
void os_vga_clear(void* vga) {
    auto* v = (VGA*)vga;
    for (auto& row : v->buf) std::fill(row.begin(), row.end(), ' ');
    v->cur_row = 0; v->cur_col = 0;
}
int os_vga_cursor_row(void* vga) { return ((VGA*)vga)->cur_row; }
int os_vga_cursor_col(void* vga) { return ((VGA*)vga)->cur_col; }
const char* os_vga_get_line(void* vga, int row) {
    static std::string s;
    auto* v = (VGA*)vga;
    if (row < 0 || row >= v->rows) return "";
    s = std::string(v->buf[row].begin(), v->buf[row].end());
    // trim trailing spaces
    size_t end = s.find_last_not_of(' ');
    s = (end == std::string::npos) ? "" : s.substr(0, end+1);
    return s.c_str();
}
int os_vga_cols(void* vga) { return ((VGA*)vga)->cols; }
int os_vga_rows(void* vga) { return ((VGA*)vga)->rows; }

} // extern "C"
