# OpenGL and GLUT API Reference for Sapphire

Built-in OpenGL and GLUT functions for 3D graphics in Sapphire.

## Status

- ✅ **OpenGL**: Fully integrated into compiler
- ✅ **GLUT**: Fully integrated into compiler
- 🔴 **Vulkan**: Planned for future

## OpenGL Functions

### Clearing and Colors

#### `gl_clear_color(r, g, b, a)`
Sets the clear color for the color buffer.

**Parameters:**
- `r, g, b, a` - Red, green, blue, alpha values (0.0 to 1.0)

**Example:**
```sapphire
gl_clear_color(0, 0, 0, 1)  # Black background
```

#### `gl_clear(mask)`
Clears buffers to preset values.

**Parameters:**
- `mask` - Bitwise OR of buffer masks

**Constants:**
- `GL_COLOR_BUFFER_BIT = 16384`
- `GL_DEPTH_BUFFER_BIT = 256`

**Example:**
```sapphire
let GL_COLOR_BUFFER_BIT = 16384
gl_clear(GL_COLOR_BUFFER_BIT)
```

### Drawing Primitives

#### `gl_begin(mode)`
Begins drawing primitives.

**Parameters:**
- `mode` - Primitive type

**Constants:**
- `GL_POINTS = 0`
- `GL_LINES = 1`
- `GL_TRIANGLES = 4`
- `GL_QUADS = 7`

**Example:**
```sapphire
let GL_TRIANGLES = 4
gl_begin(GL_TRIANGLES)
```

#### `gl_end()`
Ends drawing primitives.

**Example:**
```sapphire
gl_end()
```

#### `gl_vertex3f(x, y, z)`
Specifies a vertex position.

**Parameters:**
- `x, y, z` - Vertex coordinates

**Example:**
```sapphire
gl_vertex3f(0, 1, 0)
```

#### `gl_color3f(r, g, b)`
Sets the current color.

**Parameters:**
- `r, g, b` - Red, green, blue values (0.0 to 1.0)

**Example:**
```sapphire
gl_color3f(1, 0, 0)  # Red
```

### Transformations

#### `gl_translatef(x, y, z)`
Translates the current matrix.

**Parameters:**
- `x, y, z` - Translation amounts

**Example:**
```sapphire
gl_translatef(0, 0, 0 - 5)  # Move back 5 units
```

#### `gl_rotatef(angle, x, y, z)`
Rotates the current matrix.

**Parameters:**
- `angle` - Rotation angle in degrees
- `x, y, z` - Rotation axis

**Example:**
```sapphire
gl_rotatef(45, 0, 1, 0)  # Rotate 45° around Y axis
```

### Matrix Operations

#### `gl_matrix_mode(mode)`
Specifies which matrix is the current matrix.

**Parameters:**
- `mode` - Matrix mode

**Constants:**
- `GL_MODELVIEW = 5888`
- `GL_PROJECTION = 5889`

**Example:**
```sapphire
let GL_PROJECTION = 5889
gl_matrix_mode(GL_PROJECTION)
```

#### `gl_load_identity()`
Replaces the current matrix with the identity matrix.

**Example:**
```sapphire
gl_load_identity()
```

---

## GLUT Functions

### Initialization

#### `glut_init_display_mode(mode)`
Sets the initial display mode.

**Parameters:**
- `mode` - Display mode flags (bitwise OR)

**Constants:**
- `GLUT_RGB = 0`
- `GLUT_DOUBLE = 2`
- `GLUT_DEPTH = 16`

**Example:**
```sapphire
let GLUT_DOUBLE = 2
let GLUT_RGB = 0
let GLUT_DEPTH = 16
glut_init_display_mode(GLUT_DOUBLE + GLUT_RGB + GLUT_DEPTH)
```

#### `glut_init_window_size(width, height)`
Sets the initial window size.

**Parameters:**
- `width, height` - Window dimensions in pixels

**Example:**
```sapphire
glut_init_window_size(800, 600)
```

#### `glut_create_window(title)`
Creates a window with the specified title.

**Parameters:**
- `title` - Window title string

**Returns:** Window ID (integer)

**Example:**
```sapphire
let window = glut_create_window("My 3D App")
```

### Rendering

#### `glut_swap_buffers()`
Swaps the front and back buffers (for double buffering).

**Example:**
```sapphire
glut_swap_buffers()
```

### 3D Shapes

#### `glut_solid_sphere(radius, slices, stacks)`
Draws a solid sphere.

**Parameters:**
- `radius` - Sphere radius
- `slices` - Number of subdivisions around Z axis
- `stacks` - Number of subdivisions along Z axis

**Example:**
```sapphire
glut_solid_sphere(1, 30, 30)
```

#### `glut_wire_sphere(radius, slices, stacks)`
Draws a wireframe sphere.

**Parameters:**
- `radius` - Sphere radius
- `slices` - Number of subdivisions around Z axis
- `stacks` - Number of subdivisions along Z axis

**Example:**
```sapphire
glut_wire_sphere(1, 20, 20)
```

---

## Complete Example: Rotating Triangle

