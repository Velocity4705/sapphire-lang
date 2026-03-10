#include "opengl.h"
#include <cmath>

namespace sapphire {
namespace stdlib {
namespace graphics {

bool OpenGL::init() {
    // OpenGL initialization is typically handled by the window system (SDL2/GLUT)
    return true;
}

void OpenGL::viewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenGL::clear_color(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void OpenGL::clear(unsigned int mask) {
    glClear(mask);
}

void OpenGL::matrix_mode(unsigned int mode) {
    glMatrixMode(mode);
}

void OpenGL::load_identity() {
    glLoadIdentity();
}

void OpenGL::ortho(double left, double right, double bottom, double top, double near_val, double far_val) {
    glOrtho(left, right, bottom, top, near_val, far_val);
}

void OpenGL::perspective(double fovy, double aspect, double near_val, double far_val) {
    gluPerspective(fovy, aspect, near_val, far_val);
}

void OpenGL::frustum(double left, double right, double bottom, double top, double near_val, double far_val) {
    glFrustum(left, right, bottom, top, near_val, far_val);
}

void OpenGL::translate(float x, float y, float z) {
    glTranslatef(x, y, z);
}

void OpenGL::rotate(float angle, float x, float y, float z) {
    glRotatef(angle, x, y, z);
}

void OpenGL::scale(float x, float y, float z) {
    glScalef(x, y, z);
}

void OpenGL::push_matrix() {
    glPushMatrix();
}

void OpenGL::pop_matrix() {
    glPopMatrix();
}

void OpenGL::begin(unsigned int mode) {
    glBegin(mode);
}

void OpenGL::end() {
    glEnd();
}

void OpenGL::vertex2f(float x, float y) {
    glVertex2f(x, y);
}

void OpenGL::vertex3f(float x, float y, float z) {
    glVertex3f(x, y, z);
}

void OpenGL::color3f(float r, float g, float b) {
    glColor3f(r, g, b);
}

void OpenGL::color4f(float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
}

void OpenGL::normal3f(float x, float y, float z) {
    glNormal3f(x, y, z);
}

void OpenGL::tex_coord2f(float s, float t) {
    glTexCoord2f(s, t);
}

void OpenGL::enable(unsigned int cap) {
    glEnable(cap);
}

void OpenGL::disable(unsigned int cap) {
    glDisable(cap);
}

void OpenGL::depth_func(unsigned int func) {
    glDepthFunc(func);
}

void OpenGL::blend_func(unsigned int sfactor, unsigned int dfactor) {
    glBlendFunc(sfactor, dfactor);
}

void OpenGL::cull_face(unsigned int mode) {
    glCullFace(mode);
}

void OpenGL::light(unsigned int light, unsigned int pname, const float* params) {
    glLightfv(light, pname, params);
}

void OpenGL::material(unsigned int face, unsigned int pname, const float* params) {
    glMaterialfv(face, pname, params);
}

} // namespace graphics
} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    bool sapphire_gl_init() {
        return sapphire::stdlib::graphics::OpenGL::init();
    }
    
    void sapphire_gl_viewport(int x, int y, int width, int height) {
        sapphire::stdlib::graphics::OpenGL::viewport(x, y, width, height);
    }
    
    void sapphire_gl_clear_color(float r, float g, float b, float a) {
        sapphire::stdlib::graphics::OpenGL::clear_color(r, g, b, a);
    }
    
    void sapphire_gl_clear(unsigned int mask) {
        sapphire::stdlib::graphics::OpenGL::clear(mask);
    }
    
    void sapphire_gl_matrix_mode(unsigned int mode) {
        sapphire::stdlib::graphics::OpenGL::matrix_mode(mode);
    }
    
    void sapphire_gl_load_identity() {
        sapphire::stdlib::graphics::OpenGL::load_identity();
    }
    
    void sapphire_gl_ortho(double left, double right, double bottom, double top, double near_val, double far_val) {
        sapphire::stdlib::graphics::OpenGL::ortho(left, right, bottom, top, near_val, far_val);
    }
    
    void sapphire_gl_perspective(double fovy, double aspect, double near_val, double far_val) {
        sapphire::stdlib::graphics::OpenGL::perspective(fovy, aspect, near_val, far_val);
    }
    
    void sapphire_gl_frustum(double left, double right, double bottom, double top, double near_val, double far_val) {
        sapphire::stdlib::graphics::OpenGL::frustum(left, right, bottom, top, near_val, far_val);
    }
    
    void sapphire_gl_translate(float x, float y, float z) {
        sapphire::stdlib::graphics::OpenGL::translate(x, y, z);
    }
    
    void sapphire_gl_rotate(float angle, float x, float y, float z) {
        sapphire::stdlib::graphics::OpenGL::rotate(angle, x, y, z);
    }
    
    void sapphire_gl_scale(float x, float y, float z) {
        sapphire::stdlib::graphics::OpenGL::scale(x, y, z);
    }
    
    void sapphire_gl_push_matrix() {
        sapphire::stdlib::graphics::OpenGL::push_matrix();
    }
    
    void sapphire_gl_pop_matrix() {
        sapphire::stdlib::graphics::OpenGL::pop_matrix();
    }
    
    void sapphire_gl_begin(unsigned int mode) {
        sapphire::stdlib::graphics::OpenGL::begin(mode);
    }
    
    void sapphire_gl_end() {
        sapphire::stdlib::graphics::OpenGL::end();
    }
    
    void sapphire_gl_vertex2f(float x, float y) {
        sapphire::stdlib::graphics::OpenGL::vertex2f(x, y);
    }
    
    void sapphire_gl_vertex3f(float x, float y, float z) {
        sapphire::stdlib::graphics::OpenGL::vertex3f(x, y, z);
    }
    
    void sapphire_gl_color3f(float r, float g, float b) {
        sapphire::stdlib::graphics::OpenGL::color3f(r, g, b);
    }
    
    void sapphire_gl_color4f(float r, float g, float b, float a) {
        sapphire::stdlib::graphics::OpenGL::color4f(r, g, b, a);
    }
    
    void sapphire_gl_normal3f(float x, float y, float z) {
        sapphire::stdlib::graphics::OpenGL::normal3f(x, y, z);
    }
    
    void sapphire_gl_tex_coord2f(float s, float t) {
        sapphire::stdlib::graphics::OpenGL::tex_coord2f(s, t);
    }
    
    void sapphire_gl_enable(unsigned int cap) {
        sapphire::stdlib::graphics::OpenGL::enable(cap);
    }
    
    void sapphire_gl_disable(unsigned int cap) {
        sapphire::stdlib::graphics::OpenGL::disable(cap);
    }
    
    void sapphire_gl_depth_func(unsigned int func) {
        sapphire::stdlib::graphics::OpenGL::depth_func(func);
    }
    
    void sapphire_gl_blend_func(unsigned int sfactor, unsigned int dfactor) {
        sapphire::stdlib::graphics::OpenGL::blend_func(sfactor, dfactor);
    }
    
    void sapphire_gl_cull_face(unsigned int mode) {
        sapphire::stdlib::graphics::OpenGL::cull_face(mode);
    }
}
