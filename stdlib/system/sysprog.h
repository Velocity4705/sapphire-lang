#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Phase 1: Inline Assembly (simulated x86/ARM assembler)
// ============================================================

void* asm_context_create(const char* arch);   // "x86_64" | "arm64" | "arm32"
void  asm_context_destroy(void* ctx);
const char* asm_context_arch(void* ctx);

// Assemble a mnemonic line -> returns hex bytes string
const char* asm_assemble(void* ctx, const char* mnemonic);
// Disassemble hex bytes -> returns mnemonic string
const char* asm_disassemble(void* ctx, const char* hex_bytes);
// Execute assembled instruction (simulated, updates register state)
void  asm_execute(void* ctx, const char* mnemonic);

// Register access
void  asm_set_reg(void* ctx, const char* reg, int64_t value);
int64_t asm_get_reg(void* ctx, const char* reg);
const char* asm_dump_regs(void* ctx);

// Instruction count / history
int   asm_instruction_count(void* ctx);
const char* asm_instruction_at(void* ctx, int index);

// Optimization hint
const char* asm_optimize(void* ctx, const char* mnemonic);

// ============================================================
// Phase 2: GPIO & Hardware
// ============================================================

// GPIO Controller
void* gpio_controller_create(const char* board);  // "raspberry_pi" | "arduino" | "esp32" | "stm32"
void  gpio_controller_destroy(void* ctrl);
const char* gpio_controller_board(void* ctrl);
int   gpio_controller_pin_count(void* ctrl);

// GPIO Pin
void* gpio_pin_create(void* ctrl, int pin_num, const char* mode); // "input"|"output"|"pwm"|"analog"
void  gpio_pin_destroy(void* pin);
void  gpio_pin_write(void* pin, int value);       // 0 or 1
int   gpio_pin_read(void* pin);
int   gpio_pin_number(void* pin);
const char* gpio_pin_mode(void* pin);
void  gpio_pin_set_pull(void* pin, const char* pull); // "up"|"down"|"none"
const char* gpio_pin_to_string(void* pin);

// PWM
void* pwm_create(void* pin, int frequency, float duty_cycle);
void  pwm_destroy(void* pwm);
void  pwm_set_frequency(void* pwm, int freq);
void  pwm_set_duty_cycle(void* pwm, float duty);
void  pwm_start(void* pwm);
void  pwm_stop(void* pwm);
int   pwm_frequency(void* pwm);
float pwm_duty_cycle(void* pwm);
int   pwm_is_running(void* pwm);

// ADC (Analog-to-Digital Converter)
void* adc_create(void* ctrl, int channel, int resolution_bits);
void  adc_destroy(void* adc);
int   adc_read_raw(void* adc);
float adc_read_voltage(void* adc, float vref);
int   adc_resolution(void* adc);
int   adc_channel(void* adc);

// DAC (Digital-to-Analog Converter)
void* dac_create(void* ctrl, int channel, int resolution_bits);
void  dac_destroy(void* dac);
void  dac_write_raw(void* dac, int value);
void  dac_write_voltage(void* dac, float voltage, float vref);
int   dac_resolution(void* dac);
int   dac_channel(void* dac);

// SPI Bus
void* spi_bus_create(void* ctrl, int bus_num, int clock_hz);
void  spi_bus_destroy(void* spi);
const char* spi_transfer(void* spi, const char* hex_data);
void  spi_set_clock(void* spi, int hz);
int   spi_clock(void* spi);
int   spi_bus_num(void* spi);

// I2C Bus
void* i2c_bus_create(void* ctrl, int bus_num, int clock_hz);
void  i2c_bus_destroy(void* i2c);
int   i2c_write(void* i2c, int addr, const char* hex_data);
const char* i2c_read(void* i2c, int addr, int num_bytes);
int   i2c_scan(void* i2c);   // returns count of devices found
int   i2c_bus_num(void* i2c);

// UART
void* uart_create(void* ctrl, int port, int baud_rate);
void  uart_destroy(void* uart);
int   uart_write(void* uart, const char* data);
const char* uart_read(void* uart, int max_bytes);
void  uart_set_baud(void* uart, int baud);
int   uart_baud(void* uart);
int   uart_port(void* uart);
int   uart_bytes_available(void* uart);

// ============================================================
// Phase 3: RTOS Integration
// ============================================================

