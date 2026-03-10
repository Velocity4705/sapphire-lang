#ifndef SAPPHIRE_STDLIB_GLUT_H
#define SAPPHIRE_STDLIB_GLUT_H

#include <GL/glut.h>
#include <string>

namespace sapphire {
namespace stdlib {
namespace graphics {

/**
 * GLUT Graphics Library for Sapphire
 * 
 * Provides GLUT (OpenGL Utility Toolkit) for 3D graphics and window management.
 * Use this for quick 3D prototypes and visualizations.
 */

class GLUT {
public:
    // Initialization
    static void init(int* argc, char** argv);
    static void init_display_mode(unsigned int mode);
    static void init_window_size(int width, int height);
    static void init_window_position(int x, int y);
    static int create_window(const char* title);
    
    // Main loop
    static void main_loop();
    static void post_redisplay();
    
    // Window management
    static void set_window(int win);
    static int get_window();
    static void set_window_title(const char* title);
    static void set_icon_title(const char* title);
    static void position_window(int x, int y);
    static void reshape_window(int width, int height);
    static void full_screen();
    static void pop_window();
    static void push_window();
    static void show_window();
    static void hide_window();
    static void iconify_window();
    static void destroy_window(int win);
    
    // Rendering
    static void swap_buffers();
    
    // 3D Shapes - Solid
    static void solid_sphere(double radius, int slices, int stacks);
    static void solid_cube(double size);
    static void solid_cone(double base, double height, int slices, int stacks);
    static void solid_torus(double inner_radius, double outer_radius, int sides, int rings);
    static void solid_teapot(double size);
    static void solid_octahedron();
    static void solid_dodecahedron();
    static void solid_icosahedron();
    static void solid_tetrahedron();
    
    // 3D Shapes - Wireframe
    static void wire_sphere(double radius, int slices, int stacks);
    static void wire_cube(double size);
    static void wire_cone(double base, double height, int slices, int stacks);
    static void wire_torus(double inner_radius, double outer_radius, int sides, int rings);
    static void wire_teapot(double size);
    static void wire_octahedron();
    static void wire_dodecahedron();
    static void wire_icosahedron();
    static void wire_tetrahedron();
    
    // Timing
    static int get_time();
};

} // namespace graphics
} // namespace stdlib
} // namespace sapphire

// C API for Sapphire code generation
extern "C" {
    // Initialization
    void sapphire_glut_init();
    void sapphire_glut_init_display_mode(unsigned int mode);
    void sapphire_glut_init_window_size(int width, int height);
    void sapphire_glut_init_window_position(int x, int y);
    int sapphire_glut_create_window(const char* title);
    
    // Main loop
    void sapphire_glut_main_loop();
    void sapphire_glut_post_redisplay();
    
    // Window management
    void sapphire_glut_set_window(int win);
    int sapphire_glut_get_window();
    void sapphire_glut_set_window_title(const char* title);
    void sapphire_glut_set_icon_title(const char* title);
    void sapphire_glut_position_window(int x, int y);
    void sapphire_glut_reshape_window(int width, int height);
    void sapphire_glut_full_screen();
    void sapphire_glut_pop_window();
    void sapphire_glut_push_window();
    void sapphire_glut_show_window();
    void sapphire_glut_hide_window();
    void sapphire_glut_iconify_window();
    void sapphire_glut_destroy_window(int win);
    
    // Rendering
    void sapphire_glut_swap_buffers();
    
    // 3D Shapes - Solid
    void sapphire_glut_solid_sphere(double radius, int slices, int stacks);
    void sapphire_glut_solid_cube(double size);
    void sapphire_glut_solid_cone(double base, double height, int slices, int stacks);
    void sapphire_glut_solid_torus(double inner_radius, double outer_radius, int sides, int rings);
    void sapphire_glut_solid_teapot(double size);
    void sapphire_glut_solid_octahedron();
    void sapphire_glut_solid_dodecahedron();
    void sapphire_glut_solid_icosahedron();
    void sapphire_glut_solid_tetrahedron();
    
    // 3D Shapes - Wireframe
    void sapphire_glut_wire_sphere(double radius, int slices, int stacks);
    void sapphire_glut_wire_cube(double size);
    void sapphire_glut_wire_cone(double base, double height, int slices, int stacks);
    void sapphire_glut_wire_torus(double inner_radius, double outer_radius, int sides, int rings);
    void sapphire_glut_wire_teapot(double size);
    void sapphire_glut_wire_octahedron();
    void sapphire_glut_wire_dodecahedron();
    void sapphire_glut_wire_icosahedron();
    void sapphire_glut_wire_tetrahedron();
    
    // Timing
    int sapphire_glut_get_time();
}

#endif // SAPPHIRE_STDLIB_GLUT_H