```sapphire
# OpenGL/GLUT Triangle Example (Conceptual)
# Note: This shows the API, but GLUT main loop requires callbacks
# which aren't yet supported in Sapphire

# Constants
let GL_COLOR_BUFFER_BIT = 16384
let GL_TRIANGLES = 4
let GL_PROJECTION = 5889
let GL_MODELVIEW = 5888
let GLUT_DOUBLE = 2
let GLUT_RGB = 0

# Initialize GLUT
glut_init_display_mode(GLUT_DOUBLE + GLUT_RGB)
glut_init_window_size(800, 600)
let window = glut_create_window("Triangle")

# Setup projection
gl_matrix_mode(GL_PROJECTION)
gl_load_identity()

# Setup modelview
gl_matrix_mode(GL_MODELVIEW)
gl_load_identity()
gl_translatef(0, 0, 0 - 5)

# Clear and draw
gl_clear_color(0, 0, 0, 1)
gl_clear(GL_COLOR_BUFFER_BIT)

gl_begin(GL_TRIANGLES)
gl_color3f(1, 0, 0)
gl_vertex3f(0, 1, 0)
gl_color3f(0, 1, 0)
gl_vertex3f(0 - 1, 0 - 1, 0)
gl_color3f(0, 0, 1)
gl_vertex3f(1, 0 - 1, 0)
gl_end()

glut_swap_buffers()
```

---

## Compilation

### With OpenGL
```bash
# Compile Sapphire to LLVM IR
./sapp compile program.spp 2>/dev/null > program.ll

# Compile to object file
llc -filetype=obj program.ll -o program.o

# Link with OpenGL
clang program.o -o program -lGL -lGLU -lstdc++

# Run
./program
```

### With GLUT
```bash
# Compile Sapphire to LLVM IR
./sapp compile program.spp 2>/dev/null > program.ll

# Compile to object file
llc -filetype=obj program.ll -o program.o

# Link with GLUT (includes OpenGL)
clang program.o -o program -lglut -lGL -lGLU -lstdc++

# Run
./program
```

---

## OpenGL Constants Reference

### Buffer Bits
```sapphire
let GL_COLOR_BUFFER_BIT = 16384
let GL_DEPTH_BUFFER_BIT = 256
let GL_STENCIL_BUFFER_BIT = 1024
```

### Primitive Types
```sapphire
let GL_POINTS = 0
let GL_LINES = 1
let GL_LINE_LOOP = 2
let GL_LINE_STRIP = 3
let GL_TRIANGLES = 4
let GL_TRIANGLE_STRIP = 5
let GL_TRIANGLE_FAN = 6
let GL_QUADS = 7
```

### Matrix Modes
```sapphire
let GL_MODELVIEW = 5888
let GL_PROJECTION = 5889
let GL_TEXTURE = 5890
```

### GLUT Display Modes
```sapphire
let GLUT_RGB = 0
let GLUT_RGBA = 0
let GLUT_SINGLE = 0
let GLUT_DOUBLE = 2
let GLUT_DEPTH = 16
let GLUT_STENCIL = 32
let GLUT_MULTISAMPLE = 128
```

---

## Limitations

### Current Limitations
1. **No Callbacks**: GLUT callbacks (display, keyboard, etc.) not yet supported
2. **No Main Loop**: `glutMainLoop()` requires callback support
3. **Limited Shapes**: Only sphere functions implemented
4. **No Textures**: Texture functions not yet added

### Workarounds
- Use SDL2 for window management + OpenGL for rendering
- Implement game loop manually with SDL2
- Use C++ wrapper for complex GLUT applications

---

## Future Additions

### Planned OpenGL Functions
- `gl_enable()` / `gl_disable()`
- `gl_depth_func()`
- `gl_blend_func()`
- `gl_viewport()`
- `gl_ortho()` / `gl_perspective()`
- Texture functions
- Lighting functions

### Planned GLUT Functions
- `glut_main_loop()` (requires callback support)
- `glut_post_redisplay()`
- `glut_solid_cube()` / `glut_wire_cube()`
- `glut_solid_torus()` / `glut_wire_torus()`
- Callback registration functions

### Planned Vulkan Functions
- Instance and device management
- Swapchain creation
- Pipeline creation
- Command buffer recording
- Compute shader dispatch

---

## Best Practices

### For 2D Graphics
Use SDL2 - it's simpler and fully integrated.

### For 3D Graphics
Use OpenGL functions with SDL2 for window management:
1. Create window with SDL2
2. Use OpenGL for 3D rendering
3. Handle input with SDL2

### For Educational 3D
Use GLUT utility shapes for quick prototypes.

### For Maximum Performance
Wait for Vulkan support (or use C++ for now).

---

## Examples

### Simple Triangle (OpenGL)
See: `examples/test_opengl.spp`

### Black Hole Simulation (OpenGL)
See: `examples/challenges/round3_field_level/blackhole_opengl.cpp` (C++ reference)

### Black Hole Simulation (GLUT)
See: `examples/challenges/round3_field_level/blackhole_glut.cpp` (C++ reference)

---

**Status**: OpenGL and GLUT fully integrated into Sapphire compiler!  
**Date**: March 8, 2026  
**Version**: Sapphire v1.0 with Full Graphics Support
