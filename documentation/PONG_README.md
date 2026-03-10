# Pong Game - Sapphire Language Demo

A simple but fully functional Pong game written in Sapphire, demonstrating the language's SDL2 graphics capabilities.

## Features

- Two-player local multiplayer
- Smooth 60 FPS gameplay
- Paddle collision detection
- Score tracking (first to 5 wins)
- Keyboard controls

## Controls

- **Left Paddle**: W (up) / S (down)
- **Right Paddle**: Up Arrow / Down Arrow
- **Quit**: ESC key or close window

## How to Play

### Quick Start
```bash
./play_pong.sh
```

### Manual Build
```bash
# Compile to LLVM IR
./sapp compile working_pong.spp > working_pong.ll

# Compile to object file
llc working_pong.ll -filetype=obj -o working_pong.o

# Link with SDL2
clang working_pong.o -o working_pong -L./stdlib/graphics/sdl2 -lsapphire_sdl2 -lSDL2 -lstdc++

# Run
LD_LIBRARY_PATH=./stdlib/graphics/sdl2:$LD_LIBRARY_PATH ./working_pong
```

## Requirements

- Sapphire compiler (`sapp`)
- LLVM toolchain (`llc`)
- Clang
- SDL2 library
- Sapphire SDL2 wrapper (included in `stdlib/graphics/sdl2/`)

## Game Rules

1. Two players control paddles on opposite sides of the screen
2. The ball bounces off paddles and top/bottom walls
3. If the ball passes a paddle, the opponent scores a point
4. First player to reach 5 points wins
5. Ball resets to center after each score

## Technical Details

- **Language**: Sapphire (top-level statement syntax)
- **Graphics**: SDL2 via Sapphire's graphics.sdl2 module
- **Resolution**: 800x600 pixels
- **Frame Rate**: ~60 FPS (16ms delay per frame)
- **Paddle Size**: 15x100 pixels
- **Ball Size**: 15x15 pixels
- **Ball Speed**: 5 pixels/frame horizontal, 4 pixels/frame vertical

## Code Structure

The game uses Sapphire's top-level statement syntax (no explicit `fn main()`):
- Import SDL2 graphics module
- Initialize window and game state
- Main game loop with `while running:` condition
- Input handling, physics, collision detection
- Rendering with SDL2 primitives
- Clean shutdown

## Known Limitations

- No AI opponent (two-player only)
- No sound effects
- No pause functionality
- Fixed ball speed (doesn't increase)
- Simple rectangular collision detection

## Future Enhancements

- Single-player mode with AI
- Difficulty levels
- Sound effects
- Power-ups
- Ball speed increase over time
- Better collision physics (angle-based bounces)

## License

Part of the Sapphire language project. See main LICENSE file.
