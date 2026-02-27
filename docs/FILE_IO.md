# File I/O in Sapphire

Complete guide to reading and writing files in Sapphire.

## Reading Files

### Read Entire File

```sapphire
# Read entire file as string
let content = File.read("data.txt")
print(content)

# Read as lines
let lines = File.read_lines("data.txt")
for line in lines:
    print(line)

# Read as bytes
let bytes = File.read_bytes("image.png")
print(f"File size: {bytes.len()} bytes")
```

### Read Line by Line

```sapphire
# Efficient for large files
let file = File.open("large_file.txt", "r")
while let line = file.read_line():
    print(line)
file.close()

# With automatic cleanup
with File.open("data.txt", "r") as file:
    for line in file:
        print(line)
```

### Read with Error Handling

```sapphire
try:
    let content = File.read("data.txt")
    print(content)
catch FileNotFoundError as e:
    print(f"File not found: {e}")
catch PermissionError as e:
    print(f"Permission denied: {e}")
catch IOError as e:
    print(f"I/O error: {e}")
```

## Writing Files

### Write String to File

```sapphire
# Write (overwrites existing file)
File.write("output.txt", "Hello, Sapphire!")

# Append to file
File.append("log.txt", "New log entry\n")

# Write lines
let lines = ["Line 1", "Line 2", "Line 3"]
File.write_lines("output.txt", lines)
```

### Write with File Handle

```sapphire
let file = File.open("output.txt", "w")
file.write("First line\n")
file.write("Second line\n")
file.flush()  # Force write to disk
file.close()

# With automatic cleanup
with File.open("output.txt", "w") as file:
    file.write("Hello\n")
    file.write("World\n")
```

### Write Binary Data

```sapphire
let data = [0x48, 0x65, 0x6C, 0x6C, 0x6F]  # "Hello"
File.write_bytes("output.bin", data)
```

## File Operations

### Check File Existence

```sapphire
if File.exists("data.txt"):
    print("File exists")
else:
    print("File not found")
```

### File Information

```sapphire
let info = File.info("data.txt")
print(f"Size: {info.size} bytes")
print(f"Created: {info.created}")
print(f"Modified: {info.modified}")
print(f"Is directory: {info.is_dir}")
print(f"Is file: {info.is_file}")
print(f"Permissions: {info.permissions}")
```

### File Operations

```sapphire
# Copy file
File.copy("source.txt", "destination.txt")

# Move/rename file
File.move("old_name.txt", "new_name.txt")

# Delete file
File.delete("temp.txt")

# Create directory
File.create_dir("my_folder")

# Create nested directories
File.create_dir_all("path/to/nested/folder")

# List directory contents
let files = File.list_dir(".")
for file in files:
    print(file)
```

## File Modes

```sapphire
# Read mode (default)
let file = File.open("data.txt", "r")

# Write mode (overwrites)
let file = File.open("output.txt", "w")

# Append mode
let file = File.open("log.txt", "a")

# Read and write
let file = File.open("data.txt", "r+")

# Binary modes
let file = File.open("image.png", "rb")  # Read binary
let file = File.open("output.bin", "wb")  # Write binary
```

## Path Operations

```sapphire
import path

# Join paths
let full_path = path.join("folder", "subfolder", "file.txt")
# Result: "folder/subfolder/file.txt"

# Get absolute path
let abs_path = path.absolute("relative/path.txt")

# Get directory name
let dir = path.dirname("/path/to/file.txt")
# Result: "/path/to"

# Get file name
let name = path.basename("/path/to/file.txt")
# Result: "file.txt"

# Get file extension
let ext = path.extension("file.txt")
# Result: ".txt"

# Check if path is absolute
if path.is_absolute("/usr/bin"):
    print("Absolute path")
```

## Working with CSV

```sapphire
import csv

# Read CSV
let data = csv.read("data.csv")
for row in data:
    print(row)

# Read CSV with headers
let data = csv.read("data.csv", headers: true)
for row in data:
    print(f"Name: {row['name']}, Age: {row['age']}")

# Write CSV
let data = [
    ["Name", "Age", "City"],
    ["Alice", "30", "NYC"],
    ["Bob", "25", "LA"]
]
csv.write("output.csv", data)
```

## Working with JSON

```sapphire
import json

# Read JSON file
let data = json.read("config.json")
print(data["setting"])

# Write JSON file
let config = {
    "host": "localhost",
    "port": 8080,
    "debug": true
}
json.write("config.json", config, indent: 2)

# Parse JSON string
let json_str = '{"name": "Alice", "age": 30}'
let data = json.parse(json_str)

# Convert to JSON string
let obj = {"key": "value"}
let json_str = json.stringify(obj)
```

## Temporary Files

```sapphire
import tempfile

# Create temporary file
let temp = tempfile.create()
temp.write("Temporary data")
print(f"Temp file: {temp.path}")
temp.close()  # Automatically deleted

# Create temporary directory
let temp_dir = tempfile.create_dir()
print(f"Temp dir: {temp_dir.path}")
temp_dir.cleanup()  # Delete directory and contents
```

## Memory-Mapped Files

```sapphire
# For large files - map to memory
let mmap = File.mmap("large_file.dat")
let data = mmap[1000:2000]  # Read bytes 1000-2000
mmap[500] = 0xFF  # Write byte at position 500
mmap.flush()
mmap.close()
```

## File Locking

```sapphire
# Exclusive lock (write)
let file = File.open("data.txt", "r+")
file.lock_exclusive()
file.write("Protected data")
file.unlock()
file.close()

# Shared lock (read)
let file = File.open("data.txt", "r")
file.lock_shared()
let content = file.read()
file.unlock()
file.close()
```

## Buffered I/O

```sapphire
# Buffered reading (faster for many small reads)
let file = File.open("data.txt", "r", buffer_size: 8192)
let content = file.read()
file.close()

# Unbuffered (immediate writes)
let file = File.open("log.txt", "w", buffered: false)
file.write("Immediate write")
file.close()
```

## Examples

### Copy File with Progress

```sapphire
fn copy_with_progress(source: str, dest: str):
    let src = File.open(source, "rb")
    let dst = File.open(dest, "wb")
    let size = File.info(source).size
    let copied = 0
    
    while let chunk = src.read(4096):
        dst.write(chunk)
        copied += chunk.len()
        let progress = (copied / size) * 100
        print(f"Progress: {progress:.1f}%")
    
    src.close()
    dst.close()
```

### Read Configuration File

```sapphire
fn read_config(path: str) -> HashMap<str, str>:
    let config = HashMap::new()
    let lines = File.read_lines(path)
    
    for line in lines:
        if line.starts_with("#") or line.is_empty():
            continue
        
        let parts = line.split("=")
        if parts.len() == 2:
            config[parts[0].trim()] = parts[1].trim()
    
    return config
```

### Log to File

```sapphire
fn log(message: str):
    let timestamp = time.now().format("%Y-%m-%d %H:%M:%S")
    let entry = f"[{timestamp}] {message}\n"
    File.append("app.log", entry)
```

## Best Practices

1. **Always close files** or use `with` statement
2. **Handle errors** with try/catch
3. **Use buffered I/O** for better performance
4. **Check file existence** before reading
5. **Use binary mode** for non-text files
6. **Lock files** when multiple processes access them
7. **Use temporary files** for intermediate data

## Example Code

See [examples/file_io_example.spp](../examples/file_io_example.spp) for a complete working example.

## See Also

- [Standard Library](STANDARD_LIBRARY.md)
- [Error Handling](ERROR_HANDLING.md)
- [System Programming](SYSTEM_PROGRAMMING.md)
