# Sapphire Standard Library - New Libraries

This document describes the newly implemented standard library modules for Sapphire.

## Overview

Three major library modules have been added to extend Sapphire's capabilities:

1. **DateTime** - Date, time, and duration utilities
2. **Graphics** - 2D graphics and canvas operations
3. **Utils** - String manipulation, type conversion, and utility functions

## DateTime Library

Located in: `stdlib/datetime/`

### Classes

#### `Instant`
Represents a point in time with millisecond precision.

```sapphire
let now = Instant.now()
let elapsed = now - another_instant  # Returns Duration
```

**Methods:**
- `static now()` - Get current time
- `to_milliseconds()` - Get timestamp in milliseconds
- `operator-()` - Calculate duration between two instants

#### `Duration`
Represents a time span.

```sapphire
let duration = instant1 - instant2
print(duration.seconds())
print(duration.minutes())
print(duration.hours())
```

**Methods:**
- `seconds()` - Get duration in seconds (double)
- `minutes()` - Get duration in minutes (double)
- `hours()` - Get duration in hours (double)
- `total_milliseconds()` - Get duration in milliseconds (int64)

#### `Date`
Represents a calendar date.

```sapphire
let today = Date.today()
let custom = Date(2024, 3, 15)
print(today.to_string())  # "2024-03-15"
```

**Methods:**
- `static today()` - Get today's date
- `to_string()` - Format as "YYYY-MM-DD"

#### `Time`
Represents a time of day.

```sapphire
let current = Time.now()
print(current.to_string())  # "14:30:45"
```

**Methods:**
- `static now()` - Get current time
- `to_string()` - Format as "HH:MM:SS"

#### `DateTime`
Combines date and time.

```sapphire
let now = DateTime.now()
print(now.to_string())  # "2024-03-15 14:30:45"
```

**Methods:**
- `static now()` - Get current date and time
- `to_string()` - Format as "YYYY-MM-DD HH:MM:SS"

### Usage Example

```sapphire
import datetime

let start = Instant.now()

# Do some work...
let x = 0
for i in range(1000000):
    x = x + i

let end = Instant.now()
let elapsed = end - start

print("Elapsed time: ")
print(elapsed.seconds())
print(" seconds")
```

## Graphics Library

Located in: `stdlib/graphics/`

### Structures

#### `Color`
Represents an RGBA color.

```sapphire
let red = Color.red()
let custom = Color(255, 128, 64, 255)
```

**Predefined Colors:**
- `Color.white()` - (255, 255, 255)
- `Color.black()` - (0, 0, 0)
- `Color.red()` - (255, 0, 0)
- `Color.green()` - (0, 255, 0)
- `Color.blue()` - (0, 0, 255)
- `Color.yellow()` - (255, 255, 0)
- `Color.cyan()` - (0, 255, 255)
- `Color.magenta()` - (255, 0, 255)

#### `Point`
Represents a 2D point.

```sapphire
let p = Point(100.0, 200.0)
```

#### `Rect`
Represents a rectangle.

```sapphire
let r = Rect(10.0, 20.0, 100.0, 50.0)  # x, y, width, height
```

### Canvas Class

Main drawing surface for graphics operations.

```sapphire
let canvas = Canvas(800, 600)
canvas.set_fill_color(Color.blue())
canvas.fill_rect(Rect(50.0, 50.0, 100.0, 100.0))
canvas.save_ppm("output.ppm")
```

**Methods:**
- `Canvas(width, height)` - Create canvas
- `width()` - Get canvas width
- `height()` - Get canvas height
- `set_fill_color(color)` - Set fill color
- `set_stroke_color(color)` - Set stroke color
- `set_stroke_width(width)` - Set line width
- `clear(color)` - Clear canvas with color
- `draw_pixel(x, y, color)` - Draw single pixel
- `draw_line(p1, p2)` - Draw line
- `draw_rect(rect)` - Draw rectangle outline
- `fill_rect(rect)` - Fill rectangle
- `draw_circle(center, radius)` - Draw circle outline
- `fill_circle(center, radius)` - Fill circle
- `draw_triangle(p1, p2, p3)` - Draw triangle outline
- `fill_triangle(p1, p2, p3)` - Fill triangle
- `save_ppm(filename)` - Save as PPM image

