#include "glut.h"

namespace sapphire {
namespace stdlib {
namespace graphics {

void GLUT::init(int* argc, char** argv) {
    glutInit(argc, argv);
}

void GLUT::init_display_mode(unsigned int mode) {
    glutInitDisplayMode(mode);
}

void GLUT::init_window_size(int width, int height) {
    glutInitWindowSize(width, height);
}

void GLUT::init_window_position(int x, int y) {
    glutInitWindowPosition(x, y);
}

int GLUT::create_window(const char* title) {
    return glutCreateWindow(title);
}

void GLUT::main_loop() {
    glutMainLoop();
}

void GLUT::post_redisplay() {
    glutPostRedisplay();
}

void GLUT::set_window(int win) {
    glutSetWindow(win);
}

int GLUT::get_window() {
    return glutGetWindow();
}

void GLUT::set_window_title(const char* title) {
    glutSetWindowTitle(title);
}

void GLUT::set_icon_title(const char* title) {
    glutSetIconTitle(title);
}

void GLUT::position_window(int x, int y) {
    glutPositionWindow(x, y);
}

void GLUT::reshape_window(int width, int height) {
    glutReshapeWindow(width, height);
}

void GLUT::full_screen() {
    glutFullScreen();
}

void GLUT::pop_window() {
    glutPopWindow();
}

void GLUT::push_window() {
    glutPushWindow();
}

void GLUT::show_window() {
    glutShowWindow();
}

void GLUT::hide_window() {
    glutHideWindow();
}

void GLUT::iconify_window() {
    glutIconifyWindow();
}

void GLUT::destroy_window(int win) {
    glutDestroyWindow(win);
}

void GLUT::swap_buffers() {
    glutSwapBuffers();
}

void GLUT::solid_sphere(double radius, int slices, int stacks) {
    glutSolidSphere(radius, slices, stacks);
}

void GLUT::solid_cube(double size) {
    glutSolidCube(size);
}

void GLUT::solid_cone(double base, double height, int slices, int stacks) {
    glutSolidCone(base, height, slices, stacks);
}

void GLUT::solid_torus(double inner_radius, double outer_radius, int sides, int rings) {
    glutSolidTorus(inner_radius, outer_radius, sides, rings);
}

void GLUT::solid_teapot(double size) {
    glutSolidTeapot(size);
}

void GLUT::solid_octahedron() {
    glutSolidOctahedron();
}

void GLUT::solid_dodecahedron() {
    glutSolidDodecahedron();
}

void GLUT::solid_icosahedron() {
    glutSolidIcosahedron();
}

void GLUT::solid_tetrahedron() {
    glutSolidTetrahedron();
}

void GLUT::wire_sphere(double radius, int slices, int stacks) {
    glutWireSphere(radius, slices, stacks);
}

void GLUT::wire_cube(double size) {
    glutWireCube(size);
}

void GLUT::wire_cone(double base, double height, int slices, int stacks) {
    glutWireCone(base, height, slices, stacks);
}

void GLUT::wire_torus(double inner_radius, double outer_radius, int sides, int rings) {
    glutWireTorus(inner_radius, outer_radius, sides, rings);
}

void GLUT::wire_teapot(double size) {
    glutWireTeapot(size);
}

void GLUT::wire_octahedron() {
    glutWireOctahedron();
}

void GLUT::wire_dodecahedron() {
    glutWireDodecahedron();
}

void GLUT::wire_icosahedron() {
    glutWireIcosahedron();
}

void GLUT::wire_tetrahedron() {
    glutWireTetrahedron();
}

int GLUT::get_time() {
    return glutGet(GLUT_ELAPSED_TIME);
}

} // namespace graphics
} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    void sapphire_glut_init() {
        int argc = 0;
        char* argv[] = {nullptr};
        sapphire::stdlib::graphics::GLUT::init(&argc, argv);
    }
    
    void sapphire_glut_init_display_mode(unsigned int mode) {
        sapphire::stdlib::graphics::GLUT::init_display_mode(mode);
    }
    
    void sapphire_glut_init_window_size(int width, int height) {
        sapphire::stdlib::graphics::GLUT::init_window_size(width, height);
    }
    
    void sapphire_glut_init_window_position(int x, int y) {
        sapphire::stdlib::graphics::GLUT::init_window_position(x, y);
    }
    
    int sapphire_glut_create_window(const char* title) {
        return sapphire::stdlib::graphics::GLUT::create_window(title);
    }
    
    void sapphire_glut_main_loop() {
        sapphire::stdlib::graphics::GLUT::main_loop();
    }
    
    void sapphire_glut_post_redisplay() {
        sapphire::stdlib::graphics::GLUT::post_redisplay();
    }
    
    void sapphire_glut_set_window(int win) {
        sapphire::stdlib::graphics::GLUT::set_window(win);
    }
    
