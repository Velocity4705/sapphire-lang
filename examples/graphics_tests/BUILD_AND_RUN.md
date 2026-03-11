# Building and Running Graphics Examples

This guide shows how to compile and run each graphics example.

## Prerequisites

1. Sapphire compiler built: `make quick`
2. Graphics libraries built: `cd stdlib/graphics && make`
3. SDL2, OpenGL, GLUT installed on your system

## Quick Start

Use the provided build script:

```bash
./build_example.sh game_pong
./build_example.sh game_snake
./build_example.sh app_paint
./build_example.sh demo_particles
./build_example.sh demo_3d_cube
```

## Manual Build Process

### SDL2 Examples (Pong, Snake, Paint, Particles)

```bash
# 1. Compile to LLVM IR
./sapp compile examples/graphics_tests/game_pong.spp 2>/dev/null > game_pong.ll

# 2. Compile to object file
llc game_pong.ll -filetype=obj -o game_pong.o

# 3. Link with SDL2 library
clang game_pong.o -o game_pong \
    -L./stdlib/graphics/sdl2 \
    -lsapphire_sdl2 \
    -lSDL2 \
    -lstdc++

# 4. Run
LD_LIBRARY_PATH=./stdlib/graphics/sdl2:$LD_LIBRARY_PATH ./game_pong
```

### OpenGL/GLUT Examples (3D Cube)

```bash
# 1. Compile to LLVM IR
./sapp compile examples/graphics_tests/demo_3d_cube.spp 2>/dev/null > demo_3d_cube.ll

# 2. Compile to object file
llc demo_3d_cube.ll -filetype=obj -o demo_3d_cube.o

# 3. Link with OpenGL and GLUT libraries
clang demo_3d_cube.o -o demo_3d_cube \
    -L./stdlib/graphics/opengl \
    -L./stdlib/graphics/glut \
    -lsapphire_opengl \
    -lsapphire_glut \
    -lGL \
    -lGLU \
    -lglut \
    -lstdc++

# 4. Run
LD_LIBRARY_PATH=./stdlib/graphics/opengl:./stdlib/graphics/glut:$LD_LIBRARY_PATH ./demo_3d_cube
```

## Examples Overview

### 🎮 game_pong.spp
Classic Pong game with two paddles and a ball.
- **Controls**: W/S (left paddle), Up/Down (right paddle), ESC (quit)
- **Features**: Collision detection, scoring, game loop
- **Library**: SDL2

### 🐍 game_snake.spp
Snake game where you eat food and grow.
- **Controls**: Arrow keys (move), ESC (quit)
- **Features**: Grid-based movement, collision detection, scoring
- **Library**: SDL2

### 🎨 app_paint.spp
Simple paint application for drawing.
- **Controls**: WASD (move brush), 1-5 (colors), C (clear), ESC (quit)
- **Features**: Color selection, drawing, canvas management
- **Library**: SDL2

### ✨ demo_particles.spp
Animated particle system with colorful bouncing particles.
- **Controls**: ESC (quit), or wait 10 seconds
- **Features**: Multiple particles, motion trails, bouncing physics
- **Library**: SDL2

### 🎲 demo_3d_cube.spp
Rotating 3D cube with colored faces.
- **Controls**: Automatic rotation
- **Features**: 3D transformations, depth testing, OpenGL rendering
- **Library**: OpenGL + GLUT

## Troubleshooting

### "undefined reference" errors
Make sure graphics libraries are built:
```bash
cd stdlib/graphics
make
cd ../..
```

### "cannot open shared object file"
Set the library path:
```bash
export LD_LIBRARY_PATH=./stdlib/graphics/sdl2:./stdlib/graphics/opengl:./stdlib/graphics/glut:$LD_LIBRARY_PATH
```

### Window doesn't appear
- Make sure you have a display server running (X11/Wayland)
- Try running from a graphical terminal
- Check that SDL2/OpenGL/GLUT are installed

### Compilation errors
- Make sure you're using `./sapp compile`, not `./sapp` (interpreter)
- Check that LLVM is installed: `llc --version`
- Verify clang is installed: `clang --version`

## Performance Tips

1. **Optimize compilation**: Add `-O2` or `-O3` to clang
2. **VSync**: SDL2 examples run at ~60 FPS
3. **Resolution**: Adjust window size in source code if needed

## Next Steps

- Modify the examples to create your own games
- Combine multiple graphics libraries
- Add sound effects (future feature)
- Create more complex game mechanics
- Build a complete game engine

## Learning Resources

- SDL2 functions: See `stdlib/graphics/sdl2/sdl2.h`
- OpenGL functions: See `stdlib/graphics/opengl/opengl.h`
- GLUT functions: See `stdlib/graphics/glut/glut.h`
- Import guide: See `IMPORT_QUICK_REFERENCE.md`
