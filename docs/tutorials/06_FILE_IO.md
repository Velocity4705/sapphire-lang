# Tutorial 6: File I/O

Learn how to read from and write to files in Sapphire.

## Reading Files

### Read Entire File

```sapphire
import File from "stdlib/io"

fn main() {
    // Read entire file as string
    let content = File.read("example.txt")
    println(content)
}
```

### Read File Line by Line

```sapphire
import File from "stdlib/io"

fn main() {
    let lines = File.read_lines("example.txt")
    
    for line in lines {
        println(line)
    }
}
```

### Read with Error Handling

```sapphire
import File from "stdlib/io"

fn main() {
    try {
        let content = File.read("example.txt")
        println(content)
    } catch FileNotFoundError {
        println("File not found!")
    } catch IOError {
        println("Error reading file!")
    }
}
```

## Writing Files

### Write String to File

```sapphire
import File from "stdlib/io"

fn main() {
    let content = "Hello, World!\nThis is a test file."
    
    File.write("output.txt", content)
    println("File written successfully!")
}
```

### Append to File

```sapphire
import File from "stdlib/io"

fn main() {
    let new_content = "\nThis line is appended."
    
    File.append("output.txt", new_content)
    println("Content appended!")
}
```

### Write Lines

```sapphire
import File from "stdlib/io"

fn main() {
    let lines = [
        "Line 1",
        "Line 2",
        "Line 3"
    ]
    
    File.write_lines("output.txt", lines)
    println("Lines written!")
}
```

## File Operations

### Check if File Exists

```sapphire
import File from "stdlib/io"

fn main() {
    if File.exists("example.txt") {
        println("File exists!")
    } else {
        println("File not found!")
    }
}
```

### Delete File

```sapphire
import File from "stdlib/io"

fn main() {
    if File.exists("temp.txt") {
        File.delete("temp.txt")
        println("File deleted!")
    }
}
```

### Copy File

```sapphire
import File from "stdlib/io"

fn main() {
    File.copy("source.txt", "destination.txt")
    println("File copied!")
}
```

### Move/Rename File

```sapphire
import File from "stdlib/io"

fn main() {
    File.move("old_name.txt", "new_name.txt")
    println("File renamed!")
}
```

### Get File Size

```sapphire
import File from "stdlib/io"

fn main() {
    let size = File.size("example.txt")
    println("File size: " + str(size) + " bytes")
}
```

## Working with Paths

### Join Paths

```sapphire
import Path from "stdlib/io"

fn main() {
    let path = Path.join("folder", "subfolder", "file.txt")
    println(path)  // folder/subfolder/file.txt
}
```

### Get File Name

```sapphire
import Path from "stdlib/io"

fn main() {
    let path = "/home/user/documents/file.txt"
    
    let filename = Path.basename(path)
    println(filename)  // file.txt
    
    let dirname = Path.dirname(path)
    println(dirname)  // /home/user/documents
    
    let extension = Path.extension(path)
    println(extension)  // .txt
}
```

### Absolute Path

```sapphire
import Path from "stdlib/io"

fn main() {
    let relative = "documents/file.txt"
    let absolute = Path.absolute(relative)
    println(absolute)  // /home/user/documents/file.txt
}
```

## Directory Operations

### List Directory

```sapphire
import Dir from "stdlib/io"

fn main() {
    let files = Dir.list(".")
    
    for file in files {
        println(file)
    }
}
```

### Create Directory

```sapphire
import Dir from "stdlib/io"

fn main() {
    Dir.create("new_folder")
    println("Directory created!")
    
    // Create nested directories
    Dir.create_all("parent/child/grandchild")
    println("Nested directories created!")
}
```

### Remove Directory

```sapphire
import Dir from "stdlib/io"

fn main() {
    // Remove empty directory
    Dir.remove("empty_folder")
    
    // Remove directory and all contents
    Dir.remove_all("folder_with_files")
}
```

### Check if Directory Exists

```sapphire
import Dir from "stdlib/io"

fn main() {
    if Dir.exists("my_folder") {
        println("Directory exists!")
    } else {
        println("Directory not found!")
    }
}
```

## Binary Files

### Read Binary File

```sapphire
import File from "stdlib/io"

fn main() {
    let bytes = File.read_bytes("image.png")
    println("Read " + str(len(bytes)) + " bytes")
}
```

### Write Binary File

```sapphire
import File from "stdlib/io"

fn main() {
    let bytes = [0xFF, 0xD8, 0xFF, 0xE0]  // JPEG header
    File.write_bytes("output.jpg", bytes)
    println("Binary file written!")
}
```

## CSV Files

### Read CSV

```sapphire
import File from "stdlib/io"

fn main() {
    let lines = File.read_lines("data.csv")
    
    for line in lines {
        let fields = line.split(",")
        println(fields)
    }
}
```

### Write CSV

```sapphire
import File from "stdlib/io"

fn main() {
    let data = [
        ["Name", "Age", "City"],
        ["Alice", "25", "New York"],
        ["Bob", "30", "London"],
        ["Charlie", "35", "Paris"]
    ]
    
    let mut lines = []
    for row in data {
        lines.append(row.join(","))
    }
    
    File.write_lines("output.csv", lines)
    println("CSV file written!")
}
```

## JSON Files

### Read JSON

