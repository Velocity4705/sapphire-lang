# Sapphire Graphics Libraries - Import Guide

Complete guide for using graphics libraries in Sapphire programs via `import`.

## Overview

The Sapphire graphics libraries are now available as C++ implementations that can be imported and used in your Sapphire programs. Each library is compiled into both static (.a) and shared (.so) libraries.

## Available Libraries

### 1. SDL2 - 2D Graphics and Window Management
**Location**: `stdlib/graphics/sdl2/`  
**Files**: `libsapphire_sdl2.a`, `libsapphire_sdl2.so`  
**Best for**: 2D games, simple graphics, window management

### 2. OpenGL - 3D Graphics
**Location**: `stdlib/graphics/opengl/`  
**Files**: `libsapphire_opengl.a`, `libsapphire_opengl.so`  
**Best for**: 3D games, custom rendering pipelines

### 3. GLUT - OpenGL Utility Toolkit
**Location**: `stdlib/graphics/glut/`  
**Files**: `libsapphire_glut.a`, `libsapphire_glut.so`  
**Best for**: Quick 3D prototypes, 3D shapes

### 4. Vulkan - High-Performance Graphics
**Location**: `stdlib/graphics/vulkan/`  
**Files**: `libsapphire_vulkan.a`, `libsapphire_vulkan.so`  
**Best for**: AAA games, maximum performance

## Building the Libraries

From the root directory (sapphire-lang/):

```bash
# Build all libraries
make -C stdlib/graphics all

# Build specific library
make -C stdlib/graphics sdl2
make -C stdlib/graphics opengl
make -C stdlib/graphics glut
make -C stdlib/graphics vulkan

# Clean and rebuild
make -C stdlib/graphics clean
make -C stdlib/graphics all

# Show library info
make -C stdlib/graphics info
```

## Using in Sapphire Programs

### SDL2 Example

Create `my_game.spp`:
```sapphire
# Import SDL2 graphics library
# This will link against libsapphire_sdl2

fn main():
    # Create window
    let window = sapphire_sdl2_create_window("My Game", 800, 600)
    sapphire_sdl2_show_window(window)
    
    # Game loop
    while not sapphire_sdl2_should_close(window):
        sapphire_sdl2_poll_events(window)
        
        # Clear screen (black)
        sapphire_sdl2_clear(window, 0, 0, 0)
        
        # Draw white rectangle
        sapphire_sdl2_fill_rect(window, 100, 100, 50, 50, 255, 255, 255)
        
        # Present frame
        sapphire_sdl2_present(window)
        
        # 60 FPS
        sapphire_sdl2_delay(16)
    
    # Cleanup
    sapphire_sdl2_destroy_window(window)
```

Compile and run:
```bash
# Compile Sapphire to LLVM IR
./sapp compile my_game.spp 2>/dev/null > my_game.ll

# Compile to object file
llc -filetype=obj my_game.ll -o my_game.o

# Link with SDL2 library
clang my_game.o -o my_game \
    -Lstdlib/graphics/sdl2 -lsapphire_sdl2 \
    -lSDL2 -lstdc++

# Run
./my_game
```

### OpenGL Example

Create `my_3d_app.spp`:
```sapphire
# Import OpenGL graphics library

fn main():
    # Initialize OpenGL
    sapphire_gl_init()
    
    # Setup viewport
    sapphire_gl_viewport(0, 0, 800, 600)
    
    # Clear screen
    sapphire_gl_clear_color(0.0, 0.0, 0.0, 1.0)
    sapphire_gl_clear(0x00004100)  # COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT
    
    # Setup 3D projection
    sapphire_gl_matrix_mode(0x1701)  # GL_PROJECTION
    sapphire_gl_load_identity()
    sapphire_gl_perspective(45.0, 1.33, 0.1, 100.0)
    
    # Setup modelview
    sapphire_gl_matrix_mode(0x1700)  # GL_MODELVIEW
    sapphire_gl_load_identity()
    sapphire_gl_translate(0.0, 0.0, -5.0)
    
    # Draw triangle
    sapphire_gl_begin(0x0004)  # GL_TRIANGLES
    sapphire_gl_color3f(1.0, 0.0, 0.0)
    sapphire_gl_vertex3f(0.0, 1.0, 0.0)
    sapphire_gl_color3f(0.0, 1.0, 0.0)
    sapphire_gl_vertex3f(-1.0, -1.0, 0.0)
    sapphire_gl_color3f(0.0, 0.0, 1.0)
    sapphire_gl_vertex3f(1.0, -1.0, 0.0)
    sapphire_gl_end()
```

