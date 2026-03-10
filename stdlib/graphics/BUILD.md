# Building Sapphire Graphics Libraries

This document explains how to build and use the graphics libraries for Sapphire.

## Directory Structure

```
stdlib/graphics/
├── sdl2/
│   ├── sdl2.h          # SDL2 header
│   └── sdl2.cpp        # SDL2 implementation
├── opengl/
│   ├── opengl.h        # OpenGL header
│   └── opengl.cpp      # OpenGL implementation
├── glut/
│   ├── glut.h          # GLUT header
│   └── glut.cpp        # GLUT implementation
└── vulkan/
    ├── vulkan.h        # Vulkan header
    └── vulkan.cpp      # Vulkan implementation
```

## Building the Libraries

### SDL2 Library

```bash
# Compile SDL2 library
g++ -c stdlib/graphics/sdl2/sdl2.cpp -o stdlib/graphics/sdl2/sdl2.o \
    -Istdlib/graphics/sdl2 -std=c++17 -fPIC

# Create static library
ar rcs stdlib/graphics/sdl2/libsapphire_sdl2.a stdlib/graphics/sdl2/sdl2.o

# Or create shared library
g++ -shared -o stdlib/graphics/sdl2/libsapphire_sdl2.so \
    stdlib/graphics/sdl2/sdl2.o -lSDL2
```

### OpenGL Library

```bash
# Compile OpenGL library
g++ -c stdlib/graphics/opengl/opengl.cpp -o stdlib/graphics/opengl/opengl.o \
    -Istdlib/graphics/opengl -std=c++17 -fPIC

# Create static library
ar rcs stdlib/graphics/opengl/libsapphire_opengl.a stdlib/graphics/opengl/opengl.o

# Or create shared library
g++ -shared -o stdlib/graphics/opengl/libsapphire_opengl.so \
    stdlib/graphics/opengl/opengl.o -lGL -lGLU
```

### GLUT Library

```bash
# Compile GLUT library
g++ -c stdlib/graphics/glut/glut.cpp -o stdlib/graphics/glut/glut.o \
    -Istdlib/graphics/glut -std=c++17 -fPIC

# Create static library
ar rcs stdlib/graphics/glut/libsapphire_glut.a stdlib/graphics/glut/glut.o

# Or create shared library
g++ -shared -o stdlib/graphics/glut/libsapphire_glut.so \
    stdlib/graphics/glut/glut.o -lglut -lGL -lGLU
```

### Vulkan Library

```bash
# Compile Vulkan library
g++ -c stdlib/graphics/vulkan/vulkan.cpp -o stdlib/graphics/vulkan/vulkan.o \
    -Istdlib/graphics/vulkan -std=c++17 -fPIC

# Create static library
ar rcs stdlib/graphics/vulkan/libsapphire_vulkan.a stdlib/graphics/vulkan/vulkan.o

# Or create shared library
g++ -shared -o stdlib/graphics/vulkan/libsapphire_vulkan.so \
    stdlib/graphics/vulkan/vulkan.o -lvulkan -lSDL2
```

## Using the Libraries in Sapphire Programs

### SDL2 Example

```sapphire
# my_game.spp
import "stdlib/graphics/sdl2"

fn main():
    let window = sdl2_create_window("My Game", 800, 600)
    sdl2_show_window(window)
    
    while not sdl2_should_close(window):
        sdl2_poll_events(window)
        sdl2_clear(window, 0, 0, 0)
        sdl2_fill_rect(window, 100, 100, 50, 50, 255, 255, 255)
        sdl2_present(window)
        sdl2_delay(16)
    
    sdl2_destroy_window(window)
```

Compile and link:
```bash
./sapp compile my_game.spp 2>/dev/null > my_game.ll
llc -filetype=obj my_game.ll -o my_game.o
clang my_game.o -o my_game \
    -Lstdlib/graphics/sdl2 -lsapphire_sdl2 -lSDL2 -lstdc++
./my_game
```

### OpenGL Example

```sapphire
# my_3d_app.spp
import "stdlib/graphics/opengl"

fn main():
    gl_init()
    gl_clear_color(0.0, 0.0, 0.0, 1.0)
    gl_clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    gl_begin(GL_TRIANGLES)
    gl_color3f(1.0, 0.0, 0.0)
    gl_vertex3f(0.0, 1.0, 0.0)
    gl_color3f(0.0, 1.0, 0.0)
    gl_vertex3f(-1.0, -1.0, 0.0)
    gl_color3f(0.0, 0.0, 1.0)
    gl_vertex3f(1.0, -1.0, 0.0)
    gl_end()
```

