# How to Import Graphics Modules in Sapphire

## Current Status (March 8, 2026)

### Import System Status: ⏳ Partially Implemented

**What Works:**
- ✅ Import syntax is **parsed** (no syntax errors)
- ✅ Built-in functions work without imports
- ✅ Graphics libraries are built and ready

**What Doesn't Work Yet:**
- ❌ Imports don't actually load modules
- ❌ No module resolution system
- ❌ No external function declarations from imports

---

## How Graphics Currently Work

### Built-in Functions (Work Today!)

Sapphire has **built-in graphics functions** that work without any imports:

```sapphire
fn main():
    # Window management
    let window = window_create("My Game", 800, 600)
    window_show(window)
    
    # Rendering
    window_clear(window, 0, 0, 0)
    window_fill_rect(window, 100, 100, 200, 150, 255, 255, 255)
    window_present(window)
    
    # Input
    window_poll_events(window)
    if window_is_key_down(window, 26):  # W key
        print("W pressed")
    
    # Timing
    delay(16)  # 60 FPS
    
    # Cleanup
    window_destroy(window)
```

**Compile and run:**
```bash
./sapp compile game.spp > game.ll
llc -filetype=obj game.ll -o game.o
clang game.o -o game stdlib/gui/window.cpp -Istdlib -lSDL2 -lSDL2_ttf -lstdc++
./game
```

---

## Available Built-in Functions

### Window Management
```sapphire
window_create(title: string, width: int, height: int) -> ptr
window_show(window: ptr)
window_hide(window: ptr)
window_destroy(window: ptr)
window_should_close(window: ptr) -> bool
window_poll_events(window: ptr)
```

### Rendering
```sapphire
window_clear(window: ptr, r: int, g: int, b: int)
window_present(window: ptr)
window_fill_rect(window: ptr, x: int, y: int, w: int, h: int, r: int, g: int, b: int)
```

### Input
```sapphire
window_is_key_down(window: ptr, scancode: int) -> bool
```

### Timing
```sapphire
delay(ms: int)
SDL_Delay(ms: int)  # Alternative
```

### OpenGL Functions
```sapphire
glClearColor(r: float, g: float, b: float, a: float)
glClear(mask: int)
glBegin(mode: int)
glEnd()
glVertex3f(x: float, y: float, z: float)
glColor3f(r: float, g: float, b: float)
glTranslatef(x: float, y: float, z: float)
glRotatef(angle: float, x: float, y: float, z: float)
glMatrixMode(mode: int)
glLoadIdentity()
```

---

## Import Syntax (Parsed but Not Implemented)

These syntaxes are **valid** but don't do anything yet:

### Simple Import
```sapphire
import math
import os
import graphics
```

### Import with Alias
```sapphire
import math as m
import os as operating_system
```

### From Import
```sapphire
from math import sqrt, pow
from collections import Vec, HashMap
```

### From Import with Alias
```sapphire
from math import sqrt as square_root
```

---

## Future: When Imports Are Implemented

Once the import system is fully implemented, you'll be able to:

### Option 1: Import Graphics Module
```sapphire
import graphics

fn main():
    let window = graphics.window_create("Game", 800, 600)
    graphics.window_show(window)
    # ...
```

### Option 2: From Import
```sapphire
from graphics import window_create, window_show, window_clear

fn main():
    let window = window_create("Game", 800, 600)
    window_show(window)
    # ...
```

### Option 3: Import Specific API
```sapphire
import graphics.sdl2
import graphics.opengl
import graphics.vulkan

fn main():
    # Use SDL2
    let window = sdl2.create_window("Game", 800, 600)
    
    # Use OpenGL
    opengl.clear_color(0.0, 0.0, 0.0, 1.0)
    
    # Use Vulkan
    let context = vulkan.create_context("Game", 800, 600)
```

---

## Workaround: Direct Function Calls

Until imports are implemented, use the built-in functions directly:

### Example: Simple Game
```sapphire
fn main():
    print("Starting game...")
    
    let window = window_create("My Game", 800, 600)
    window_show(window)
    
    let running = true
    let player_x = 400
    let player_y = 300
    
    while running:
        window_poll_events(window)
        
        if window_should_close(window):
            running = false
            break
        
        # Input
        if window_is_key_down(window, 26):  # W
            player_y = player_y - 5
        if window_is_key_down(window, 22):  # S
            player_y = player_y + 5
        if window_is_key_down(window, 4):   # A
            player_x = player_x - 5
        if window_is_key_down(window, 7):   # D
            player_x = player_x + 5
        
        # Render
        window_clear(window, 0, 0, 0)
        window_fill_rect(window, player_x, player_y, 50, 50, 255, 255, 255)
        window_present(window)
        
        delay(16)  # 60 FPS
    
    window_destroy(window)
    print("Game ended")
```

---

## Key Codes

Common keyboard scancodes for `window_is_key_down()`:

```
W = 26    A = 4     S = 22    D = 7
Q = 20    E = 8     R = 21    F = 9

UP = 82       DOWN = 81
LEFT = 80     RIGHT = 79

SPACE = 44    RETURN = 40
ESCAPE = 41   BACKSPACE = 42

0-9 = 39, 30-38
```

---

## OpenGL Constants

For use with OpenGL functions:

```
# Buffer bits
GL_COLOR_BUFFER_BIT = 0x00004000
GL_DEPTH_BUFFER_BIT = 0x00000100

# Primitive modes
GL_POINTS = 0x0000
GL_LINES = 0x0001
GL_TRIANGLES = 0x0004
GL_QUADS = 0x0007

# Matrix modes
GL_MODELVIEW = 0x1700
GL_PROJECTION = 0x1701
```

---

## Implementation Status

### What's Complete
- ✅ Import syntax parsing
- ✅ Built-in graphics functions
- ✅ C++ graphics libraries (SDL2, OpenGL, GLUT, Vulkan)
- ✅ Working examples

### What's Needed
- ⏳ Module resolution system
- ⏳ Import statement code generation
- ⏳ External function declaration from modules
- ⏳ Module search paths

---

## Recommendation

**For now:** Use the built-in `window_*` and `gl*` functions directly. They work great!

**Future:** Once imports are implemented, you'll be able to organize code into modules and import graphics APIs cleanly.

---

## Examples

See these working examples:
- `examples/test_window_visible.spp` - Window with colored rectangles
- `examples/test_window_create.spp` - Basic window creation
- `examples/challenges/round3_field_level/pong_sdl2.spp` - Pong game

---

**Date**: March 8, 2026  
**Status**: Built-in functions work, imports parsed but not implemented  
**Workaround**: Use built-in functions directly
