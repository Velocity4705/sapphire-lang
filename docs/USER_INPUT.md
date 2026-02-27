# User Input in Sapphire

Complete guide to reading user input from console, files, and other sources.

## Console Input

### Basic Input

```sapphire
# Read a line of text
let name = input("Enter your name: ")
print(f"Hello, {name}!")

# Read without prompt
let data = input()
```

### Input with Type Conversion

```sapphire
# Read integer
let age = input("Enter your age: ").to_int()
print(f"You are {age} years old")

# Read float
let price = input("Enter price: ").to_float()
print(f"Price: ${price:.2f}")

# Read boolean
let answer = input("Continue? (yes/no): ").to_lower() == "yes"
```

### Input with Validation

```sapphire
fn get_valid_age() -> int:
    while true:
        try:
            let age = input("Enter age (1-120): ").to_int()
            if age >= 1 and age <= 120:
                return age
            else:
                print("Age must be between 1 and 120")
        catch ValueError:
            print("Please enter a valid number")
```

### Multiple Inputs

```sapphire
# Read multiple values on one line
let input_str = input("Enter three numbers: ")
let numbers = input_str.split().map(|x| x.to_int())

# Unpack values
let a, b, c = input("Enter x, y, z: ").split(",").map(|x| x.to_int())
```

## Password Input

```sapphire
import getpass

# Read password (hidden input)
let password = getpass.getpass("Enter password: ")

# Verify password
let confirm = getpass.getpass("Confirm password: ")
if password == confirm:
    print("Password set successfully")
else:
    print("Passwords don't match")
```

## Command-Line Arguments

```sapphire
import sys

# Get all arguments
let args = sys.args
print(f"Program: {args[0]}")
print(f"Arguments: {args[1:]}")

# Parse arguments
if args.len() < 2:
    print("Usage: program <filename>")
    sys.exit(1)

let filename = args[1]
```

### Argument Parser

```sapphire
import argparse

let parser = argparse.ArgumentParser(
    description: "My Sapphire Program"
)

parser.add_argument("input", help: "Input file")
parser.add_argument("--output", "-o", help: "Output file")
parser.add_argument("--verbose", "-v", action: "store_true")
parser.add_argument("--count", "-c", type: "int", default: 1)

let args = parser.parse_args()

print(f"Input: {args.input}")
print(f"Output: {args.output}")
print(f"Verbose: {args.verbose}")
print(f"Count: {args.count}")
```

## Interactive Menus

```sapphire
fn show_menu() -> int:
    print("\n=== Main Menu ===")
    print("1. New file")
    print("2. Open file")
    print("3. Save file")
    print("4. Exit")
    
    while true:
        let choice = input("Select option (1-4): ")
        if choice in ["1", "2", "3", "4"]:
            return choice.to_int()
        else:
            print("Invalid choice. Try again.")

fn main():
    while true:
        let choice = show_menu()
        
        match choice:
            1 => print("Creating new file...")
            2 => print("Opening file...")
            3 => print("Saving file...")
            4 => {
                print("Goodbye!")
                break
            }
```

## Reading from Standard Input

```sapphire
import sys

# Read all input at once
let content = sys.stdin.read()
print(f"Read {content.len()} characters")

# Read line by line
for line in sys.stdin:
    print(f"Line: {line.trim()}")

# Read with timeout
let line = sys.stdin.read_line(timeout: 5.0)
if line is None:
    print("Timeout!")
```

## Piped Input

```sapphire
import sys

# Check if input is piped
if sys.stdin.is_tty():
    print("Interactive mode")
    let name = input("Enter name: ")
else:
    print("Piped mode")
    let name = sys.stdin.read_line()

# Usage:
# echo "Alice" | ./program
```

## File Input

```sapphire
# Read from file
let content = File.read("input.txt")
let lines = content.split("\n")

for line in lines:
    process(line)

# Read with error handling
try:
    let data = File.read("data.txt")
    process(data)
catch FileNotFoundError:
    print("File not found. Using default values.")
    let data = get_default_data()
```

## Environment Variables

```sapphire
import os

# Get environment variable
let home = os.getenv("HOME")
print(f"Home directory: {home}")

# Get with default value
let port = os.getenv("PORT", default: "8080")

# Set environment variable
os.setenv("MY_VAR", "value")

# Check if variable exists
if os.hasenv("DEBUG"):
    print("Debug mode enabled")
```

## Configuration Files

```sapphire
import config

# Read INI file
let cfg = config.read_ini("config.ini")
let host = cfg["server"]["host"]
let port = cfg["server"]["port"].to_int()

# Read YAML
let cfg = config.read_yaml("config.yaml")

# Read TOML
let cfg = config.read_toml("config.toml")

# Read JSON
let cfg = config.read_json("config.json")
```

