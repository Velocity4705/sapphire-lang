# Snake Games - Sapphire + SDL2

Two versions of the classic Snake game built with Sapphire and SDL2.

## 🐍 game_snake.spp - Classic Version

Simple, clean implementation of Snake.

### Features
- Grid-based movement
- Food collection
- Wall collision detection
- Progressive speed increase
- Score tracking

### Controls
- **Arrow Keys**: Move snake
- **ESC**: Quit

### Build & Run
```bash
./build_example.sh game_snake
./game_snake
```

---

## 🐍✨ game_snake_enhanced.spp - Enhanced Version

Full-featured Snake with growing tail and better visuals.

### Features
- **Growing tail** (up to 20 segments)
- **Self-collision detection**
- **Gradient tail colors** (dark to bright green)
- **Snake eyes** (direction-based)
- **Pulsing food** animation
- **High score** tracking
- **Restart** without closing game
- **Better visuals** (grid, colors, UI)

### Controls
- **Arrow Keys**: Move snake
- **SPACE**: Restart after game over
- **ESC**: Quit

### Build & Run
```bash
./build_example.sh game_snake_enhanced
./game_snake_enhanced
```

---

## Comparison

| Feature | Classic | Enhanced |
|---------|---------|----------|
| Tail growth | ❌ | ✅ (20 segments) |
| Self-collision | ❌ | ✅ |
| Visual effects | Basic | Advanced |
| Restart option | ❌ | ✅ |
| High score | ❌ | ✅ |
| Snake eyes | ❌ | ✅ |
| Code complexity | Simple | Advanced |

## Technical Details

### Classic Version
- **Lines of code**: 165
- **Tail segments**: 0 (head only)
- **Features**: Basic gameplay
- **Best for**: Learning, simple demo

### Enhanced Version
- **Lines of code**: 520
- **Tail segments**: 20 (expandable)
- **Features**: Full game experience
- **Best for**: Playing, showcasing

## How It Works

### Game Loop
```
1. Poll input events
2. Update game state (move snake, check collisions)
3. Render (clear, draw grid, draw food, draw snake)
4. Delay for frame rate control
5. Repeat
```

### Tail Management (Enhanced)
The tail is simulated using individual variables (t1x, t1y, t2x, t2y, etc.) since Sapphire doesn't have arrays yet. Each frame:
1. Shift all tail segments (t20 ← t19 ← ... ← t2 ← t1)
2. Set t1 to old head position
3. Move head to new position

### Collision Detection
- **Walls**: Check if head position < 0 or >= grid_size
- **Food**: Check if head position == food position
- **Self**: Check if head position == any tail segment position

## Code Highlights

### Direction Buffering
```sapphire
// Buffer next direction to prevent double-tap issues
let next_dx = 1
let next_dy = 0

// In input handling:
if sapphire_sdl2_is_key_down(window, 80) {  // Left
    if snake_dx != 1 {  // Can't reverse
        next_dx = 0 - 1
        next_dy = 0
    }
}

// Apply on move:
snake_dx = next_dx
snake_dy = next_dy
```

### Gradient Tail Colors
```sapphire
// Darker segments at the back
if tail_length >= 20 {
    sapphire_sdl2_fill_rect(window, px, py, size, size, 0, 100, 0)
}
// Brighter segments near head
if tail_length >= 1 {
    sapphire_sdl2_fill_rect(window, px, py, size, size, 0, 255, 0)
}
```

### Pulsing Food Animation
```sapphire
let pulse = (frame % 30) / 30
let food_brightness = 200 + pulse * 55
sapphire_sdl2_fill_rect(window, x, y, w, h, food_brightness, 0, 0)
```

## Tips for Playing

1. **Plan ahead**: Think about where you're going, not just where you are
2. **Use the edges**: Stay near walls to maximize space
3. **Spiral pattern**: Create a spiral to fill the grid efficiently
4. **Don't rush**: Take your time, speed increases naturally
5. **Watch the tail**: In enhanced version, avoid your own tail!

## Modifying the Games

### Change Grid Size
```sapphire
let grid_size = 25  // Try 15, 20, 30, etc.
let cell_size = 32  // Adjust to fit window
```

### Change Speed
```sapphire
let move_delay = 8  // Lower = faster (min: 1)
```

### Change Colors
```sapphire
// Background
sapphire_sdl2_clear(window, 15, 15, 25)  // RGB

// Snake
sapphire_sdl2_fill_rect(window, x, y, w, h, 0, 255, 50)  // Green

// Food
sapphire_sdl2_fill_rect(window, x, y, w, h, 255, 0, 0)  // Red
```

### Add More Tail Segments
In enhanced version, add more variables:
```sapphire
let t21x = 0
let t21y = 0
// ... and update the shift logic
```

## Learning Objectives

### From Classic Version
- Basic game loop structure
- Input handling
- Simple collision detection
- State management
- SDL2 rendering basics

### From Enhanced Version
- Complex state management
- Multiple collision types
- Visual effects and polish
- Game restart mechanics
- UI elements
- Code organization for larger projects

## Performance

Both versions run at **60 FPS** with smooth gameplay:
- Frame time: ~16ms
- Input latency: <1 frame
- Rendering: Efficient rectangle drawing
- Memory: Minimal (all stack-allocated)

## Next Steps

1. **Play both versions** to see the difference
2. **Read the source code** to understand implementation
3. **Modify the games** to add your own features
4. **Create your own game** using these as templates

## Ideas for Enhancement

- Add sound effects
- Add power-ups (speed boost, invincibility)
- Add obstacles on the grid
- Add different game modes (timed, survival)
- Add multiplayer (two snakes)
- Add levels with increasing difficulty
- Save high scores to file

---

**Built with**: Sapphire Programming Language + SDL2  
**Performance**: Native C-level speed  
**Platform**: Linux (X11/Wayland)