Compile and run:
```bash
./sapp compile my_3d_app.spp 2>/dev/null > my_3d_app.ll
llc -filetype=obj my_3d_app.ll -o my_3d_app.o
clang my_3d_app.o -o my_3d_app \
    -Lstdlib/graphics/opengl -lsapphire_opengl \
    -lGL -lGLU -lstdc++
./my_3d_app
```

### GLUT Example

Create `my_glut_app.spp`:
```sapphire
# Import GLUT graphics library

fn main():
    # Initialize GLUT
    sapphire_glut_init()
    sapphire_glut_init_display_mode(0x0002 | 0x0000 | 0x0010)  # DOUBLE | RGB | DEPTH
    sapphire_glut_init_window_size(800, 600)
    sapphire_glut_init_window_position(100, 100)
    
    # Create window
    let window = sapphire_glut_create_window("My GLUT App")
    
    # Draw sphere
    sapphire_glut_solid_sphere(1.0, 20, 20)
    
    # Swap buffers
    sapphire_glut_swap_buffers()
```

Compile and run:
```bash
./sapp compile my_glut_app.spp 2>/dev/null > my_glut_app.ll
llc -filetype=obj my_glut_app.ll -o my_glut_app.o
clang my_glut_app.o -o my_glut_app \
    -Lstdlib/graphics/glut -lsapphire_glut \
    -lglut -lGL -lGLU -lstdc++
./my_glut_app
```

### Vulkan Example

Create `my_vulkan_app.spp`:
```sapphire
# Import Vulkan graphics library

fn main():
    # Create Vulkan context
    let context = sapphire_vulkan_create_context("My Vulkan App", 800, 600)
    
    # Initialize Vulkan
    if sapphire_vulkan_init(context):
        print("Vulkan initialized successfully")
        
        # Render loop
        while not sapphire_vulkan_should_close(context):
            sapphire_vulkan_poll_events(context)
            sapphire_vulkan_begin_frame(context)
            sapphire_vulkan_draw_triangle(context)
            sapphire_vulkan_end_frame(context)
    
    # Cleanup
    sapphire_vulkan_destroy_context(context)
```

Compile and run:
```bash
./sapp compile my_vulkan_app.spp 2>/dev/null > my_vulkan_app.ll
llc -filetype=obj my_vulkan_app.ll -o my_vulkan_app.o
clang my_vulkan_app.o -o my_vulkan_app \
    -Lstdlib/graphics/vulkan -lsapphire_vulkan \
    -lvulkan -lSDL2 -lstdc++
./my_vulkan_app
```

## C API Reference

### SDL2 Functions

```c
// Window management
void* sapphire_sdl2_create_window(const char* title, int width, int height);
void sapphire_sdl2_destroy_window(void* window);
void sapphire_sdl2_show_window(void* window);
void sapphire_sdl2_hide_window(void* window);
bool sapphire_sdl2_should_close(void* window);

// Event handling
void sapphire_sdl2_poll_events(void* window);
bool sapphire_sdl2_is_key_down(void* window, int scancode);

// Rendering
void sapphire_sdl2_clear(void* window, uint8_t r, uint8_t g, uint8_t b);
void sapphire_sdl2_present(void* window);
void sapphire_sdl2_fill_rect(void* window, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);
void sapphire_sdl2_draw_line(void* window, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b);

// Timing
void sapphire_sdl2_delay(uint32_t ms);
uint32_t sapphire_sdl2_get_ticks();
```