## Interactive Prompts

```sapphire
import prompt

# Yes/No prompt
if prompt.confirm("Do you want to continue?"):
    print("Continuing...")
else:
    print("Cancelled")

# Choice prompt
let color = prompt.select(
    "Choose a color:",
    options: ["Red", "Green", "Blue"]
)

# Multi-select
let toppings = prompt.multi_select(
    "Choose toppings:",
    options: ["Cheese", "Pepperoni", "Mushrooms", "Olives"]
)

# Input with validation
let email = prompt.input(
    "Enter email:",
    validator: |x| x.contains("@")
)
```

## Progress Bars

```sapphire
import progress

# Simple progress bar
let bar = progress.Bar(total: 100)
for i in 0..100:
    process_item(i)
    bar.update(i + 1)
bar.finish()

# With custom format
let bar = progress.Bar(
    total: 100,
    format: "{percent}% [{bar}] {current}/{total}"
)
```

## Spinner

```sapphire
import spinner

# Show spinner during long operation
let spin = spinner.Spinner("Processing...")
spin.start()
long_operation()
spin.stop()
print("Done!")
```

## Input Validation

```sapphire
fn validate_email(email: str) -> bool:
    return email.contains("@") and email.contains(".")

fn validate_phone(phone: str) -> bool:
    return phone.len() >= 10 and phone.is_numeric()

fn get_validated_input(prompt: str, validator: fn(str) -> bool) -> str:
    while true:
        let value = input(prompt)
        if validator(value):
            return value
        else:
            print("Invalid input. Try again.")

# Usage
let email = get_validated_input(
    "Enter email: ",
    validate_email
)
```

## Autocomplete

```sapphire
import readline

# Enable autocomplete
readline.set_completer(["apple", "banana", "cherry"])

let fruit = input("Enter fruit: ")
# User can press Tab to autocomplete
```

## History

```sapphire
import readline

# Enable command history
readline.enable_history()

# Save history to file
readline.save_history("history.txt")

# Load history from file
readline.load_history("history.txt")

# Get previous commands
let history = readline.get_history()
for cmd in history:
    print(cmd)
```

## Examples

### Simple Calculator

```sapphire
fn calculator():
    print("Simple Calculator")
    print("Enter 'quit' to exit")
    
    while true:
        let expr = input(">>> ")
        
        if expr == "quit":
            break
        
        try:
            let result = eval(expr)
            print(f"= {result}")
        catch SyntaxError:
            print("Invalid expression")
```

### User Registration

```sapphire
fn register_user():
    print("=== User Registration ===")
    
    let username = input("Username: ")
    let email = get_validated_input("Email: ", validate_email)
    let password = getpass.getpass("Password: ")
    let confirm = getpass.getpass("Confirm password: ")
    
    if password != confirm:
        print("Passwords don't match!")
        return None
    
    let age = input("Age: ").to_int()
    
    if prompt.confirm("Create account?"):
        return User {
            username: username,
            email: email,
            password: hash_password(password),
            age: age
        }
    else:
        return None
```

### Interactive Shell

```sapphire
fn shell():
    print("Sapphire Shell v1.0")
    print("Type 'help' for commands")
    
    let commands = {
        "help": show_help,
        "ls": list_files,
        "cd": change_dir,
        "pwd": print_working_dir,
        "exit": || sys.exit(0)
    }
    
    while true:
        let line = input("$ ")
        let parts = line.split()
        
        if parts.is_empty():
            continue
        
        let cmd = parts[0]
        let args = parts[1:]
        
        if cmd in commands:
            commands[cmd](args)
        else:
            print(f"Unknown command: {cmd}")
```

## Best Practices

1. **Always validate input** - Never trust user input
2. **Provide clear prompts** - Tell users what format you expect
3. **Handle errors gracefully** - Catch invalid input and ask again
4. **Use type conversion carefully** - Check for ValueError
5. **Sanitize input** - Remove dangerous characters
6. **Use getpass for passwords** - Don't show passwords on screen
7. **Provide defaults** - Make common choices easy
8. **Give feedback** - Show progress for long operations

## Example Code

See [examples/user_input_example.spp](../examples/user_input_example.spp) for a complete working example.

## See Also

- [File I/O](FILE_IO.md)
- [Command-Line Arguments](CLI_ARGUMENTS.md)
- [Error Handling](ERROR_HANDLING.md)
- [Standard Library](STANDARD_LIBRARY.md)
