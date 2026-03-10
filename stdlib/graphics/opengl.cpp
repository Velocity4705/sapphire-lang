#include "opengl.h"
#include <iostream>

namespace sapphire {
namespace stdlib {
namespace graphics {

bool OpenGLContext::init() {
    // OpenGL initialization is typically handled by the windowing system
    // (SDL, GLUT, GLFW, etc.)
    return true;
}

void OpenGLContext::viewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenGLContext::clear_color(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void OpenGLContext::clear(unsigned int mask) {
    glClear(mask);
}

void OpenGLContext::matrix_mode(unsigned int mode) {
    glMatrixMode(mode);
}

void OpenGLContext::load_identity() {
    glLoadIdentity();
}

void OpenGLContext::ortho(double left, double right, double bottom, double top, double near, double far) {
    glOrtho(left, right, bottom, top, near, far);
}

void OpenGLContext::perspective(double fovy, double aspect, double near, double far) {
    gluPerspective(fovy, aspect, near, far);
}

void OpenGLContext::translate(float x, float y, float z) {
    glTranslatef(x, y, z);
}

void OpenGLContext::rotate(float angle, float x, float y, float z) {
    glRotatef(angle, x, y, z);
}

void OpenGLContext::scale(float x, float y, float z) {
    glScalef(x, y, z);
}

void OpenGLContext::begin(unsigned int mode) {
    glBegin(mode);
}

void OpenGLContext::end() {
    glEnd();
}

void OpenGLContext::vertex2f(float x, float y) {
    glVertex2f(x, y);
}

void OpenGLContext::vertex3f(float x, float y, float z) {
    glVertex3f(x, y, z);
}

void OpenGLContext::color3f(float r, float g, float b) {
    glColor3f(r, g, b);
}

void OpenGLContext::color4f(float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
}

void OpenGLContext::enable(unsigned int cap) {
    glEnable(cap);
}

void OpenGLContext::disable(unsigned int cap) {
    glDisable(cap);
}

void OpenGLContext::depth_func(unsigned int func) {
    glDepthFunc(func);
}

void OpenGLContext::blend_func(unsigned int sfactor, unsigned int dfactor) {
    glBlendFunc(sfactor, dfactor);
}

} // namespace graphics
} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    bool sapphire_gl_init() {
        return sapphire::stdlib::graphics::OpenGLContext::init();
    }
    
    void sapphire_gl_viewport(int x, int y, int width, int height) {
        sapphire::stdlib::graphics::OpenGLContext::viewport(x, y, width, height);
    }
    
    void sapphire_gl_clear_color(float r, float g, float b, float a) {
        sapphire::stdlib::graphics::OpenGLContext::clear_color(r, g, b, a);
    }
    
    void sapphire_gl_clear(unsigned int mask) {
        sapphire::stdlib::graphics::OpenGLContext::clear(mask);
    }
    
    void sapphire_gl_matrix_mode(unsigned int mode) {
        sapphire::stdlib::graphics::OpenGLContext::matrix_mode(mode);
    }
    
    void sapphire_gl_load_identity() {
        sapphire::stdlib::graphics::OpenGLContext::load_identity();
    }
    
    void sapphire_gl_ortho(double left, double right, double bottom, double top, double near, double far) {
        sapphire::stdlib::graphics::OpenGLContext::ortho(left, right, bottom, top, near, far);
    }
    
    void sapphire_gl_perspective(double fovy, double aspect, double near, double far) {
        sapphire::stdlib::graphics::OpenGLContext::perspective(fovy, aspect, near, far);
    }
    
    void sapphire_gl_translate(float x, float y, float z) {
        sapphire::stdlib::graphics::OpenGLContext::translate(x, y, z);
    }
    
    void sapphire_gl_rotate(float angle, float x, float y, float z) {
        sapphire::stdlib::graphics::OpenGLContext::rotate(angle, x, y, z);
    }
    
    void sapphire_gl_scale(float x, float y, float z) {
        sapphire::stdlib::graphics::OpenGLContext::scale(x, y, z);
    }
    
    void sapphire_gl_begin(unsigned int mode) {
        sapphire::stdlib::graphics::OpenGLContext::begin(mode);
    }
    
    void sapphire_gl_end() {
        sapphire::stdlib::graphics::OpenGLContext::end();
    }
    
    void sapphire_gl_vertex2f(float x, float y) {
        sapphire::stdlib::graphics::OpenGLContext::vertex2f(x, y);
    }
    
    void sapphire_gl_vertex3f(float x, float y, float z) {
        sapphire::stdlib::graphics::OpenGLContext::vertex3f(x, y, z);
    }
    
    void sapphire_gl_color3f(float r, float g, float b) {
        sapphire::stdlib::graphics::OpenGLContext::color3f(r, g, b);
    }
    
    void sapphire_gl_color4f(float r, float g, float b, float a) {
        sapphire::stdlib::graphics::OpenGLContext::color4f(r, g, b, a);
    }
    
    void sapphire_gl_enable(unsigned int cap) {
        sapphire::stdlib::graphics::OpenGLContext::enable(cap);
    }
    
    void sapphire_gl_disable(unsigned int cap) {
        sapphire::stdlib::graphics::OpenGLContext::disable(cap);
    }
    
    void sapphire_gl_depth_func(unsigned int func) {
        sapphire::stdlib::graphics::OpenGLContext::depth_func(func);
    }
    
    void sapphire_gl_blend_func(unsigned int sfactor, unsigned int dfactor) {
        sapphire::stdlib::graphics::OpenGLContext::blend_func(sfactor, dfactor);
    }
}