// RTOS Kernel
void* rtos_kernel_create(const char* name);
void  rtos_kernel_destroy(void* kernel);
void  rtos_kernel_start(void* kernel);
void  rtos_kernel_stop(void* kernel);
int   rtos_kernel_is_running(void* kernel);
const char* rtos_kernel_name(void* kernel);
int   rtos_kernel_tick(void* kernel);
int   rtos_kernel_task_count(void* kernel);

// Task
void* rtos_task_create(void* kernel, const char* name, int priority, int stack_size);
void  rtos_task_destroy(void* task);
void  rtos_task_start(void* task);
void  rtos_task_suspend(void* task);
void  rtos_task_resume(void* task);
void  rtos_task_delete(void* task);
const char* rtos_task_name(void* task);
int   rtos_task_priority(void* task);
const char* rtos_task_state(void* task);  // "ready"|"running"|"suspended"|"deleted"
int   rtos_task_stack_size(void* task);

// Semaphore
void* rtos_semaphore_create(int initial_count, int max_count);
void  rtos_semaphore_destroy(void* sem);
int   rtos_semaphore_take(void* sem, int timeout_ms);
int   rtos_semaphore_give(void* sem);
int   rtos_semaphore_count(void* sem);

// Mutex
void* rtos_mutex_create();
void  rtos_mutex_destroy(void* mutex);
int   rtos_mutex_lock(void* mutex, int timeout_ms);
int   rtos_mutex_unlock(void* mutex);
int   rtos_mutex_is_locked(void* mutex);

// Message Queue
void* rtos_queue_create(int capacity, int item_size);
void  rtos_queue_destroy(void* queue);
int   rtos_queue_send(void* queue, const char* data, int timeout_ms);
const char* rtos_queue_receive(void* queue, int timeout_ms);
int   rtos_queue_size(void* queue);
int   rtos_queue_capacity(void* queue);
int   rtos_queue_is_full(void* queue);
int   rtos_queue_is_empty(void* queue);

// Timer
void* rtos_timer_create(const char* name, int period_ms, int auto_reload);
void  rtos_timer_destroy(void* timer);
void  rtos_timer_start(void* timer);
void  rtos_timer_stop(void* timer);
void  rtos_timer_reset(void* timer);
int   rtos_timer_is_active(void* timer);
int   rtos_timer_period(void* timer);
int   rtos_timer_fire_count(void* timer);
const char* rtos_timer_name(void* timer);

// ============================================================
// Phase 4: Bootloader & Kernel
// ============================================================

// Memory Map
void* mem_map_create(uint64_t base_addr, uint64_t size);
void  mem_map_destroy(void* map);
void  mem_map_add_region(void* map, const char* name, uint64_t addr, uint64_t size, const char* type);
int   mem_map_region_count(void* map);
const char* mem_map_region_name(void* map, int index);
uint64_t mem_map_region_addr(void* map, int index);
uint64_t mem_map_region_size(void* map, int index);
const char* mem_map_region_type(void* map, int index);
const char* mem_map_to_string(void* map);

// Interrupt Table
void* interrupt_table_create(int num_vectors);
void  interrupt_table_destroy(void* table);
void  interrupt_table_register(void* table, int vector, const char* name, const char* handler);
int   interrupt_table_fire(void* table, int vector);
const char* interrupt_table_handler(void* table, int vector);
const char* interrupt_table_name(void* table, int vector);
int   interrupt_table_fire_count(void* table, int vector);
int   interrupt_table_size(void* table);

// Process Table
void* process_table_create(int max_processes);
void  process_table_destroy(void* table);
int   process_table_create_process(void* table, const char* name, int priority);
void  process_table_kill(void* table, int pid);
const char* process_table_name(void* table, int pid);
int   process_table_priority(void* table, int pid);
const char* process_table_state(void* table, int pid);
int   process_table_count(void* table);
int   process_table_schedule(void* table);  // returns next pid to run
const char* process_table_to_string(void* table);

// Kernel
void* kernel_create(const char* name, const char* arch);
void  kernel_destroy(void* kernel);
void  kernel_boot(void* kernel);
void  kernel_panic(void* kernel, const char* msg);
const char* kernel_name(void* kernel);
const char* kernel_arch(void* kernel);
const char* kernel_state(void* kernel);  // "offline"|"booting"|"running"|"panic"
int   kernel_uptime_ms(void* kernel);
void  kernel_syscall(void* kernel, const char* call, const char* args);
const char* kernel_log(void* kernel);
int   kernel_log_count(void* kernel);

#ifdef __cplusplus
}
#endif
