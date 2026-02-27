# System Programming in Sapphire

Complete guide to low-level system programming, system calls, and OS interaction.

## Process Management

### Creating Processes

```sapphire
import process

# Spawn a new process
let proc = process.spawn("ls", ["-la"])
let exit_code = proc.wait()
print(f"Exit code: {exit_code}")

# Capture output
let proc = process.spawn("echo", ["Hello"], capture_output: true)
let output = proc.stdout.read()
print(output)

# Run command and get output
let output = process.run("ls -la", shell: true)
print(output)
```

### Process Information

```sapphire
import process

// Current process
let pid = process.getpid()
let ppid = process.getppid()
print(f"PID: {pid}, Parent PID: {ppid}")

// Process details
let info = process.info(pid)
print(f"Name: {info.name}")
print(f"CPU: {info.cpu_percent}%")
print(f"Memory: {info.memory_mb} MB")
print(f"Threads: {info.num_threads}")
```

### Process Control

```sapphire
// Fork process (Unix)
let pid = process.fork()
if pid == 0:
    // Child process
    print("I'm the child")
    process.exit(0)
else:
    // Parent process
    print(f"Child PID: {pid}")
    process.wait(pid)

// Execute new program
process.exec("/bin/ls", ["-la"])
// This replaces current process
```

### Signals

```sapphire
import signal

// Handle signals
signal.handle(signal.SIGINT, |sig| {
    print("Caught Ctrl+C")
    cleanup()
    process.exit(0)
})

// Send signal to process
signal.send(pid, signal.SIGTERM)

// Ignore signal
signal.ignore(signal.SIGPIPE)

// Reset to default
signal.default(signal.SIGINT)
```

## File System Operations

### File Metadata

```sapphire
import fs

// Get file stats
let stat = fs.stat("file.txt")
print(f"Size: {stat.size}")
print(f"Mode: {stat.mode:o}")
print(f"UID: {stat.uid}")
print(f"GID: {stat.gid}")
print(f"Inode: {stat.inode}")
print(f"Links: {stat.nlink}")
print(f"Access time: {stat.atime}")
print(f"Modify time: {stat.mtime}")
print(f"Change time: {stat.ctime}")
```

### Permissions

```sapphire
// Change file permissions
fs.chmod("file.txt", 0o644)  // rw-r--r--

// Change owner
fs.chown("file.txt", uid: 1000, gid: 1000)

// Check permissions
if fs.access("file.txt", fs.R_OK):
    print("Readable")
if fs.access("file.txt", fs.W_OK):
    print("Writable")
if fs.access("file.txt", fs.X_OK):
    print("Executable")
```

### Links

```sapphire
// Create hard link
fs.link("original.txt", "hardlink.txt")

// Create symbolic link
fs.symlink("original.txt", "symlink.txt")

// Read symbolic link
let target = fs.readlink("symlink.txt")

// Check if symlink
if fs.is_symlink("path"):
    print("It's a symlink")
```

### Directory Operations

```sapphire
// Create directory
fs.mkdir("mydir")
fs.mkdir("path/to/dir", recursive: true)

// Remove directory
fs.rmdir("mydir")
fs.rmdir("path/to/dir", recursive: true)

// List directory
let entries = fs.listdir(".")
for entry in entries:
    print(entry)

// Walk directory tree
for root, dirs, files in fs.walk("/path"):
    print(f"Directory: {root}")
    for file in files:
        print(f"  File: {file}")
```

## Memory Management

### Memory Allocation

```sapphire
import mem

// Allocate memory
let ptr = mem.alloc(1024)  // 1KB
mem.write(ptr, data)
let data = mem.read(ptr, 1024)
mem.free(ptr)

// Aligned allocation
let ptr = mem.alloc_aligned(1024, alignment: 16)

// Zero-initialized allocation
let ptr = mem.calloc(100, size: 8)  // 100 * 8 bytes
```

### Memory Mapping

```sapphire
// Memory-map a file
let mmap = mem.mmap(
    file: "data.bin",
    size: 1024 * 1024,  // 1MB
    prot: mem.PROT_READ | mem.PROT_WRITE,
    flags: mem.MAP_SHARED
)

// Access mapped memory
mmap[0] = 0xFF
let value = mmap[100]

// Sync to disk
mmap.sync()

// Unmap
mmap.unmap()
```

### Memory Information

```sapphire
// Get memory info
let info = mem.info()
print(f"Total: {info.total_mb} MB")
print(f"Available: {info.available_mb} MB")
print(f"Used: {info.used_mb} MB")
print(f"Free: {info.free_mb} MB")
print(f"Percent: {info.percent}%")
```

## System Calls

### Direct System Calls

```sapphire
import syscall

// Make system call
let result = syscall.call(
    syscall.SYS_WRITE,
    1,  // stdout
    "Hello\n".as_ptr(),
    6   // length
)

// Get errno
let err = syscall.errno()
```

### Low-Level I/O

```sapphire
import io

// Open file descriptor
let fd = io.open("file.txt", io.O_RDWR | io.O_CREAT, 0o644)

// Read/write
let buffer = [0u8; 1024]
let n = io.read(fd, buffer)
io.write(fd, "data")

// Seek
io.seek(fd, 100, io.SEEK_SET)

// Close
io.close(fd)

// Duplicate file descriptor
let fd2 = io.dup(fd)
```

### Pipes

```sapphire
// Create pipe
let read_fd, write_fd = io.pipe()

// Fork and communicate
let pid = process.fork()
if pid == 0:
    // Child: write to pipe
    io.close(read_fd)
    io.write(write_fd, "Hello from child")
    io.close(write_fd)
    process.exit(0)
else:
    // Parent: read from pipe
    io.close(write_fd)
    let data = io.read(read_fd, 1024)
    print(data)
    io.close(read_fd)
    process.wait(pid)
```

