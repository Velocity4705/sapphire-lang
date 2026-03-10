# Sapphire System Programming - Quick Reference

**Fast reference for OS and embedded development in Sapphire**

---

## Import System Library

```sapphire
import system
```

---

## System Functions

### Process
```sapphire
let pid = sapphire_system_get_pid()
sapphire_system_exit(0)
```

### Memory
```sapphire
let ptr = sapphire_system_allocate(1024)
sapphire_system_free(ptr)
let page_size = sapphire_system_page_size()
```

### File System
```sapphire
let exists = sapphire_system_file_exists("/path/to/file")
let dir_exists = sapphire_system_directory_exists("/path/to/dir")
sapphire_system_create_directory("/path/to/dir")
let cwd = sapphire_system_get_cwd()
```

### Environment
```sapphire
let home = sapphire_system_get_env("HOME")
let user = sapphire_system_get_username()
let host = sapphire_system_get_hostname()
```

### System Info
```sapphire
let cpu_count = sapphire_system_cpu_count()
let cpu_arch = sapphire_system_cpu_arch()
```

### Timing
```sapphire
let ms = sapphire_system_timestamp_ms()
let us = sapphire_system_timestamp_us()
let ns = sapphire_system_timestamp_ns()
sapphire_system_sleep_ms(100)
```

---

## Common Patterns

### Measure Execution Time
```sapphire
let start = sapphire_system_timestamp_ns()
// ... your code ...
let end = sapphire_system_timestamp_ns()
print("Time:", end - start, "ns")
```

### Real-Time Control Loop
```sapphire
let cycle_count = 0
while cycle_count < 100:
    let cycle_start = sapphire_system_timestamp_ns()
    
    // ... control logic ...
    
    let cycle_end = sapphire_system_timestamp_ns()
    let cycle_time = (cycle_end - cycle_start) / 1000
    
    if cycle_time > 1000:
        print("[WARN] Deadline miss:", cycle_time, "us")
    
    sapphire_system_sleep_ms(100)
    cycle_count = cycle_count + 1
```

### Data Buffering
```sapphire
let BUFFER_SIZE = 10
let buffer_index = 0
let data_sum = 0

while running:
    let sensor_value = read_sensor()
    data_sum = data_sum + sensor_value
    buffer_index = buffer_index + 1
    
    if buffer_index >= BUFFER_SIZE:
        let average = data_sum / BUFFER_SIZE
        transmit_data(average)
        buffer_index = 0
        data_sum = 0
```

### State Machine
```sapphire
let STATE_IDLE = 0
let STATE_ACTIVE = 1
let STATE_ERROR = 2

let current_state = STATE_IDLE

while running:
    if current_state == STATE_IDLE:
        // ... idle logic ...
        if condition:
            current_state = STATE_ACTIVE
    
    if current_state == STATE_ACTIVE:
        // ... active logic ...
        if error:
            current_state = STATE_ERROR
    
    if current_state == STATE_ERROR:
        // ... error handling ...
        current_state = STATE_IDLE
```

---

## Compilation

### Quick Compile
```bash
./sapp compile program.spp 2>/dev/null | grep -A 1000 "^; ModuleID" > program.ll
llc program.ll -filetype=obj -o program.o
gcc program.o stdlib/system/system.cpp -o program -lstdc++ -lpthread
./program
```

### One-Liner
```bash
./sapp compile program.spp 2>/dev/null | grep -A 1000 "^; ModuleID" > program.ll && llc program.ll -filetype=obj -o program.o && gcc program.o stdlib/system/system.cpp -o program -lstdc++ -lpthread && ./program
```

---

## Run Demos

### OS Demos
```bash
./run_kernel_demo.sh
```

### Embedded Demos
```bash
./run_embedded_demos.sh
```

---

## Example Programs

### Minimal System Program
```sapphire
import system

print("Hello from Sapphire!")
let pid = sapphire_system_get_pid()
print("PID:", pid)
```

### Temperature Controller
```sapphire
import system

let TARGET_TEMP = 22
let current_temp = 20
let heater_on = 0

while cycle_count < 30:
    if current_temp < TARGET_TEMP:
        heater_on = 1
        current_temp = current_temp + 1
    
    if current_temp >= TARGET_TEMP:
        heater_on = 0
    
    print("Temp:", current_temp, "Heater:", heater_on)
    sapphire_system_sleep_ms(100)
    cycle_count = cycle_count + 1
```

### IoT Sensor Node
```sapphire
import system

let BUFFER_SIZE = 5
let buffer_index = 0
let temp_sum = 0

print("[BOOT] IoT Sensor Node")

let temp_buffer = sapphire_system_allocate(BUFFER_SIZE * 8)

while cycle_count < 25:
    let temp = 25 + cycle_count % 5
    temp_sum = temp_sum + temp
    buffer_index = buffer_index + 1
    
    if buffer_index >= BUFFER_SIZE:
        let avg = temp_sum / BUFFER_SIZE
        print("[TX] Temperature:", avg, "C")
        buffer_index = 0
        temp_sum = 0
    
    sapphire_system_sleep_ms(200)
    cycle_count = cycle_count + 1

sapphire_system_free(temp_buffer)
```

---

## Documentation

### Quick Start
- `SYSTEM_QUICK_START.md` - System programming quick start
- `EMBEDDED_DEVELOPMENT.md` - Embedded development guide

### Complete Guides
- `SYSTEM_LIBRARY_COMPLETE.md` - Complete system library docs
- `EMBEDDED_COMPLETE.md` - Complete embedded summary

### Examples
- `system_demo.spp` - System capabilities
- `kernel_hello.spp` - Kernel initialization
- `process_monitor.spp` - Process monitoring
- `embedded_demo.spp` - Temperature controller
- `iot_sensor_node.spp` - IoT sensor node

---

## Performance Tips

### 1. Use Nanosecond Timing
```sapphire
let start = sapphire_system_timestamp_ns()  // Most precise
```

### 2. Minimize Allocations
```sapphire
// Allocate once, reuse
let buffer = sapphire_system_allocate(1024)
// ... use buffer many times ...
sapphire_system_free(buffer)
```

### 3. Check Cycle Time
```sapphire
let cycle_time_us = (cycle_end - cycle_start) / 1000
if cycle_time_us > 1000:
    print("[WARN] Slow cycle:", cycle_time_us, "us")
```

### 4. Use Fixed Sample Rates
```sapphire
sapphire_system_sleep_ms(100)  // Consistent timing
```

---

## Troubleshooting

### "Unknown function" error
→ Add `import system` at top of file

### Compilation fails
→ Check compilation workflow (3 steps)

### Linking errors
→ Include `-lstdc++ -lpthread`

### Slow performance
→ Check cycle time, optimize loop

---

## Need Help?

- Check examples in repository
- Read documentation files
- Look at `stdlib/system/system.h`

---

**Happy system programming!** 🚀