    int sapphire_glut_get_window() {
        return sapphire::stdlib::graphics::GLUT::get_window();
    }
    
    void sapphire_glut_set_window_title(const char* title) {
        sapphire::stdlib::graphics::GLUT::set_window_title(title);
    }
    
    void sapphire_glut_set_icon_title(const char* title) {
        sapphire::stdlib::graphics::GLUT::set_icon_title(title);
    }
    
    void sapphire_glut_position_window(int x, int y) {
        sapphire::stdlib::graphics::GLUT::position_window(x, y);
    }
    
    void sapphire_glut_reshape_window(int width, int height) {
        sapphire::stdlib::graphics::GLUT::reshape_window(width, height);
    }
    
    void sapphire_glut_full_screen() {
        sapphire::stdlib::graphics::GLUT::full_screen();
    }
    
    void sapphire_glut_pop_window() {
        sapphire::stdlib::graphics::GLUT::pop_window();
    }
    
    void sapphire_glut_push_window() {
        sapphire::stdlib::graphics::GLUT::push_window();
    }
    
    void sapphire_glut_show_window() {
        sapphire::stdlib::graphics::GLUT::show_window();
    }
    
    void sapphire_glut_hide_window() {
        sapphire::stdlib::graphics::GLUT::hide_window();
    }
    
    void sapphire_glut_iconify_window() {
        sapphire::stdlib::graphics::GLUT::iconify_window();
    }
    
    void sapphire_glut_destroy_window(int win) {
        sapphire::stdlib::graphics::GLUT::destroy_window(win);
    }
    
    void sapphire_glut_swap_buffers() {
        sapphire::stdlib::graphics::GLUT::swap_buffers();
    }
    
    void sapphire_glut_solid_sphere(double radius, int slices, int stacks) {
        sapphire::stdlib::graphics::GLUT::solid_sphere(radius, slices, stacks);
    }
    
    void sapphire_glut_solid_cube(double size) {
        sapphire::stdlib::graphics::GLUT::solid_cube(size);
    }
    
    void sapphire_glut_solid_cone(double base, double height, int slices, int stacks) {
        sapphire::stdlib::graphics::GLUT::solid_cone(base, height, slices, stacks);
    }
    
    void sapphire_glut_solid_torus(double inner_radius, double outer_radius, int sides, int rings) {
        sapphire::stdlib::graphics::GLUT::solid_torus(inner_radius, outer_radius, sides, rings);
    }
    
    void sapphire_glut_solid_teapot(double size) {
        sapphire::stdlib::graphics::GLUT::solid_teapot(size);
    }
    
    void sapphire_glut_solid_octahedron() {
        sapphire::stdlib::graphics::GLUT::solid_octahedron();
    }
    
    void sapphire_glut_solid_dodecahedron() {
        sapphire::stdlib::graphics::GLUT::solid_dodecahedron();
    }
    
    void sapphire_glut_solid_icosahedron() {
        sapphire::stdlib::graphics::GLUT::solid_icosahedron();
    }
    
    void sapphire_glut_solid_tetrahedron() {
        sapphire::stdlib::graphics::GLUT::solid_tetrahedron();
    }
    
    void sapphire_glut_wire_sphere(double radius, int slices, int stacks) {
        sapphire::stdlib::graphics::GLUT::wire_sphere(radius, slices, stacks);
    }
    
    void sapphire_glut_wire_cube(double size) {
        sapphire::stdlib::graphics::GLUT::wire_cube(size);
    }
    
    void sapphire_glut_wire_cone(double base, double height, int slices, int stacks) {
        sapphire::stdlib::graphics::GLUT::wire_cone(base, height, slices, stacks);
    }
    
    void sapphire_glut_wire_torus(double inner_radius, double outer_radius, int sides, int rings) {
        sapphire::stdlib::graphics::GLUT::wire_torus(inner_radius, outer_radius, sides, rings);
    }
    
    void sapphire_glut_wire_teapot(double size) {
        sapphire::stdlib::graphics::GLUT::wire_teapot(size);
    }
    
    void sapphire_glut_wire_octahedron() {
        sapphire::stdlib::graphics::GLUT::wire_octahedron();
    }
    
    void sapphire_glut_wire_dodecahedron() {
        sapphire::stdlib::graphics::GLUT::wire_dodecahedron();
    }
    
    void sapphire_glut_wire_icosahedron() {
        sapphire::stdlib::graphics::GLUT::wire_icosahedron();
    }
    
    void sapphire_glut_wire_tetrahedron() {
        sapphire::stdlib::graphics::GLUT::wire_tetrahedron();
    }
    
    int sapphire_glut_get_time() {
        return sapphire::stdlib::graphics::GLUT::get_time();
    }
}