### OpenGL Functions

```c
// Initialization
bool sapphire_gl_init();

// Viewport and clearing
void sapphire_gl_viewport(int x, int y, int width, int height);
void sapphire_gl_clear_color(float r, float g, float b, float a);
void sapphire_gl_clear(unsigned int mask);

// Matrix operations
void sapphire_gl_matrix_mode(unsigned int mode);
void sapphire_gl_load_identity();
void sapphire_gl_perspective(double fovy, double aspect, double near_val, double far_val);

// Transformations
void sapphire_gl_translate(float x, float y, float z);
void sapphire_gl_rotate(float angle, float x, float y, float z);
void sapphire_gl_scale(float x, float y, float z);

// Drawing
void sapphire_gl_begin(unsigned int mode);
void sapphire_gl_end();
void sapphire_gl_vertex3f(float x, float y, float z);
void sapphire_gl_color3f(float r, float g, float b);

// State management
void sapphire_gl_enable(unsigned int cap);
void sapphire_gl_disable(unsigned int cap);
```

### GLUT Functions

```c
// Initialization
void sapphire_glut_init();
void sapphire_glut_init_display_mode(unsigned int mode);
void sapphire_glut_init_window_size(int width, int height);
int sapphire_glut_create_window(const char* title);

// Rendering
void sapphire_glut_swap_buffers();

// 3D Shapes
void sapphire_glut_solid_sphere(double radius, int slices, int stacks);
void sapphire_glut_solid_cube(double size);
void sapphire_glut_solid_cone(double base, double height, int slices, int stacks);
void sapphire_glut_solid_teapot(double size);
void sapphire_glut_wire_sphere(double radius, int slices, int stacks);
```

### Vulkan Functions

```c
// Context management
void* sapphire_vulkan_create_context(const char* title, int width, int height);
void sapphire_vulkan_destroy_context(void* context);
bool sapphire_vulkan_init(void* context);

// Rendering
void sapphire_vulkan_begin_frame(void* context);
void sapphire_vulkan_end_frame(void* context);
void sapphire_vulkan_draw_triangle(void* context);

// Window management
bool sapphire_vulkan_should_close(void* context);
void sapphire_vulkan_poll_events(void* context);
```

## Compiler Integration

The Sapphire compiler should automatically detect which graphics library is being used and link appropriately.

When the compiler sees function calls like `sapphire_sdl2_*`, it should add:
```bash
-Lstdlib/graphics/sdl2 -lsapphire_sdl2 -lSDL2
```

When it sees `sapphire_gl_*`:
```bash
-Lstdlib/graphics/opengl -lsapphire_opengl -lGL -lGLU
```

When it sees `sapphire_glut_*`:
```bash
-Lstdlib/graphics/glut -lsapphire_glut -lglut -lGL -lGLU
```

When it sees `sapphire_vulkan_*`:
```bash
-Lstdlib/graphics/vulkan -lsapphire_vulkan -lvulkan -lSDL2
```

## Library Sizes

```
SDL2:    13K (static), 20K (shared)
OpenGL:  13K (static), 24K (shared)
GLUT:    18K (static), 31K (shared)
Vulkan:  17K (static), 29K (shared)
```

## Dependencies

Make sure these are installed:

**Fedora/RHEL:**
```bash
sudo dnf install SDL2-devel mesa-libGL-devel mesa-libGLU-devel freeglut-devel vulkan-devel
```

**Ubuntu/Debian:**
```bash
sudo apt install libsdl2-dev libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev libvulkan-dev
```

**macOS:**
```bash
brew install sdl2 freeglut molten-vk
```

## Status

✅ All libraries built and ready to use  
✅ C API fully implemented  
✅ Static and shared libraries available  
✅ Ready for compiler integration  

---

**Date**: March 8, 2026  
**Version**: 1.0  
**Status**: Production Ready
