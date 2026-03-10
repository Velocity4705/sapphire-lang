#include "glut.h"

namespace sapphire {
namespace stdlib {
namespace graphics {

void GLUTContext::init(int* argc, char** argv) {
    glutInit(argc, argv);
}

void GLUTContext::init_display_mode(unsigned int mode) {
    glutInitDisplayMode(mode);
}

void GLUTContext::init_window_size(int width, int height) {
    glutInitWindowSize(width, height);
}

void GLUTContext::init_window_position(int x, int y) {
    glutInitWindowPosition(x, y);
}

int GLUTContext::create_window(const char* title) {
    return glutCreateWindow(title);
}

void GLUTContext::main_loop() {
    glutMainLoop();
}

void GLUTContext::post_redisplay() {
    glutPostRedisplay();
}

void GLUTContext::display_func(void (*func)()) {
    glutDisplayFunc(func);
}

void GLUTContext::reshape_func(void (*func)(int, int)) {
    glutReshapeFunc(func);
}

void GLUTContext::keyboard_func(void (*func)(unsigned char, int, int)) {
    glutKeyboardFunc(func);
}

void GLUTContext::mouse_func(void (*func)(int, int, int, int)) {
    glutMouseFunc(func);
}

void GLUTContext::motion_func(void (*func)(int, int)) {
    glutMotionFunc(func);
}

void GLUTContext::idle_func(void (*func)()) {
    glutIdleFunc(func);
}

void GLUTContext::timer_func(unsigned int msecs, void (*func)(int), int value) {
    glutTimerFunc(msecs, func, value);
}

void GLUTContext::swap_buffers() {
    glutSwapBuffers();
}

void GLUTContext::set_window(int win) {
    glutSetWindow(win);
}

int GLUTContext::get_window() {
    return glutGetWindow();
}

void GLUTContext::solid_sphere(double radius, int slices, int stacks) {
    glutSolidSphere(radius, slices, stacks);
}

void GLUTContext::wire_sphere(double radius, int slices, int stacks) {
    glutWireSphere(radius, slices, stacks);
}

void GLUTContext::solid_cube(double size) {
    glutSolidCube(size);
}

void GLUTContext::wire_cube(double size) {
    glutWireCube(size);
}

void GLUTContext::solid_torus(double innerRadius, double outerRadius, int sides, int rings) {
    glutSolidTorus(innerRadius, outerRadius, sides, rings);
}

void GLUTContext::wire_torus(double innerRadius, double outerRadius, int sides, int rings) {
    glutWireTorus(innerRadius, outerRadius, sides, rings);
}

} // namespace graphics
} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    void sapphire_glut_init(int* argc, char** argv) {
        sapphire::stdlib::graphics::GLUTContext::init(argc, argv);
    }
    
    void sapphire_glut_init_display_mode(unsigned int mode) {
        sapphire::stdlib::graphics::GLUTContext::init_display_mode(mode);
    }
    
    void sapphire_glut_init_window_size(int width, int height) {
        sapphire::stdlib::graphics::GLUTContext::init_window_size(width, height);
    }
    
    void sapphire_glut_init_window_position(int x, int y) {
        sapphire::stdlib::graphics::GLUTContext::init_window_position(x, y);
    }
    
    int sapphire_glut_create_window(const char* title) {
        return sapphire::stdlib::graphics::GLUTContext::create_window(title);
    }
    
    void sapphire_glut_main_loop() {
        sapphire::stdlib::graphics::GLUTContext::main_loop();
    }
    
    void sapphire_glut_post_redisplay() {
        sapphire::stdlib::graphics::GLUTContext::post_redisplay();
    }
    
    void sapphire_glut_swap_buffers() {
        sapphire::stdlib::graphics::GLUTContext::swap_buffers();
    }
    
    void sapphire_glut_solid_sphere(double radius, int slices, int stacks) {
        sapphire::stdlib::graphics::GLUTContext::solid_sphere(radius, slices, stacks);
    }
    
    void sapphire_glut_wire_sphere(double radius, int slices, int stacks) {
        sapphire::stdlib::graphics::GLUTContext::wire_sphere(radius, slices, stacks);
    }
    
    void sapphire_glut_solid_cube(double size) {
        sapphire::stdlib::graphics::GLUTContext::solid_cube(size);
    }
    
    void sapphire_glut_wire_cube(double size) {
        sapphire::stdlib::graphics::GLUTContext::wire_cube(size);
    }
    
    void sapphire_glut_solid_torus(double innerRadius, double outerRadius, int sides, int rings) {
        sapphire::stdlib::graphics::GLUTContext::solid_torus(innerRadius, outerRadius, sides, rings);
    }
    
    void sapphire_glut_wire_torus(double innerRadius, double outerRadius, int sides, int rings) {
        sapphire::stdlib::graphics::GLUTContext::wire_torus(innerRadius, outerRadius, sides, rings);
    }
}