### Usage Example

```sapphire
import graphics

let canvas = Canvas(400, 300)
canvas.clear(Color.white())

# Draw filled rectangle
canvas.set_fill_color(Color.red())
canvas.fill_rect(Rect(50.0, 50.0, 100.0, 100.0))

# Draw circle
canvas.set_fill_color(Color.blue())
canvas.fill_circle(Point(200.0, 150.0), 50.0)

# Draw line
canvas.set_stroke_color(Color.black())
canvas.draw_line(Point(0.0, 0.0), Point(400.0, 300.0))

canvas.save_ppm("drawing.ppm")
```

## Utils Library

Located in: `stdlib/utils/`

### String Functions

#### Case Conversion
```sapphire
import utils

let upper = Utils.to_upper("hello")      # "HELLO"
let lower = Utils.to_lower("WORLD")      # "world"
```

#### Trimming
```sapphire
let trimmed = Utils.trim("  hello  ")    # "hello"
let ltrimmed = Utils.ltrim("  hello")    # "hello"
let rtrimmed = Utils.rtrim("hello  ")    # "hello"
```

#### Splitting and Joining
```sapphire
let parts = Utils.split("a,b,c", ",")   # ["a", "b", "c"]
let joined = Utils.join(parts, "-")     # "a-b-c"
```

#### String Predicates
```sapphire
Utils.starts_with("hello", "he")        # true
Utils.ends_with("hello", "lo")          # true
Utils.contains("hello", "ll")           # true
```

#### String Replacement
```sapphire
let replaced = Utils.replace("hello", "l", "L")  # "heLLo"
let repeated = Utils.repeat("ab", 3)             # "ababab"
```

### Type Conversion

```sapphire
let str_int = Utils.to_string(42)       # "42"
let str_double = Utils.to_string(3.14)  # "3.14"
let str_bool = Utils.to_string(true)    # "true"

let num = Utils.to_int("42")            # 42
let decimal = Utils.to_double("3.14")   # 3.14
let flag = Utils.to_bool("true")        # true
```

### Character Classification

```sapphire
Utils.is_digit('5')                     # true
Utils.is_alpha('a')                     # true
Utils.is_alnum('5')                     # true
Utils.is_whitespace(' ')                # true
```

### Math Utilities

```sapphire
Utils.abs(-42)                          # 42
Utils.min(10, 20)                       # 10
Utils.max(10, 20)                       # 20
```

### Random Utilities

```sapphire
let rand_int = Utils.random_int(1, 100)        # Random int 1-100
let rand_double = Utils.random_double(0.0, 1.0) # Random double 0-1
```

### Hash Utilities

```sapphire
let hash = Utils.hash_string("hello")   # uint32 hash value
```

## Building with New Libraries

To include these libraries in your build, update the Makefile:

```makefile
SOURCES += stdlib/datetime/datetime.cpp \
           stdlib/graphics/graphics.cpp \
           stdlib/utils/utils.cpp
```

Then rebuild:
```bash
make clean
make quick
```

## Future Extensions

Planned additions to the standard library:

- **Network** - HTTP, TCP/UDP sockets
- **JSON** - JSON parsing and serialization
- **Regex** - Regular expressions
- **Compression** - Gzip, zlib support
- **Crypto** - Hashing, encryption
- **Database** - SQL database support
- **Threading** - Concurrent programming
- **Audio** - Audio playback and recording
- **Video** - Video processing

## Notes

- All libraries use the `sapphire::stdlib` namespace
- C API exports are provided for code generation
- Libraries follow RAII principles for resource management
- Thread-safe where applicable (e.g., random number generation)
