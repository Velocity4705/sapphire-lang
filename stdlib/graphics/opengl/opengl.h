#ifndef SAPPHIRE_STDLIB_OPENGL_H
#define SAPPHIRE_STDLIB_OPENGL_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdint>

namespace sapphire {
namespace stdlib {
namespace graphics {

/**
 * OpenGL Graphics Library for Sapphire
 * 
 * Provides OpenGL 2.1+ 3D graphics capabilities.
 * Use with SDL2 or GLUT for window management.
 */

class OpenGL {
public:
    // Initialization
    static bool init();
    
    // Viewport and clearing
    static void viewport(int x, int y, int width, int height);
    static void clear_color(float r, float g, float b, float a);
    static void clear(unsigned int mask);
    
    // Matrix operations
    static void matrix_mode(unsigned int mode);
    static void load_identity();
    static void ortho(double left, double right, double bottom, double top, double near_val, double far_val);
    static void perspective(double fovy, double aspect, double near_val, double far_val);
    static void frustum(double left, double right, double bottom, double top, double near_val, double far_val);
    
    // Transformations
    static void translate(float x, float y, float z);
    static void rotate(float angle, float x, float y, float z);
    static void scale(float x, float y, float z);
    static void push_matrix();
    static void pop_matrix();
    
    // Drawing primitives
    static void begin(unsigned int mode);
    static void end();
    static void vertex2f(float x, float y);
    static void vertex3f(float x, float y, float z);
    static void color3f(float r, float g, float b);
    static void color4f(float r, float g, float b, float a);
    static void normal3f(float x, float y, float z);
    static void tex_coord2f(float s, float t);
    
    // State management
    static void enable(unsigned int cap);
    static void disable(unsigned int cap);
    static void depth_func(unsigned int func);
    static void blend_func(unsigned int sfactor, unsigned int dfactor);
    static void cull_face(unsigned int mode);
    
    // Lighting
    static void light(unsigned int light, unsigned int pname, const float* params);
    static void material(unsigned int face, unsigned int pname, const float* params);
};

} // namespace graphics
} // namespace stdlib
} // namespace sapphire

// C API for Sapphire code generation
extern "C" {
    // Initialization
    bool sapphire_gl_init();
    
    // Viewport and clearing
    void sapphire_gl_viewport(int x, int y, int width, int height);
    void sapphire_gl_clear_color(float r, float g, float b, float a);
    void sapphire_gl_clear(unsigned int mask);
    
    // Matrix operations
    void sapphire_gl_matrix_mode(unsigned int mode);
    void sapphire_gl_load_identity();
    void sapphire_gl_ortho(double left, double right, double bottom, double top, double near_val, double far_val);
    void sapphire_gl_perspective(double fovy, double aspect, double near_val, double far_val);
    void sapphire_gl_frustum(double left, double right, double bottom, double top, double near_val, double far_val);
    
    // Transformations
    void sapphire_gl_translate(float x, float y, float z);
    void sapphire_gl_rotate(float angle, float x, float y, float z);
    void sapphire_gl_scale(float x, float y, float z);
    void sapphire_gl_push_matrix();
    void sapphire_gl_pop_matrix();
    
    // Drawing primitives
    void sapphire_gl_begin(unsigned int mode);
    void sapphire_gl_end();
    void sapphire_gl_vertex2f(float x, float y);
    void sapphire_gl_vertex3f(float x, float y, float z);
    void sapphire_gl_color3f(float r, float g, float b);
    void sapphire_gl_color4f(float r, float g, float b, float a);
    void sapphire_gl_normal3f(float x, float y, float z);
    void sapphire_gl_tex_coord2f(float s, float t);
    
    // State management
    void sapphire_gl_enable(unsigned int cap);
    void sapphire_gl_disable(unsigned int cap);
    void sapphire_gl_depth_func(unsigned int func);
    void sapphire_gl_blend_func(unsigned int sfactor, unsigned int dfactor);
    void sapphire_gl_cull_face(unsigned int mode);
}

#endif // SAPPHIRE_STDLIB_OPENGL_H