## Network Programming

### Sockets

```sapphire
import socket

// Create TCP socket
let sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

// Bind and listen
sock.bind("0.0.0.0", 8080)
sock.listen(5)

// Accept connections
while true:
    let client, addr = sock.accept()
    print(f"Connection from {addr}")
    
    let data = client.recv(1024)
    client.send("HTTP/1.1 200 OK\r\n\r\nHello")
    client.close()
```

### UDP Sockets

```sapphire
// Create UDP socket
let sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind("0.0.0.0", 9000)

// Receive datagram
let data, addr = sock.recvfrom(1024)
print(f"Received from {addr}: {data}")

// Send datagram
sock.sendto("Response", addr)
```

### Unix Domain Sockets

```sapphire
// Create Unix socket
let sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock.bind("/tmp/mysocket")
sock.listen(5)

let client, _ = sock.accept()
let data = client.recv(1024)
```

## Hardware Access

### Memory-Mapped I/O

```sapphire
// Map hardware registers
let gpio_base = 0x3F200000
let gpio = mem.mmap_device(gpio_base, 0x1000)

// Read/write registers
gpio[0] = 0x01  // Set GPIO pin
let status = gpio[4]  // Read status
```

### Port I/O (x86)

```sapphire
import port

// Read from I/O port
let value = port.inb(0x60)  // Read byte
let value = port.inw(0x60)  // Read word
let value = port.inl(0x60)  // Read long

// Write to I/O port
port.outb(0x60, 0xFF)  // Write byte
port.outw(0x60, 0xFFFF)  // Write word
port.outl(0x60, 0xFFFFFFFF)  // Write long
```

## Interrupts and Timers

### Timers

```sapphire
import timer

// Create timer
let t = timer.Timer()
t.start()
// ... do work ...
let elapsed = t.elapsed()
print(f"Elapsed: {elapsed}ms")

// Sleep
timer.sleep(1.0)  // Sleep 1 second
timer.usleep(1000)  // Sleep 1000 microseconds
```

### Alarms

```sapphire
import alarm

// Set alarm
alarm.set(5, || {
    print("Alarm triggered!")
})

// Cancel alarm
alarm.cancel()
```

## System Information

### OS Information

```sapphire
import sys

print(f"OS: {sys.platform}")
print(f"Version: {sys.version}")
print(f"Architecture: {sys.arch}")
print(f"Hostname: {sys.hostname}")
print(f"Username: {sys.username}")
```

### CPU Information

```sapphire
import cpu

let info = cpu.info()
print(f"Cores: {info.cores}")
print(f"Threads: {info.threads}")
print(f"Frequency: {info.frequency_mhz} MHz")
print(f"Usage: {cpu.percent()}%")
```

### Disk Information

```sapphire
import disk

let usage = disk.usage("/")
print(f"Total: {usage.total_gb} GB")
print(f"Used: {usage.used_gb} GB")
print(f"Free: {usage.free_gb} GB")
print(f"Percent: {usage.percent}%")
```

## Kernel Modules

### Loading Modules

```sapphire
import kernel

// Load kernel module
kernel.load_module("mymodule")

// Unload module
kernel.unload_module("mymodule")

// List loaded modules
let modules = kernel.list_modules()
for mod in modules:
    print(mod)
```

## Device Drivers

### Character Devices

```sapphire
// Open device
let dev = io.open("/dev/mydevice", io.O_RDWR)

// Read from device
let data = io.read(dev, 1024)

// Write to device
io.write(dev, command)

// ioctl
io.ioctl(dev, IOCTL_CMD, arg)

io.close(dev)
```

## Real-Time Programming

### Priority Scheduling

```sapphire
import sched

// Set real-time priority
sched.set_priority(sched.SCHED_FIFO, priority: 99)

// Get priority
let priority = sched.get_priority()

// Set CPU affinity
sched.set_affinity([0, 1])  // Run on CPUs 0 and 1
```

### Memory Locking

```sapphire
import mem

// Lock memory (prevent swapping)
mem.lock_all()

// Lock specific pages
mem.lock(ptr, size)

// Unlock
mem.unlock_all()
```

## Examples

### Simple Shell

```sapphire
fn shell():
    while true:
        let cmd = input("$ ")
        if cmd == "exit":
            break
        
        let proc = process.spawn(cmd, shell: true)
        proc.wait()
```

### File Monitor

```sapphire
import inotify

fn monitor_file(path: str):
    let watch = inotify.Watch(path)
    
    while true:
        let event = watch.read()
        match event.type:
            inotify.MODIFY => print(f"{path} modified")
            inotify.DELETE => print(f"{path} deleted")
            inotify.CREATE => print(f"{path} created")
```

### Memory Profiler

```sapphire
fn profile_memory():
    let start = mem.info()
    
    // Run code
    allocate_lots_of_memory()
    
    let end = mem.info()
    let used = end.used_mb - start.used_mb
    print(f"Memory used: {used} MB")
```

## Best Practices

1. **Check return values** - System calls can fail
2. **Handle errors** - Use errno to get error details
3. **Clean up resources** - Close file descriptors, free memory
4. **Use RAII** - Automatic resource management
5. **Be careful with permissions** - Don't run as root unnecessarily
6. **Validate input** - Especially for system calls
7. **Use safe wrappers** - Prefer high-level APIs when possible

## Example Code

See [examples/system_programming_example.spp](../examples/system_programming_example.spp) for a complete working example.

## See Also

- [File I/O](FILE_IO.md)
- [Concurrency](CONCURRENCY.md)
- [Memory Management](MEMORY_MANAGEMENT.md)
- [Platform Directives](PLATFORM_DIRECTIVES.md)
