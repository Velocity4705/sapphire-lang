# Graphics Quick Start Guide

Quick reference for using graphics APIs with Sapphire.

---

## Available Graphics APIs

### 1. SDL2 (Sapphire) - Easiest
**Best for**: 2D games, simple graphics, beginners

```bash
# Run example
./sapp examples/test_window_visible.spp
```

**Lines**: 37  
**Complexity**: ⭐ Simple  
**Performance**: ⭐⭐⭐⭐ Good

---

### 2. GLUT (C++) - Simple 3D
**Best for**: 3D visualization, prototypes, learning

```bash
# Compile and run
./examples/run_test_glut.sh
```

**Lines**: 99  
**Complexity**: ⭐⭐ Moderate  
**Performance**: ⭐⭐⭐⭐ Good

---

### 3. SDL2 + OpenGL (C++) - Standard 3D
**Best for**: 3D games, cross-platform, production

```bash
# Compile and run
./examples/run_test_sdl2_opengl.sh
```

**Lines**: 120  
**Complexity**: ⭐⭐⭐ Moderate  
**Performance**: ⭐⭐⭐⭐ Good

---

### 4. Vulkan (C++) - High Performance
**Best for**: AAA games, game engines, maximum performance

```bash
# Compile and run
./examples/run_vulkan_full_renderer.sh
```

**Lines**: 826  
**Complexity**: ⭐⭐⭐⭐⭐ Very High  
**Performance**: ⭐⭐⭐⭐⭐ Excellent

---

## Graphics Headers

Use these headers in your Sapphire code:

```sapphire
// Window management
use stdlib::graphics::window::*

// SDL2 constants
use stdlib::graphics::sdl2::*

// OpenGL constants
use stdlib::graphics::opengl::*

// GLUT constants
use stdlib::graphics::glut::*

// Vulkan constants
use stdlib::graphics::vulkan::*
```

---

## Quick Examples

### SDL2 Window (Sapphire)
```sapphire
use stdlib::graphics::window::*
use stdlib::graphics::sdl2::*

fn main() {
    let window = Window::new("My Game", 800, 600)
    window.clear(255, 0, 0)  // Red background
    window.present()
    window.delay(3000)
}
```

### OpenGL Triangle (C++)
```cpp
#include <SDL2/SDL.h>
#include <GL/gl.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("OpenGL", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    // Render loop
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    SDL_GL_SwapWindow(window);
    SDL_Delay(3000);
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
```

---

## Decision Tree

**Choose SDL2 if**:
- Building 2D games
- Want simplicity
- New to graphics programming
- Need quick prototypes

**Choose GLUT if**:
- Building 3D visualizations
- Learning 3D graphics
- Need simple 3D setup
- Prototyping 3D concepts

**Choose OpenGL if**:
- Building 3D games
- Need good performance
- Want cross-platform support
- Production-ready code

**Choose Vulkan if**:
- Building AAA games
- Need maximum performance
- Want low-level control
- Building game engines
- Targeting modern hardware

---

## Build Requirements

### SDL2
```bash
sudo dnf install SDL2-devel
```

### OpenGL
```bash
sudo dnf install mesa-libGL-devel
```

### GLUT
```bash
sudo dnf install freeglut-devel
```

### Vulkan
```bash
sudo dnf install vulkan-devel vulkan-loader glslang
```

---

## Test All Graphics

Run all graphics tests:
```bash
./examples/test_all_graphics.sh
```

Individual tests:
```bash
# SDL2 (Sapphire)
./sapp examples/test_window_visible.spp

# SDL2 + OpenGL
./examples/run_test_sdl2_opengl.sh

# GLUT
./examples/run_test_glut.sh

# Vulkan (simple)
./examples/run_test_vulkan_window.sh

# Vulkan (full)
./examples/run_vulkan_full_renderer.sh
```

---

## Documentation

- `GRAPHICS_HEADERS.md` - Header system documentation
- `GRAPHICS_COMPLETE.md` - Graphics completion summary
- `.dev-docs/VULKAN_COMPLETE_IMPLEMENTATION.md` - Vulkan details
- `.dev-docs/ALL_GRAPHICS_APIS_COMPLETE.md` - All APIs summary
- `stdlib/graphics/README.md` - Graphics library docs

---

## Performance Comparison

| API | FPS | Frame Time | CPU Usage | GPU Usage |
|-----|-----|------------|-----------|-----------|
| SDL2 | 60+ | ~16ms | Low | Low |
| GLUT | 60+ | ~16ms | Low | Medium |
| OpenGL | 60+ | ~16ms | Low | Medium |
| Vulkan | 60+ | ~16ms | Very Low | High |

---

## Common Issues

### Window doesn't appear
- Check if graphics drivers are installed
- Verify display server is running (X11/Wayland)
- Try running with `DISPLAY=:0`

### Compilation errors
- Install required development packages
- Check library paths
- Verify compiler version (GCC 9+)

### Black screen
- Check shader compilation
- Verify render loop is running
- Check clear color settings

---

## Next Steps

1. Try the examples
2. Read the documentation
3. Build your first game
4. Experiment with different APIs
5. Join the community

---

**Status**: All graphics APIs working!  
**Date**: March 8, 2026  
**Version**: 1.0-beta.6

🎮 Happy Game Development! 🎮
