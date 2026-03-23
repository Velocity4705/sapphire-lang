#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// M14: OS Development — Bootloader, Kernel, Memory, Process
// ============================================================

// ----- Bootloader simulation -----
void*  boot_create(const char* disk_image);
void   boot_destroy(void* b);
int    boot_load_mbr(void* b);
int    boot_load_stage2(void* b, uint32_t lba, uint32_t sectors);
int    boot_load_kernel(void* b, uint32_t lba, uint32_t size);
const char* boot_status(void* b);
int    boot_get_memory_map_count(void* b);
const char* boot_get_memory_map_entry(void* b, int idx);  // "base,len,type"
void   boot_enter_protected_mode(void* b);
void   boot_enter_long_mode(void* b);
const char* boot_mode(void* b);  // "real", "protected", "long"

// ----- Kernel -----
void*  os_kernel_create(const char* name);
void   os_kernel_destroy(void* k);
void   os_kernel_init_idt(void* k);
void   os_kernel_init_gdt(void* k);
void   os_kernel_init_pic(void* k);
void   os_kernel_init_pit(void* k, uint32_t hz);
void   os_kernel_register_isr(void* k, int vector, const char* name);
int    os_kernel_isr_count(void* k);
const char* os_kernel_isr_name(void* k, int vector);
void   os_kernel_trigger_interrupt(void* k, int vector);
int    os_kernel_interrupt_count(void* k, int vector);
void   os_kernel_vga_write(void* k, int row, int col, const char* text, int color);
const char* os_kernel_vga_read(void* k, int row, int col);
void   os_kernel_vga_clear(void* k);
const char* os_kernel_status(void* k);

// ----- Physical Memory Manager -----
void*  pmm_create(uint64_t total_bytes, uint32_t page_size);
void   pmm_destroy(void* pmm);
void   pmm_mark_free(void* pmm, uint64_t base, uint64_t length);
void   pmm_mark_used(void* pmm, uint64_t base, uint64_t length);
uint64_t pmm_alloc_page(void* pmm);
void   pmm_free_page(void* pmm, uint64_t addr);
uint64_t pmm_alloc_pages(void* pmm, int count);
int    pmm_free_pages_count(void* pmm);
int    pmm_used_pages_count(void* pmm);
int    pmm_total_pages(void* pmm);
uint32_t pmm_page_size(void* pmm);

// ----- Virtual Memory / Paging -----
void*  vmm_create(void* pmm);
void   vmm_destroy(void* vmm);
int    vmm_map(void* vmm, uint64_t virt, uint64_t phys, int flags);  // flags: 1=present,2=write,4=user
int    vmm_unmap(void* vmm, uint64_t virt);
uint64_t vmm_translate(void* vmm, uint64_t virt);  // returns phys or 0 if not mapped
int    vmm_is_mapped(void* vmm, uint64_t virt);
int    vmm_page_fault_count(void* vmm);
const char* vmm_dump_table(void* vmm);  // JSON summary

// ----- Heap Allocator -----
void*  heap_create(uint64_t base, uint64_t size);
void   heap_destroy(void* heap);
uint64_t heap_alloc(void* heap, uint64_t size);
void   heap_free(void* heap, uint64_t addr);
uint64_t heap_used(void* heap);
uint64_t heap_free_space(void* heap);
int    heap_block_count(void* heap);

// ----- Process / Scheduler -----
void*  scheduler_create(const char* policy);  // "round_robin", "priority", "fifo"
void   scheduler_destroy(void* sched);
int    scheduler_add_process(void* sched, const char* name, int priority, uint64_t stack_size);
void   scheduler_terminate(void* sched, int pid);
int    scheduler_tick(void* sched);   // returns current pid
int    scheduler_current_pid(void* sched);
const char* scheduler_current_name(void* sched);
int    scheduler_process_count(void* sched);
const char* scheduler_process_name(void* sched, int pid);
const char* scheduler_process_state(void* sched, int pid);  // "running","ready","blocked","terminated"
int    scheduler_process_priority(void* sched, int pid);
uint64_t scheduler_process_ticks(void* sched, int pid);
void   scheduler_block(void* sched, int pid);
void   scheduler_unblock(void* sched, int pid);

// ----- System Calls -----
void*  syscall_table_create(void);
void   syscall_table_destroy(void* tbl);
void   syscall_table_register(void* tbl, int num, const char* name, int arg_count);
const char* syscall_table_invoke(void* tbl, int num, const char* args);  // returns result string
int    syscall_table_count(void* tbl);
const char* syscall_table_name(void* tbl, int num);

// ----- Device Drivers -----
void*  driver_create(const char* name, const char* type);  // type: "char","block","net"
void   driver_destroy(void* drv);
void   driver_write(void* drv, uint64_t addr, uint64_t value, int width);  // width: 1,2,4,8
uint64_t driver_read(void* drv, uint64_t addr, int width);
void   driver_irq_fire(void* drv);
int    driver_irq_count(void* drv);
const char* driver_name(void* drv);
const char* driver_type(void* drv);
int    driver_is_ready(void* drv);

// ----- VGA / Console -----
void*  os_vga_create(int cols, int rows);
void   os_vga_destroy(void* vga);
void   os_vga_putchar(void* vga, char c, int color);
void   os_vga_puts(void* vga, const char* s, int color);
void   os_vga_set_cursor(void* vga, int row, int col);
void   os_vga_clear(void* vga);
int    os_vga_cursor_row(void* vga);
int    os_vga_cursor_col(void* vga);
const char* os_vga_get_line(void* vga, int row);
int    os_vga_cols(void* vga);
int    os_vga_rows(void* vga);

#ifdef __cplusplus
}
#endif