Compile and link:
```bash
./sapp compile my_3d_app.spp 2>/dev/null > my_3d_app.ll
llc -filetype=obj my_3d_app.ll -o my_3d_app.o
clang my_3d_app.o -o my_3d_app \
    -Lstdlib/graphics/opengl -lsapphire_opengl -lGL -lGLU -lstdc++
./my_3d_app
```

### GLUT Example

```sapphire
# my_glut_app.spp
import "stdlib/graphics/glut"

fn main():
    glut_init()
    glut_init_display_mode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glut_init_window_size(800, 600)
    let window = glut_create_window("My GLUT App")
    
    glut_solid_sphere(1.0, 20, 20)
    glut_swap_buffers()
```

Compile and link:
```bash
./sapp compile my_glut_app.spp 2>/dev/null > my_glut_app.ll
llc -filetype=obj my_glut_app.ll -o my_glut_app.o
clang my_glut_app.o -o my_glut_app \
    -Lstdlib/graphics/glut -lsapphire_glut -lglut -lGL -lGLU -lstdc++
./my_glut_app
```

### Vulkan Example

```sapphire
# my_vulkan_app.spp
import "stdlib/graphics/vulkan"

fn main():
    let context = vulkan_create_context("My Vulkan App", 800, 600)
    vulkan_init(context)
    
    while not vulkan_should_close(context):
        vulkan_poll_events(context)
        vulkan_begin_frame(context)
        vulkan_draw_triangle(context)
        vulkan_end_frame(context)
    
    vulkan_destroy_context(context)
```

Compile and link:
```bash
./sapp compile my_vulkan_app.spp 2>/dev/null > my_vulkan_app.ll
llc -filetype=obj my_vulkan_app.ll -o my_vulkan_app.o
clang my_vulkan_app.o -o my_vulkan_app \
    -Lstdlib/graphics/vulkan -lsapphire_vulkan -lvulkan -lSDL2 -lstdc++
./my_vulkan_app
```

## Build All Libraries

Use the provided Makefile:

```bash
# Build all libraries
make -C stdlib/graphics all

# Build specific library
make -C stdlib/graphics sdl2
make -C stdlib/graphics opengl
make -C stdlib/graphics glut
make -C stdlib/graphics vulkan

# Clean build artifacts
make -C stdlib/graphics clean
```

## Dependencies

### Fedora/RHEL
```bash
sudo dnf install SDL2-devel mesa-libGL-devel mesa-libGLU-devel \
                 freeglut-devel vulkan-devel
```

### Ubuntu/Debian
```bash
sudo apt install libsdl2-dev libgl1-mesa-dev libglu1-mesa-dev \
                 freeglut3-dev libvulkan-dev
```

### macOS
```bash
brew install sdl2 freeglut molten-vk
```

## Integration with Sapphire Compiler

The Sapphire compiler should automatically link these libraries when it detects `import "stdlib/graphics/..."` statements.

Add to compiler link flags:
- SDL2: `-Lstdlib/graphics/sdl2 -lsapphire_sdl2 -lSDL2`
- OpenGL: `-Lstdlib/graphics/opengl -lsapphire_opengl -lGL -lGLU`
- GLUT: `-Lstdlib/graphics/glut -lsapphire_glut -lglut -lGL -lGLU`
- Vulkan: `-Lstdlib/graphics/vulkan -lsapphire_vulkan -lvulkan -lSDL2`

## Notes

- All libraries are built with C++17 standard
- Libraries provide both C++ and C APIs
- C API functions are prefixed with `sapphire_<lib>_`
- Libraries are position-independent code (PIC) for shared library support
- Vulkan implementation is simplified - see `examples/vulkan_full_renderer.cpp` for complete implementation

## Troubleshooting

### Library not found
Make sure libraries are built and in the correct location:
```bash
ls -la stdlib/graphics/*/lib*.a
ls -la stdlib/graphics/*/lib*.so
```

### Missing symbols
Ensure all dependencies are linked:
```bash
ldd my_program  # Check linked libraries
nm -D libsapphire_sdl2.so  # Check exported symbols
```

### Compilation errors
Check include paths:
```bash
g++ -v  # Show default include paths
pkg-config --cflags sdl2  # Show SDL2 include flags
```

---

**Status**: Ready for integration  
**Date**: March 8, 2026  
**Version**: 1.0
