# Graphics Test Examples

These examples demonstrate graphics functionality and require **compiled mode** to run.

## Quick Start

```bash
# Build any example
./build_example.sh game_pong
./build_example.sh game_snake
./build_example.sh app_paint
./build_example.sh demo_particles
./build_example.sh demo_3d_cube

# Run (library path is set automatically)
./game_pong
```

## 🎮 Games

### game_pong.spp - Classic Pong
Two-player Pong game with scoring.

**Controls:**
- W/S: Left paddle
- Up/Down: Right paddle
- ESC: Quit

**Features:**
- Ball physics and collision
- Paddle movement
- Score tracking
- 60 FPS gameplay

**Build:** `./build_example.sh game_pong`

---

### game_snake.spp - Snake Game
Eat food, grow longer, don't hit walls!

**Controls:**
- Arrow keys: Move snake
- ESC: Quit

**Features:**
- Grid-based movement
- Food spawning
- Collision detection
- Progressive difficulty

**Build:** `./build_example.sh game_snake`

---

## 🎨 Applications

### app_paint.spp - Paint Application
Simple drawing application with color selection.

**Controls:**
- WASD: Move brush
- 1-5: Select color (White/Red/Green/Blue/Yellow)
- C: Clear canvas
- ESC: Quit

**Features:**
- Multiple colors
- Brush cursor
- Canvas persistence
- Color palette UI

**Build:** `./build_example.sh app_paint`

---

## ✨ Demos

### demo_particles.spp - Particle System
Animated bouncing particles with motion trails.

**Controls:**
- ESC: Quit (or wait 10 seconds)

**Features:**
- 5 colored particles
- Motion trail effects
- Bouncing physics
- Smooth animation

**Build:** `./build_example.sh demo_particles`

---

### demo_3d_cube.spp - 3D Rotating Cube
OpenGL 3D graphics demonstration.

**Controls:**
- Automatic rotation

**Features:**
- 3D transformations
- Colored faces
- Depth testing
- Smooth rotation

**Build:** `./build_example.sh demo_3d_cube`

---

## Why These Are Separate

The main `test_examples.sh` script runs examples through the **interpreter**, which doesn't support:
- Import statements
- External C library functions
- Graphics built-in functions

These examples are designed for the **compiler** and must be compiled to native executables.

## Building and Running

### Using the Build Script (Recommended)

```bash
./build_example.sh <example_name>
./<example_name>
```

### Manual Build (SDL2 examples)

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

### Manual Build (OpenGL examples)

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

## Prerequisites

1. **Sapphire compiler**: `make quick`
2. **Graphics libraries**: `cd stdlib/graphics && make`
3. **System libraries**: SDL2, OpenGL, GLUT installed

## Troubleshooting

### "undefined reference" errors
```bash
cd stdlib/graphics
make
cd ../..
```

### "cannot open shared object file"
```bash
export LD_LIBRARY_PATH=./stdlib/graphics/sdl2:./stdlib/graphics/opengl:./stdlib/graphics/glut:$LD_LIBRARY_PATH
```

### Window doesn't appear
- Ensure display server is running (X11/Wayland)
- Run from graphical terminal
- Check SDL2/OpenGL/GLUT installation

### Compilation errors
- Use `./sapp compile`, not `./sapp` (interpreter)
- Check LLVM: `llc --version`
- Check clang: `clang --version`

## Learning from These Examples

### Game Development Patterns
- **Game loop**: Update → Render → Delay
- **Input handling**: Keyboard state checking
- **Collision detection**: Bounding box checks
- **State management**: Variables for game state

### Graphics Techniques
- **Double buffering**: Clear → Draw → Present
- **Color management**: RGB values (0-255)
- **Animation**: Update positions each frame
- **Visual effects**: Motion trails, particles

### Code Organization
- **Initialization**: Setup window and state
- **Main loop**: Game/render loop
- **Cleanup**: Destroy resources
- **Input**: Handle user interaction

## Next Steps

1. **Modify examples**: Change colors, speeds, sizes
2. **Combine features**: Mix game mechanics
3. **Add features**: Sound, menus, high scores
4. **Create new games**: Use these as templates
5. **Learn more**: Study the source code

## Documentation

- **Build guide**: `BUILD_AND_RUN.md`
- **Import reference**: `../../IMPORT_QUICK_REFERENCE.md`
- **SDL2 API**: `../../stdlib/graphics/sdl2/sdl2.h`
- **OpenGL API**: `../../stdlib/graphics/opengl/opengl.h`
- **GLUT API**: `../../stdlib/graphics/glut/glut.h`

## Note

These examples demonstrate the power of Sapphire's graphics import system. They're fully functional games and applications that compile to native code with performance comparable to C/C++.

For regular Sapphire examples that work in the interpreter, see the parent `examples/` directory.