```sapphire
import File from "stdlib/io"
import JSON from "stdlib/json"

fn main() {
    let content = File.read("data.json")
    let data = JSON.parse(content)
    
    println(data["name"])
    println(data["age"])
}
```

### Write JSON

```sapphire
import File from "stdlib/io"
import JSON from "stdlib/json"

fn main() {
    let data = {
        "name": "Alice",
        "age": 25,
        "city": "New York"
    }
    
    let json_string = JSON.stringify(data)
    File.write("output.json", json_string)
    println("JSON file written!")
}
```

## Practice Exercises

### Exercise 1: Line Counter
Count lines in a file:

```sapphire
import File from "stdlib/io"

fn count_lines(filename: str) -> int {
    let lines = File.read_lines(filename)
    return len(lines)
}

fn main() {
    let count = count_lines("example.txt")
    println("Number of lines: " + str(count))
}
```

### Exercise 2: Word Frequency
Count word frequency in a file:

```sapphire
import File from "stdlib/io"

fn word_frequency(filename: str) -> {str: int} {
    let content = File.read(filename)
    let words = content.to_lower().split(" ")
    let mut counts = {}
    
    for word in words {
        let cleaned = word.trim()
        if len(cleaned) > 0 {
            if counts.has_key(cleaned) {
                counts[cleaned] = counts[cleaned] + 1
            } else {
                counts[cleaned] = 1
            }
        }
    }
    
    return counts
}

fn main() {
    let freq = word_frequency("example.txt")
    
    for word in freq.keys() {
        println(word + ": " + str(freq[word]))
    }
}
```

### Exercise 3: File Backup
Create a backup of a file:

```sapphire
import File from "stdlib/io"
import DateTime from "stdlib/datetime"

fn backup_file(filename: str) {
    if not File.exists(filename) {
        println("File not found!")
        return
    }
    
    let timestamp = DateTime.now().format("%Y%m%d_%H%M%S")
    let backup_name = filename + ".backup_" + timestamp
    
    File.copy(filename, backup_name)
    println("Backup created: " + backup_name)
}

fn main() {
    backup_file("important.txt")
}
```

### Exercise 4: Log File Analyzer
Analyze log files:

```sapphire
import File from "stdlib/io"

fn analyze_logs(filename: str) {
    let lines = File.read_lines(filename)
    let mut error_count = 0
    let mut warning_count = 0
    let mut info_count = 0
    
    for line in lines {
        if line.contains("ERROR") {
            error_count = error_count + 1
        } else if line.contains("WARNING") {
            warning_count = warning_count + 1
        } else if line.contains("INFO") {
            info_count = info_count + 1
        }
    }
    
    println("Errors: " + str(error_count))
    println("Warnings: " + str(warning_count))
    println("Info: " + str(info_count))
}

fn main() {
    analyze_logs("application.log")
}
```

### Exercise 5: File Merger
Merge multiple files:

```sapphire
import File from "stdlib/io"

fn merge_files(input_files: [str], output_file: str) {
    let mut content = ""
    
    for filename in input_files {
        if File.exists(filename) {
            content = content + File.read(filename) + "\n"
        } else {
            println("Warning: " + filename + " not found")
        }
    }
    
    File.write(output_file, content)
    println("Files merged into " + output_file)
}

fn main() {
    let files = ["file1.txt", "file2.txt", "file3.txt"]
    merge_files(files, "merged.txt")
}
```

### Exercise 6: Configuration File Reader
Read configuration from a file:

```sapphire
import File from "stdlib/io"

fn read_config(filename: str) -> {str: str} {
    let lines = File.read_lines(filename)
    let mut config = {}
    
    for line in lines {
        let trimmed = line.trim()
        
        // Skip comments and empty lines
        if len(trimmed) == 0 or trimmed.starts_with("#") {
            continue
        }
        
        // Parse key=value
        if trimmed.contains("=") {
            let parts = trimmed.split("=")
            let key = parts[0].trim()
            let value = parts[1].trim()
            config[key] = value
        }
    }
    
    return config
}

fn main() {
    let config = read_config("config.txt")
    
    for key in config.keys() {
        println(key + " = " + config[key])
    }
}
```

## Common Mistakes

### 1. Not Handling File Not Found
```sapphire
// Wrong:
let content = File.read("missing.txt")  // May crash!

// Correct:
try {
    let content = File.read("missing.txt")
    println(content)
} catch FileNotFoundError {
    println("File not found!")
}
```

### 2. Forgetting to Close Files
```sapphire
// Wrong (in languages that require manual closing):
let file = File.open("example.txt")
let content = file.read()
// Forgot to close!

// Correct (Sapphire handles this automatically):
let content = File.read("example.txt")
// File is automatically closed
```

### 3. Wrong Path Separators
```sapphire
// Wrong (hardcoded separators):
let path = "folder\\subfolder\\file.txt"  // Windows only

// Correct (use Path.join):
let path = Path.join("folder", "subfolder", "file.txt")
// Works on all platforms
```

## Summary

In this tutorial, you learned:
- ✅ Reading and writing files
- ✅ File operations (copy, move, delete)
- ✅ Working with paths
- ✅ Directory operations
- ✅ Binary and CSV files
- ✅ JSON files

## Next Steps

Continue to [Tutorial 7: Error Handling](07_ERROR_HANDLING.md) to learn about:
- Try-catch blocks
- Exception types
- Error propagation
- Best practices

Happy coding! 🚀
