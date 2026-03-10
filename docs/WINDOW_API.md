# Sapphire Window API

Built-in window and graphics support for creating interactive applications and games.

## Overview

Sapphire provides built-in functions for creating windows, handling keyboard input, and rendering graphics. These functions are compiled directly into native code with SDL2 backend.

## Quick Start

```sapphire
# Create a simple window
let window = window_create("My App", 800, 600)
window_show(window)

# Game loop
let running = true
while running:
    running = window_poll_events(window)
    
    window_clear(window, 0, 0, 0)
    window_fill_rect(window, 100, 100, 50, 50, 255, 0, 0)
    window_present(window)
    
    delay(16)

window_destroy(window)
```

## API Reference

### Window Management

#### `window_create(title: str, width: int, height: int) -> ptr`
Creates a new window (hidden by default).

**Parameters:**
- `title` - Window title text
- `width` - Window width in pixels
- `height` - Window height in pixels

**Returns:** Window handle (opaque pointer)

**Example:**
```sapphire
let window = window_create("Pong", 800, 600)
```

#### `window_show(window: ptr)`
Makes the window visible on screen.

**Example:**
```sapphire
window_show(window)
```

#### `window_poll_events(window: ptr) -> bool`
Processes window events (close, keyboard, etc.). Must be called every frame.

**Returns:** `true` if window is still open, `false` if user closed it

**Example:**
```sapphire
let running = window_poll_events(window)
```

#### `window_destroy(window: ptr)`
Closes and destroys the window. Call when done.

**Example:**
```sapphire
window_destroy(window)
```

### Rendering

#### `window_clear(window: ptr, r: int, g: int, b: int)`
Clears the window with a solid color.

**Parameters:**
- `r, g, b` - RGB color values (0-255)

**Example:**
```sapphire
window_clear(window, 0, 0, 0)  # Black background
```

#### `window_fill_rect(window: ptr, x: int, y: int, w: int, h: int, r: int, g: int, b: int)`
Draws a filled rectangle.

**Parameters:**
- `x, y` - Top-left corner position
- `w, h` - Width and height
- `r, g, b` - RGB color values (0-255)

**Example:**
```sapphire
window_fill_rect(window, 100, 100, 50, 50, 255, 0, 0)  # Red square
```

#### `window_present(window: ptr)`
Displays the rendered frame. Call after all drawing is done.

**Example:**
```sapphire
window_present(window)
```

### Keyboard Input

#### `window_is_key_down(window: ptr, keycode: int) -> bool`
Checks if a key is currently pressed.

**Parameters:**
- `keycode` - SDL2 scancode (see Key Codes section)

**Returns:** `true` if key is pressed, `false` otherwise

**Example:**
```sapphire
if window_is_key_down(window, 26):  # W key
    player_y = player_y - 5
```

### Timing

#### `delay(milliseconds: int)`
Pauses execution for the specified time. Useful for frame rate control.

**Example:**
```sapphire
delay(16)  # ~60 FPS (1000ms / 60 = 16.67ms)
```

## Key Codes

SDL2 scancodes for common keys:

```sapphire
# Letters
let KEY_A = 4
let KEY_B = 5
let KEY_W = 26
let KEY_S = 22
let KEY_D = 7

# Arrow keys
let KEY_UP = 82
let KEY_DOWN = 81
let KEY_LEFT = 80
let KEY_RIGHT = 79

# Special keys
let KEY_SPACE = 44
let KEY_ESCAPE = 41
let KEY_RETURN = 40
```

For complete list, see: [SDL_scancode.h](https://wiki.libsdl.org/SDL2/SDL_Scancode)

## Complete Example: Bouncing Ball

```sapphire
# Bouncing ball demo
let window = window_create("Bouncing Ball", 800, 600)
window_show(window)

let ball_x = 400
let ball_y = 300
let ball_vx = 5
let ball_vy = 3

let frame = 0
while frame < 1000:
    window_poll_events(window)
    
    # Update position
    ball_x = ball_x + ball_vx
    ball_y = ball_y + ball_vy
    
    # Bounce off walls
    if ball_x < 0:
        ball_vx = 0 - ball_vx
    if ball_x > 780:
        ball_vx = 0 - ball_vx
    if ball_y < 0:
        ball_vy = 0 - ball_vy
    if ball_y > 580:
        ball_vy = 0 - ball_vy
    
    # Render
    window_clear(window, 0, 0, 0)
    window_fill_rect(window, ball_x, ball_y, 20, 20, 255, 255, 0)
    window_present(window)
    
    delay(16)
    frame = frame + 1

window_destroy(window)
```

## Game Loop Pattern

Standard pattern for interactive applications:

```sapphire
# Setup
let window = window_create("Game", 800, 600)
window_show(window)

# Game state
let player_x = 100
let player_y = 100

# Main loop
let running = true
while running:
    # 1. Handle events
    running = window_poll_events(window)
    
    # 2. Handle input
    if window_is_key_down(window, 26):  # W
        player_y = player_y - 5
    if window_is_key_down(window, 22):  # S
        player_y = player_y + 5
    
    # 3. Update game logic
    # ... physics, collisions, etc ...
    
    # 4. Render
    window_clear(window, 0, 0, 0)
    window_fill_rect(window, player_x, player_y, 50, 50, 0, 255, 0)
    window_present(window)
    
    # 5. Frame timing
    delay(16)

# Cleanup
window_destroy(window)
```

## Compilation

Window programs require SDL2 libraries at link time:

```bash
# Compile Sapphire to LLVM IR
./sapp compile game.spp 2>/dev/null > game.ll

# Compile to object file
llc -filetype=obj game.ll -o game.o

# Link with SDL2
clang game.o stdlib/gui/window.cpp -o game -lSDL2 -lSDL2_ttf -lstdc++

# Run
./game
```

Or use the automated script pattern (see `run_pong.sh` for example).

## System Requirements

### Linux (Fedora/RHEL)
```bash
sudo dnf install SDL2-devel SDL2_ttf-devel
```

### Linux (Ubuntu/Debian)
```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev
```

### macOS
```bash
brew install sdl2 sdl2_ttf
```

## Performance

- Native code generation (no interpreter overhead)
- Hardware-accelerated rendering (SDL2 + GPU)
- 60 FPS achievable on modern hardware
- Comparable to C/C++ game performance

## Examples

See `examples/challenges/round3_field_level/` for complete examples:
- `test_window.spp` - Basic window test
- `pong_simple.spp` - Complete Pong game
- `pong_game.cpp` - C++ reference implementation

## Limitations

Current limitations:
- No text rendering from Sapphire (use C++ wrapper)
- No image loading (future feature)
- No audio support (future feature)
- Fixed to SDL2 backend (no alternative renderers)

## Future Enhancements

Planned features:
- Sprite/texture support
- Audio playback
- Mouse input
- Gamepad support
- Text rendering from Sapphire
- 3D graphics (OpenGL integration)

---

**Status:** Production-ready for 2D games and interactive applications!
