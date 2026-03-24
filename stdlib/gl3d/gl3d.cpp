#include "gl3d.h"
#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <chrono>

// stb_image — compiled directly here (texture.cpp uses static linkage)
#define STB_IMAGE_IMPLEMENTATION
#include "../texture/stb_image.h"

static std::string s_error;
static bool s_sdl_init = false;

static void ensure_sdl() {
    if (!s_sdl_init) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        s_sdl_init = true;
    }
}

// ── GLSL shader loader ────────────────────────────────────────────────────
static GLuint compile_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[512]; glGetShaderInfoLog(s, 512, nullptr, buf);
        s_error = buf; glDeleteShader(s); return 0;
    }
    return s;
}

struct Gl3dShader {
    GLuint prog = 0;
};

struct Gl3dMesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    int icount = 0;
};

// Line mesh: flat [x,y,z, x,y,z, ...] pairs drawn with GL_LINES
struct Gl3dLineMesh {
    GLuint vao = 0, vbo = 0;
    int vcount = 0;  // number of vertices (2 per line segment)
};

struct Gl3dTexture {
    GLuint id = 0;
    int w = 0, h = 0;
};

struct Gl3dWindow {
    SDL_Window*   win  = nullptr;
    SDL_GLContext ctx  = nullptr;
    int w = 0, h = 0;
    float dt = 0.016f;
    float time = 0.f;
    std::chrono::steady_clock::time_point last_frame;
    std::chrono::steady_clock::time_point t0;
    bool running = true;
    bool mouse_captured = false;
    float mdx = 0, mdy = 0;
    std::unordered_map<SDL_Scancode, bool> keys_held;
    std::unordered_map<SDL_Scancode, bool> keys_pressed;
};

// ── Key name → scancode ───────────────────────────────────────────────────
static SDL_Scancode key_scancode(const char* name) {
    std::string n(name);
    if (n=="w"||n=="W") return SDL_SCANCODE_W;
    if (n=="a"||n=="A") return SDL_SCANCODE_A;
    if (n=="s"||n=="S") return SDL_SCANCODE_S;
    if (n=="d"||n=="D") return SDL_SCANCODE_D;
    if (n=="q"||n=="Q") return SDL_SCANCODE_Q;
    if (n=="e"||n=="E") return SDL_SCANCODE_E;
    if (n=="r"||n=="R") return SDL_SCANCODE_R;
    if (n=="f"||n=="F") return SDL_SCANCODE_F;
    if (n=="space"||n=="Space") return SDL_SCANCODE_SPACE;
    if (n=="escape"||n=="Escape"||n=="esc") return SDL_SCANCODE_ESCAPE;
    if (n=="up")    return SDL_SCANCODE_UP;
    if (n=="down")  return SDL_SCANCODE_DOWN;
    if (n=="left")  return SDL_SCANCODE_LEFT;
    if (n=="right") return SDL_SCANCODE_RIGHT;
    if (n=="shift"||n=="lshift") return SDL_SCANCODE_LSHIFT;
    if (n=="ctrl"||n=="lctrl")   return SDL_SCANCODE_LCTRL;
    if (n=="tab")   return SDL_SCANCODE_TAB;
    if (n=="h"||n=="H") return SDL_SCANCODE_H;
    if (n=="c"||n=="C") return SDL_SCANCODE_C;
    if (n=="z"||n=="Z") return SDL_SCANCODE_Z;
    if (n=="x"||n=="X") return SDL_SCANCODE_X;
    if (n=="v"||n=="V") return SDL_SCANCODE_V;
    if (n=="b"||n=="B") return SDL_SCANCODE_B;
    if (n=="n"||n=="N") return SDL_SCANCODE_N;
    if (n=="m"||n=="M") return SDL_SCANCODE_M;
    if (n=="1") return SDL_SCANCODE_1;
    if (n=="2") return SDL_SCANCODE_2;
    if (n=="3") return SDL_SCANCODE_3;
    if (n=="4") return SDL_SCANCODE_4;
    if (n=="5") return SDL_SCANCODE_5;
    if (n=="[") return SDL_SCANCODE_LEFTBRACKET;
    if (n=="]") return SDL_SCANCODE_RIGHTBRACKET;
    if (n==";"||n=="semicolon") return SDL_SCANCODE_SEMICOLON;
    if (n=="'"||n=="quote")     return SDL_SCANCODE_APOSTROPHE;
    if (n==","||n=="comma")     return SDL_SCANCODE_COMMA;
    if (n=="."||n=="period")    return SDL_SCANCODE_PERIOD;
    if (n=="/"||n=="slash")     return SDL_SCANCODE_SLASH;
    return SDL_SCANCODE_UNKNOWN;
}

// ── Window ────────────────────────────────────────────────────────────────
extern "C" {

void* gl3d_window_create(const char* title, int w, int h) {
    ensure_sdl();
    auto* win = new Gl3dWindow();
    win->win = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS);
    if (!win->win) { s_error = SDL_GetError(); delete win; return nullptr; }
    win->ctx = SDL_GL_CreateContext(win->win);
    if (!win->ctx) { s_error = SDL_GetError(); SDL_DestroyWindow(win->win); delete win; return nullptr; }
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    win->w = w; win->h = h;
    win->last_frame = std::chrono::steady_clock::now();
    win->t0 = win->last_frame;
    // Ensure the window has keyboard focus before any input capture
    SDL_RaiseWindow(win->win);
    SDL_SetWindowInputFocus(win->win);
    return win;
}

void gl3d_window_destroy(void* w) {
    auto* win = (Gl3dWindow*)w;
    if (!win) return;
    SDL_GL_DeleteContext(win->ctx);
    SDL_DestroyWindow(win->win);
    delete win;
}

int gl3d_window_poll(void* w) {
    auto* win = (Gl3dWindow*)w;
    if (!win || !win->running) return 0;
    // clear per-frame state
    win->keys_pressed.clear();
    win->mdx = 0; win->mdy = 0;
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) { win->running = false; return 0; }
        if (ev.type == SDL_KEYDOWN) {
            auto sc = ev.key.keysym.scancode;
            if (sc == SDL_SCANCODE_ESCAPE) { win->running = false; return 0; }
            if (!win->keys_held[sc]) win->keys_pressed[sc] = true;
            win->keys_held[sc] = true;
        }
        if (ev.type == SDL_KEYUP)
            win->keys_held[ev.key.keysym.scancode] = false;
        if (ev.type == SDL_MOUSEMOTION) {
            win->mdx += ev.motion.xrel;
            win->mdy += ev.motion.yrel;
        }
        if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED) {
            win->w = ev.window.data1; win->h = ev.window.data2;
            glViewport(0, 0, win->w, win->h);
        }
        if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            // Flush stale key state on focus gain to avoid phantom held keys
            win->keys_held.clear();
            win->keys_pressed.clear();
        }
    }
    // delta time
    auto now = std::chrono::steady_clock::now();
    win->dt = std::chrono::duration<float>(now - win->last_frame).count();
    if (win->dt > 0.1f) win->dt = 0.1f;
    win->last_frame = now;
    win->time = std::chrono::duration<float>(now - win->t0).count();
    return 1;
}

void gl3d_window_swap(void* w) {
    auto* win = (Gl3dWindow*)w;
    if (win) SDL_GL_SwapWindow(win->win);
}

void gl3d_window_set_title(void* w, const char* title) {
    auto* win = (Gl3dWindow*)w;
    if (win) SDL_SetWindowTitle(win->win, title);
}

int gl3d_window_width(void* w)  { return w ? ((Gl3dWindow*)w)->w : 0; }
int gl3d_window_height(void* w) { return w ? ((Gl3dWindow*)w)->h : 0; }

// ── Input ─────────────────────────────────────────────────────────────────
int gl3d_key(void* w, const char* name) {
    auto* win = (Gl3dWindow*)w;
    if (!win) return 0;
    auto sc = key_scancode(name);
    auto it = win->keys_held.find(sc);
    return (it != win->keys_held.end() && it->second) ? 1 : 0;
}
int gl3d_key_pressed(void* w, const char* name) {
    auto* win = (Gl3dWindow*)w;
    if (!win) return 0;
    auto sc = key_scancode(name);
    auto it = win->keys_pressed.find(sc);
    return (it != win->keys_pressed.end() && it->second) ? 1 : 0;
}
float gl3d_mouse_dx(void* w) { return w ? ((Gl3dWindow*)w)->mdx : 0; }
float gl3d_mouse_dy(void* w) { return w ? ((Gl3dWindow*)w)->mdy : 0; }
void  gl3d_mouse_capture(void* w, int captured) {
    auto* win = (Gl3dWindow*)w;
    if (!win) return;
    win->mouse_captured = captured != 0;
    if (captured) {
        // Ensure window has focus before grabbing mouse — on Linux/Wayland
        // SDL_SetRelativeMouseMode can silently fail if the window isn't focused,
        // which also breaks keyboard input.
        SDL_RaiseWindow(win->win);
        SDL_SetWindowInputFocus(win->win);
        SDL_Delay(10);  // give WM time to process focus
    }
    SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
    // Re-assert input focus after mode switch
    if (captured) {
        SDL_RaiseWindow(win->win);
    }
}
float gl3d_delta(void* w) { return w ? ((Gl3dWindow*)w)->dt : 0.016f; }
float gl3d_time(void* w)  { return w ? ((Gl3dWindow*)w)->time : 0.f; }

// ── Mesh ──────────────────────────────────────────────────────────────────
void* gl3d_mesh_create(const float* verts, int vcount,
                        const unsigned int* idx, int icount) {
    auto* m = new Gl3dMesh();
    m->icount = icount;
    glGenVertexArrays(1, &m->vao);
    glGenBuffers(1, &m->vbo);
    glGenBuffers(1, &m->ebo);
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, vcount * 8 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount * sizeof(unsigned int), idx, GL_STATIC_DRAW);
    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return m;
}

void gl3d_mesh_destroy(void* mesh) {
    auto* m = (Gl3dMesh*)mesh;
    if (!m) return;
    glDeleteVertexArrays(1, &m->vao);
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ebo);
    delete m;
}

void gl3d_mesh_draw(void* mesh) {
    auto* m = (Gl3dMesh*)mesh;
    if (!m) return;
    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ── Line mesh (for 4D wireframes etc.) ───────────────────────────────────
// verts: flat [x,y,z, x,y,z, ...], vcount = number of vertices (2 per segment)
void* gl3d_mesh_lines_create(const float* verts, int vcount) {
    auto* m = new Gl3dLineMesh();
    m->vcount = vcount;
    glGenVertexArrays(1, &m->vao);
    glGenBuffers(1, &m->vbo);
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, vcount * 3 * sizeof(float), verts, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return m;
}

void gl3d_mesh_lines_update(void* mesh, const float* verts, int vcount) {
    auto* m = (Gl3dLineMesh*)mesh;
    if (!m) return;
    m->vcount = vcount;
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, vcount * 3 * sizeof(float), verts, GL_DYNAMIC_DRAW);
}

void gl3d_mesh_lines_draw(void* mesh) {
    auto* m = (Gl3dLineMesh*)mesh;
    if (!m) return;
    glBindVertexArray(m->vao);
    glDrawArrays(GL_LINES, 0, m->vcount);
    glBindVertexArray(0);
}

void gl3d_mesh_lines_destroy(void* mesh) {
    auto* m = (Gl3dLineMesh*)mesh;
    if (!m) return;
    glDeleteVertexArrays(1, &m->vao);
    glDeleteBuffers(1, &m->vbo);
    delete m;
}

// ── Shader ────────────────────────────────────────────────────────────────
void* gl3d_shader_create(const char* vert_src, const char* frag_src) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vert_src);
    if (!vs) return nullptr;
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, frag_src);
    if (!fs) { glDeleteShader(vs); return nullptr; }
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs); glDeleteShader(fs);
    GLint ok; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[512]; glGetProgramInfoLog(prog, 512, nullptr, buf);
        s_error = buf; glDeleteProgram(prog); return nullptr;
    }
    auto* sh = new Gl3dShader(); sh->prog = prog;
    return sh;
}
void gl3d_shader_destroy(void* sh) {
    auto* s = (Gl3dShader*)sh;
    if (!s) return;
    glDeleteProgram(s->prog); delete s;
}
void gl3d_shader_use(void* sh) {
    auto* s = (Gl3dShader*)sh;
    if (s) glUseProgram(s->prog);
}
static GLint uloc(void* sh, const char* name) {
    return glGetUniformLocation(((Gl3dShader*)sh)->prog, name);
}
void gl3d_shader_set_float(void* sh, const char* n, float v)
    { if(sh) glUniform1f(uloc(sh,n), v); }
void gl3d_shader_set_vec3(void* sh, const char* n, float x, float y, float z)
    { if(sh) glUniform3f(uloc(sh,n), x, y, z); }
void gl3d_shader_set_vec4(void* sh, const char* n, float x, float y, float z, float w)
    { if(sh) glUniform4f(uloc(sh,n), x, y, z, w); }
void gl3d_shader_set_mat4(void* sh, const char* n, const float* m)
    { if(sh) glUniformMatrix4fv(uloc(sh,n), 1, GL_FALSE, m); }
void gl3d_shader_set_int(void* sh, const char* n, int v)
    { if(sh) glUniform1i(uloc(sh,n), v); }

// ── Texture ───────────────────────────────────────────────────────────────
void* gl3d_texture_load(const char* path) {
    int w, h, ch;
    unsigned char* data = stbi_load(path, &w, &h, &ch, 4);
    if (!data) { s_error = stbi_failure_reason(); return nullptr; }
    auto* t = new Gl3dTexture(); t->w = w; t->h = h;
    glGenTextures(1, &t->id);
    glBindTexture(GL_TEXTURE_2D, t->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return t;
}
void* gl3d_texture_solid(unsigned char r, unsigned char g, unsigned char b) {
    auto* t = new Gl3dTexture(); t->w = 1; t->h = 1;
    glGenTextures(1, &t->id);
    glBindTexture(GL_TEXTURE_2D, t->id);
    unsigned char px[4] = {r, g, b, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return t;
}
void gl3d_texture_destroy(void* tex) {
    auto* t = (Gl3dTexture*)tex;
    if (!t) return;
    glDeleteTextures(1, &t->id); delete t;
}
void gl3d_texture_bind(void* tex, int unit) {
    auto* t = (Gl3dTexture*)tex;
    if (!t) return;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, t->id);
}

// ── Render state ──────────────────────────────────────────────────────────
void gl3d_clear(float r, float g, float b) {
    glClearColor(r, g, b, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void gl3d_depth_test(int e) { e ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST); }
void gl3d_wireframe(int e)  { glPolygonMode(GL_FRONT_AND_BACK, e ? GL_LINE : GL_FILL); }
void gl3d_viewport(int x, int y, int w, int h) { glViewport(x, y, w, h); }
const char* gl3d_error(void) { return s_error.c_str(); }

// ── Math ──────────────────────────────────────────────────────────────────
static const float PI = 3.14159265358979f;
static float* mat4_new() { return new float[16](); }

float* gl3d_mat4_identity() {
    float* m = mat4_new();
    m[0]=m[5]=m[10]=m[15]=1.f;
    return m;
}
float* gl3d_mat4_perspective(float fovy, float aspect, float near, float far) {
    float* m = mat4_new();
    float f = 1.f / tanf(fovy * PI / 360.f);
    m[0]  = f / aspect;
    m[5]  = f;
    m[10] = (far + near) / (near - far);
    m[11] = -1.f;
    m[14] = (2.f * far * near) / (near - far);
    return m;
}
float* gl3d_mat4_lookat(float ex,float ey,float ez,
                         float cx,float cy,float cz,
                         float ux,float uy,float uz) {
    float fx=cx-ex, fy=cy-ey, fz=cz-ez;
    float fl=sqrtf(fx*fx+fy*fy+fz*fz); fx/=fl; fy/=fl; fz/=fl;
    float rx=fy*uz-fz*uy, ry=fz*ux-fx*uz, rz=fx*uy-fy*ux;
    float rl=sqrtf(rx*rx+ry*ry+rz*rz); rx/=rl; ry/=rl; rz/=rl;
    float ux2=ry*fz-rz*fy, uy2=rz*fx-rx*fz, uz2=rx*fy-ry*fx;
    float* m = mat4_new();
    m[0]=rx;  m[4]=ry;  m[8]=rz;   m[12]=-(rx*ex+ry*ey+rz*ez);
    m[1]=ux2; m[5]=uy2; m[9]=uz2;  m[13]=-(ux2*ex+uy2*ey+uz2*ez);
    m[2]=-fx; m[6]=-fy; m[10]=-fz; m[14]=(fx*ex+fy*ey+fz*ez);
    m[15]=1.f;
    return m;
}
float* gl3d_mat4_translate(float x, float y, float z) {
    float* m = gl3d_mat4_identity();
    m[12]=x; m[13]=y; m[14]=z;
    return m;
}
float* gl3d_mat4_scale(float x, float y, float z) {
    float* m = mat4_new();
    m[0]=x; m[5]=y; m[10]=z; m[15]=1.f;
    return m;
}
float* gl3d_mat4_rotate_y(float deg) {
    float* m = gl3d_mat4_identity();
    float r = deg * PI / 180.f;
    m[0]=cosf(r); m[8]=sinf(r);
    m[2]=-sinf(r); m[10]=cosf(r);
    return m;
}
float* gl3d_mat4_rotate_x(float deg) {
    float* m = gl3d_mat4_identity();
    float r = deg * PI / 180.f;
    m[5]=cosf(r); m[9]=-sinf(r);
    m[6]=sinf(r); m[10]=cosf(r);
    return m;
}
float* gl3d_mat4_rotate_z(float deg) {
    float* m = gl3d_mat4_identity();
    float r = deg * PI / 180.f;
    m[0]=cosf(r); m[4]=-sinf(r);
    m[1]=sinf(r); m[5]=cosf(r);
    return m;
}
float* gl3d_mat4_mul(const float* a, const float* b) {
    float* c = mat4_new();
    for (int row=0;row<4;row++)
        for (int col=0;col<4;col++)
            for (int k=0;k<4;k++)
                c[col*4+row] += a[k*4+row] * b[col*4+k];
    return c;
}
void gl3d_mat4_free(float* m) { delete[] m; }

// ── Built-in mesh generators ──────────────────────────────────────────────
// Helper: push vertex [x,y,z, nx,ny,nz, u,v]
static void push_v(std::vector<float>& v,
                   float x,float y,float z,
                   float nx,float ny,float nz,
                   float u,float vt) {
    v.insert(v.end(), {x,y,z,nx,ny,nz,u,vt});
}

void* gl3d_mesh_box(float w, float h, float d) {
    float hw=w*.5f, hh=h*.5f, hd=d*.5f;
    std::vector<float> verts;
    std::vector<unsigned int> idx;
    // 6 faces
    struct Face { float nx,ny,nz; float verts[4][5]; }; // x,y,z,u,v
    Face faces[6] = {
        // +Z
        {0,0,1,{{-hw,-hh,hd,0,0},{hw,-hh,hd,1,0},{hw,hh,hd,1,1},{-hw,hh,hd,0,1}}},
        // -Z
        {0,0,-1,{{hw,-hh,-hd,0,0},{-hw,-hh,-hd,1,0},{-hw,hh,-hd,1,1},{hw,hh,-hd,0,1}}},
        // +X
        {1,0,0,{{hw,-hh,hd,0,0},{hw,-hh,-hd,1,0},{hw,hh,-hd,1,1},{hw,hh,hd,0,1}}},
        // -X
        {-1,0,0,{{-hw,-hh,-hd,0,0},{-hw,-hh,hd,1,0},{-hw,hh,hd,1,1},{-hw,hh,-hd,0,1}}},
        // +Y
        {0,1,0,{{-hw,hh,hd,0,0},{hw,hh,hd,1,0},{hw,hh,-hd,1,1},{-hw,hh,-hd,0,1}}},
        // -Y
        {0,-1,0,{{-hw,-hh,-hd,0,0},{hw,-hh,-hd,1,0},{hw,-hh,hd,1,1},{-hw,-hh,hd,0,1}}},
    };
    for (auto& f : faces) {
        unsigned int base = (unsigned int)(verts.size()/8);
        for (auto& v : f.verts)
            push_v(verts, v[0],v[1],v[2], f.nx,f.ny,f.nz, v[3],v[4]);
        idx.insert(idx.end(), {base,base+1,base+2, base,base+2,base+3});
    }
    return gl3d_mesh_create(verts.data(), (int)(verts.size()/8),
                             idx.data(), (int)idx.size());
}

void* gl3d_mesh_plane(float w, float d, int sub) {
    std::vector<float> verts;
    std::vector<unsigned int> idx;
    float hw=w*.5f, hd=d*.5f;
    for (int z=0;z<=sub;z++)
        for (int x=0;x<=sub;x++) {
            float fx = -hw + w*(float)x/sub;
            float fz = -hd + d*(float)z/sub;
            push_v(verts, fx,0,fz, 0,1,0, (float)x/sub,(float)z/sub);
        }
    for (int z=0;z<sub;z++)
        for (int x=0;x<sub;x++) {
            unsigned int i = z*(sub+1)+x;
            idx.insert(idx.end(), {i,i+sub+1,i+1, i+1,i+sub+1,i+sub+2});
        }
    return gl3d_mesh_create(verts.data(),(int)(verts.size()/8),
                             idx.data(),(int)idx.size());
}

void* gl3d_mesh_sphere(float r, int sl, int st) {
    std::vector<float> verts;
    std::vector<unsigned int> idx;
    for (int i=0;i<=st;i++) {
        float phi = PI * i / st;
        for (int j=0;j<=sl;j++) {
            float theta = 2*PI*j/sl;
            float x=sinf(phi)*cosf(theta), y=cosf(phi), z=sinf(phi)*sinf(theta);
            push_v(verts, r*x,r*y,r*z, x,y,z, (float)j/sl,(float)i/st);
        }
    }
    for (int i=0;i<st;i++)
        for (int j=0;j<sl;j++) {
            unsigned int a=i*(sl+1)+j, b=a+sl+1;
            idx.insert(idx.end(), {a,b,a+1, a+1,b,b+1});
        }
    return gl3d_mesh_create(verts.data(),(int)(verts.size()/8),
                             idx.data(),(int)idx.size());
}

void* gl3d_mesh_cylinder(float r, float h, int sl) {
    std::vector<float> verts;
    std::vector<unsigned int> idx;
    float hh = h*.5f;
    for (int i=0;i<=sl;i++) {
        float a = 2*PI*i/sl;
        float x=cosf(a), z=sinf(a);
        push_v(verts, r*x,-hh,r*z, x,0,z, (float)i/sl,0);
        push_v(verts, r*x, hh,r*z, x,0,z, (float)i/sl,1);
    }
    for (int i=0;i<sl;i++) {
        unsigned int b=i*2;
        idx.insert(idx.end(), {b,b+2,b+1, b+1,b+2,b+3});
    }
    return gl3d_mesh_create(verts.data(),(int)(verts.size()/8),
                             idx.data(),(int)idx.size());
}

void* gl3d_mesh_terrain(const float* heights, int sub, float size, float hscale) {
    std::vector<float> verts;
    std::vector<unsigned int> idx;
    int N = sub + 1;
    float half = size * .5f;
    for (int z=0;z<N;z++)
        for (int x=0;x<N;x++) {
            float fx = -half + size*(float)x/sub;
            float fz = -half + size*(float)z/sub;
            float fy = heights[z*N+x] * hscale;
            // compute normal from neighbours
            float nx=0,ny=1,nz=0;
            if (x>0&&x<sub&&z>0&&z<sub) {
                float hL=heights[z*N+(x-1)]*hscale, hR=heights[z*N+(x+1)]*hscale;
                float hD=heights[(z-1)*N+x]*hscale, hU=heights[(z+1)*N+x]*hscale;
                float step = size/sub;
                nx = (hL-hR)/(2*step);
                nz = (hD-hU)/(2*step);
                float len=sqrtf(nx*nx+1+nz*nz);
                nx/=len; ny=1.f/len; nz/=len;
            }
            push_v(verts, fx,fy,fz, nx,ny,nz,
                   (float)x/sub*8.f, (float)z/sub*8.f);
        }
    for (int z=0;z<sub;z++)
        for (int x=0;x<sub;x++) {
            unsigned int i=z*N+x;
            idx.insert(idx.end(), {i,i+N,i+1, i+1,i+N,i+N+1});
        }
    return gl3d_mesh_create(verts.data(),(int)(verts.size()/8),
                             idx.data(),(int)idx.size());
}

// ── Fast 4D world renderer ────────────────────────────────────────────────
// Projects a grid of 4D hypercube cells into 3D and draws them in one call.
// cam[16] = {px,py,pz,pw, rx,ry,rz,rw, ux,uy,uz,uw, fx,fy,fz,fw, ax,ay,az,aw}
//   (position + right4 + up4 + fwd4 + ana4)
// cells[n*4] = flat list of cell center 4D coords
// w_dist = W perspective distance
// Returns a Gl3dLineMesh* ready to draw, or updates existing one.
// Each hypercube has 32 edges → 64 vertices → 192 floats per cell.
void* gl3d_world4d_build(const float* cam, const float* cells, int ncells, float w_dist) {
    // Hypercube local verts (16 × 4)
    static const float hv[64] = {
        -.5f,-.5f,-.5f,-.5f,  .5f,-.5f,-.5f,-.5f,
        -.5f, .5f,-.5f,-.5f,  .5f, .5f,-.5f,-.5f,
        -.5f,-.5f, .5f,-.5f,  .5f,-.5f, .5f,-.5f,
        -.5f, .5f, .5f,-.5f,  .5f, .5f, .5f,-.5f,
        -.5f,-.5f,-.5f, .5f,  .5f,-.5f,-.5f, .5f,
        -.5f, .5f,-.5f, .5f,  .5f, .5f,-.5f, .5f,
        -.5f,-.5f, .5f, .5f,  .5f,-.5f, .5f, .5f,
        -.5f, .5f, .5f, .5f,  .5f, .5f, .5f, .5f,
    };
    static const int he[64] = {
        0,1, 0,2, 0,4, 0,8, 1,3, 1,5, 1,9, 2,3,
        2,6, 2,10, 3,7, 3,11, 4,5, 4,6, 4,12, 5,7,
        5,13, 6,7, 6,14, 7,15, 8,9, 8,10, 8,12, 9,11,
        9,13, 10,11, 10,14, 11,15, 12,13, 12,14, 13,15, 14,15
    };

    float px=cam[0], py=cam[1], pz=cam[2], pw=cam[3];
    // basis rows: right, up, fwd, ana
    const float* R = cam+4;
    const float* U = cam+8;
    const float* F = cam+12;
    const float* A = cam+16;

    std::vector<float> buf;
    buf.reserve(ncells * 192);

    for (int ci = 0; ci < ncells; ci++) {
        float cx=cells[ci*4], cy=cells[ci*4+1], cz=cells[ci*4+2], cw=cells[ci*4+3];

        // Project 16 vertices
        float proj3[16*3];
        for (int vi = 0; vi < 16; vi++) {
            float wx = cx + hv[vi*4];
            float wy = cy + hv[vi*4+1];
            float wz = cz + hv[vi*4+2];
            float ww = cw + hv[vi*4+3];
            // Translate to camera
            float dx=wx-px, dy=wy-py, dz=wz-pz, dw=ww-pw;
            // Project onto basis
            float vx = R[0]*dx + R[1]*dy + R[2]*dz + R[3]*dw;
            float vy = U[0]*dx + U[1]*dy + U[2]*dz + U[3]*dw;
            float vz = F[0]*dx + F[1]*dy + F[2]*dz + F[3]*dw;
            float vw = A[0]*dx + A[1]*dy + A[2]*dz + A[3]*dw;
            // W-perspective divide
            float denom = w_dist + vw;
            if (denom < 0.05f) denom = 0.05f;
            float s = w_dist / denom;
            proj3[vi*3]   = vx * s;
            proj3[vi*3+1] = vy * s;
            proj3[vi*3+2] = vz * s;
        }
        // Emit 32 edges
        for (int ei = 0; ei < 32; ei++) {
            int a = he[ei*2], b = he[ei*2+1];
            buf.push_back(proj3[a*3]);   buf.push_back(proj3[a*3+1]); buf.push_back(proj3[a*3+2]);
            buf.push_back(proj3[b*3]);   buf.push_back(proj3[b*3+1]); buf.push_back(proj3[b*3+2]);
        }
    }

    int vc = (int)(buf.size() / 3);
    auto* m = new Gl3dLineMesh();
    m->vcount = vc;
    glGenVertexArrays(1, &m->vao);
    glGenBuffers(1, &m->vbo);
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), buf.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return m;
}

// Update existing world4d mesh in-place (avoids VAO/VBO realloc)
void gl3d_world4d_update(void* mesh, const float* cam, const float* cells, int ncells, float w_dist) {
    auto* m = (Gl3dLineMesh*)mesh;
    if (!m) return;

    static const float hv[64] = {
        -.5f,-.5f,-.5f,-.5f,  .5f,-.5f,-.5f,-.5f,
        -.5f, .5f,-.5f,-.5f,  .5f, .5f,-.5f,-.5f,
        -.5f,-.5f, .5f,-.5f,  .5f,-.5f, .5f,-.5f,
        -.5f, .5f, .5f,-.5f,  .5f, .5f, .5f,-.5f,
        -.5f,-.5f,-.5f, .5f,  .5f,-.5f,-.5f, .5f,
        -.5f, .5f,-.5f, .5f,  .5f, .5f,-.5f, .5f,
        -.5f,-.5f, .5f, .5f,  .5f,-.5f, .5f, .5f,
        -.5f, .5f, .5f, .5f,  .5f, .5f, .5f, .5f,
    };
    static const int he[64] = {
        0,1, 0,2, 0,4, 0,8, 1,3, 1,5, 1,9, 2,3,
        2,6, 2,10, 3,7, 3,11, 4,5, 4,6, 4,12, 5,7,
        5,13, 6,7, 6,14, 7,15, 8,9, 8,10, 8,12, 9,11,
        9,13, 10,11, 10,14, 11,15, 12,13, 12,14, 13,15, 14,15
    };

    float px=cam[0], py=cam[1], pz=cam[2], pw=cam[3];
    const float* R = cam+4;
    const float* U = cam+8;
    const float* F = cam+12;
    const float* A = cam+16;

    std::vector<float> buf;
    buf.reserve(ncells * 192);

    for (int ci = 0; ci < ncells; ci++) {
        float cx=cells[ci*4], cy=cells[ci*4+1], cz=cells[ci*4+2], cw=cells[ci*4+3];
        float proj3[16*3];
        for (int vi = 0; vi < 16; vi++) {
            float wx=cx+hv[vi*4], wy=cy+hv[vi*4+1], wz=cz+hv[vi*4+2], ww=cw+hv[vi*4+3];
            float dx=wx-px, dy=wy-py, dz=wz-pz, dw=ww-pw;
            float vx=R[0]*dx+R[1]*dy+R[2]*dz+R[3]*dw;
            float vy=U[0]*dx+U[1]*dy+U[2]*dz+U[3]*dw;
            float vz=F[0]*dx+F[1]*dy+F[2]*dz+F[3]*dw;
            float vw=A[0]*dx+A[1]*dy+A[2]*dz+A[3]*dw;
            float denom=w_dist+vw; if(denom<0.05f) denom=0.05f;
            float s=w_dist/denom;
            proj3[vi*3]=vx*s; proj3[vi*3+1]=vy*s; proj3[vi*3+2]=vz*s;
        }
        for (int ei=0; ei<32; ei++) {
            int a=he[ei*2], b=he[ei*2+1];
            buf.push_back(proj3[a*3]);   buf.push_back(proj3[a*3+1]); buf.push_back(proj3[a*3+2]);
            buf.push_back(proj3[b*3]);   buf.push_back(proj3[b*3+1]); buf.push_back(proj3[b*3+2]);
        }
    }

    m->vcount = (int)(buf.size() / 3);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), buf.data(), GL_DYNAMIC_DRAW);
}

} // extern "C"

// ── Car Physics Engine ────────────────────────────────────────────────────
// Self-contained rigid-body car physics with suspension, slope forces,
// gravity, tire friction, and obstacle collision.
//
// Sapphire call:
//   state = gl3d_car_step(state, input, heights, subdiv, tsize, hscale,
//                         tree_xs, tree_zs, ntrees, dt)
//
// state[12]:  x, y, z, yaw, vx, vy, vz, vyaw, pitch, roll, wheel_rot, steer_ang
// input[5]:   throttle, brake, steer, handbrake, reset
// heights[]:  terrain heightmap (same as gl3d_mesh_terrain)
// Returns new state[12]

extern "C" {

static float terrain_h(const float* heights, int sub, float tsize, float hscale,
                        float wx, float wz) {
    int N = sub + 1;
    float half = tsize * 0.5f;
    float fx = (wx + half) / tsize;
    float fz = (wz + half) / tsize;
    if (fx < 0) fx = 0; if (fx > 1) fx = 1;
    if (fz < 0) fz = 0; if (fz > 1) fz = 1;
    int xi = (int)(fx * sub); if (xi >= sub) xi = sub-1;
    int zi = (int)(fz * sub); if (zi >= sub) zi = sub-1;
    // Bilinear interpolation
    float lx = fx * sub - xi;
    float lz = fz * sub - zi;
    float h00 = heights[zi*N + xi];
    float h10 = heights[zi*N + xi+1];
    float h01 = heights[(zi+1)*N + xi];
    float h11 = heights[(zi+1)*N + xi+1];
    float h = h00*(1-lx)*(1-lz) + h10*lx*(1-lz) + h01*(1-lx)*lz + h11*lx*lz;
    return h * hscale;
}

static void terrain_normal(const float* heights, int sub, float tsize, float hscale,
                            float wx, float wz, float& nx, float& ny, float& nz) {
    float step = tsize / sub * 0.5f;
    float hL = terrain_h(heights, sub, tsize, hscale, wx-step, wz);
    float hR = terrain_h(heights, sub, tsize, hscale, wx+step, wz);
    float hD = terrain_h(heights, sub, tsize, hscale, wx, wz-step);
    float hU = terrain_h(heights, sub, tsize, hscale, wx, wz+step);
    nx = (hL - hR) / (2*step);
    nz = (hD - hU) / (2*step);
    ny = 1.0f;
    float len = sqrtf(nx*nx + ny*ny + nz*nz);
    nx /= len; ny /= len; nz /= len;
}

// Returns new state[12]
void gl3d_car_step(const float* state, const float* input,
                   const float* heights, int subdiv, float tsize, float hscale,
                   const float* tree_xs, const float* tree_zs, int ntrees,
                   float dt, float* out) {
    // Unpack state
    float x     = state[0],  y     = state[1],  z     = state[2];
    float yaw   = state[3];
    float vx    = state[4],  vy    = state[5],  vz    = state[6];
    float vyaw  = state[7];
    float pitch = state[8],  roll  = state[9];
    float wrot  = state[10], sang  = state[11];

    // Unpack input
    float throttle  = input[0];
    float brake     = input[1];
    float steer_in  = input[2];
    float handbrake = input[3];
    int   do_reset  = (int)input[4];

    if (do_reset) {
        float gh = terrain_h(heights, subdiv, tsize, hscale, 0, 0);
        out[0]=0; out[1]=gh+0.5f; out[2]=0; out[3]=0;
        out[4]=0; out[5]=0; out[6]=0; out[7]=0;
        out[8]=0; out[9]=0; out[10]=0; out[11]=0;
        return;
    }

    const float MASS       = 1200.0f;
    const float GRAVITY    = 9.81f;
    const float SUSP_K     = 28000.0f;  // spring stiffness
    const float SUSP_D     = 2200.0f;   // damping
    const float SUSP_REST  = 0.45f;     // rest length
    const float WHEEL_BASE = 1.4f;      // half-wheelbase
    const float WHEEL_TRACK= 0.9f;      // half-track
    const float MAX_STEER  = 30.0f;     // degrees
    const float ENGINE_F   = 9000.0f;
    const float BRAKE_F    = 14000.0f;
    const float DRAG_LIN   = 0.35f;
    const float DRAG_QUAD  = 0.9f;
    const float LAT_FRIC   = 18.0f;     // lateral friction coefficient
    const float TREE_R     = 1.2f;      // tree collision radius
    const float CAR_R      = 1.1f;

    float yr = yaw * 3.14159f / 180.0f;
    float fwd_x = sinf(yr), fwd_z = -cosf(yr);
    float right_x = cosf(yr), right_z = sinf(yr);

    // ── Suspension: 4 wheel contact points ───────────────────────────
    // Wheel offsets in car-local space [front-left, front-right, rear-left, rear-right]
    float wox[4] = {-WHEEL_TRACK,  WHEEL_TRACK, -WHEEL_TRACK,  WHEEL_TRACK};
    float woz[4] = { WHEEL_BASE,   WHEEL_BASE,  -WHEEL_BASE,  -WHEEL_BASE};

    float susp_fy = 0.0f;  // total suspension force Y
    float susp_pitch_torque = 0.0f;
    float susp_roll_torque  = 0.0f;
    float avg_ground_y = 0.0f;
    int   wheels_grounded = 0;

    for (int i = 0; i < 4; i++) {
        // Wheel world position (approximate, ignoring pitch/roll for contact)
        float wx2 = x + wox[i]*right_x + woz[i]*fwd_x;
        float wz2 = z + wox[i]*right_z + woz[i]*fwd_z;
        float ground_y = terrain_h(heights, subdiv, tsize, hscale, wx2, wz2);
        float wheel_y  = y - SUSP_REST;  // wheel at rest position
        float compress = ground_y - wheel_y;  // positive = compressed
        if (compress > 0.0f) {
            float spring_f = SUSP_K * compress - SUSP_D * vy;
            if (spring_f < 0) spring_f = 0;
            susp_fy += spring_f;
            // Torques for pitch/roll
            susp_pitch_torque += spring_f * woz[i];
            susp_roll_torque  += spring_f * wox[i];
            avg_ground_y += ground_y;
            wheels_grounded++;
        }
    }
    if (wheels_grounded > 0) avg_ground_y /= wheels_grounded;

    // ── Forces ────────────────────────────────────────────────────────
    float fx_acc = 0.0f, fy_acc = 0.0f, fz_acc = 0.0f;

    // Gravity
    fy_acc -= MASS * GRAVITY;

    // Suspension
    fy_acc += susp_fy;

    // Terrain slope force (pushes car along slope)
    float tnx, tny, tnz;
    terrain_normal(heights, subdiv, tsize, hscale, x, z, tnx, tny, tnz);
    // Slope component: gravity projected onto terrain tangent
    float slope_fwd  = -(tnx*fwd_x  + tnz*fwd_z)  * MASS * GRAVITY;
    float slope_side = -(tnx*right_x + tnz*right_z) * MASS * GRAVITY;

    // Engine / brake force (along forward direction)
    float speed_fwd = vx*fwd_x + vz*fwd_z;
    float drive_f = 0.0f;
    if (throttle > 0.0f) drive_f =  ENGINE_F * throttle;
    if (brake    > 0.0f) drive_f = -BRAKE_F  * brake * (speed_fwd > 0 ? 1.0f : -1.0f);
    if (handbrake > 0.5f) drive_f = -BRAKE_F * 0.6f * (speed_fwd > 0 ? 1.0f : -1.0f);

    // Only apply drive/slope when wheels are on ground
    float ground_factor = (wheels_grounded >= 2) ? 1.0f : 0.0f;
    fx_acc += (drive_f + slope_fwd) * fwd_x * ground_factor;
    fz_acc += (drive_f + slope_fwd) * fwd_z * ground_factor;
    fx_acc += slope_side * right_x * ground_factor;
    fz_acc += slope_side * right_z * ground_factor;

    // Lateral friction (prevents sliding sideways)
    float speed_lat = vx*right_x + vz*right_z;
    float lat_f = -speed_lat * LAT_FRIC * MASS * ground_factor;
    fx_acc += lat_f * right_x;
    fz_acc += lat_f * right_z;

    // Aerodynamic drag
    float spd = sqrtf(vx*vx + vz*vz);
    float drag = -(DRAG_LIN * spd + DRAG_QUAD * spd * spd);
    if (spd > 0.001f) {
        fx_acc += drag * vx / spd;
        fz_acc += drag * vz / spd;
    }

    // ── Steering yaw rate ─────────────────────────────────────────────
    float target_steer = steer_in * MAX_STEER;
    sang += (target_steer - sang) * 10.0f * dt;  // smooth steer

    float steer_rad = sang * 3.14159f / 180.0f;
    float turn_radius = (fabsf(steer_rad) > 0.001f) ? (WHEEL_BASE * 2.0f / tanf(fabsf(steer_rad))) : 1e6f;
    float yaw_rate = (speed_fwd / turn_radius) * (steer_rad > 0 ? 1.0f : -1.0f) * 180.0f / 3.14159f;
    yaw_rate *= ground_factor;

    // ── Integrate ─────────────────────────────────────────────────────
    vx += (fx_acc / MASS) * dt;
    vy += (fy_acc / MASS) * dt;
    vz += (fz_acc / MASS) * dt;

    x += vx * dt;
    y += vy * dt;
    z += vz * dt;
    yaw += yaw_rate * dt;

    // ── Ground clamp (after integration) ─────────────────────────────
    float ground_y_center = terrain_h(heights, subdiv, tsize, hscale, x, z);
    float rest_y = ground_y_center + SUSP_REST + 0.05f;
    if (y < rest_y) {
        y = rest_y;
        if (vy < 0) vy = 0;
    }

    // ── Terrain bounds ────────────────────────────────────────────────
    float half = tsize * 0.5f - 2.0f;
    if (x >  half) { x =  half; vx = 0; }
    if (x < -half) { x = -half; vx = 0; }
    if (z >  half) { z =  half; vz = 0; }
    if (z < -half) { z = -half; vz = 0; }

    // ── Tree collision ────────────────────────────────────────────────
    for (int ti = 0; ti < ntrees; ti++) {
        float tx = tree_xs[ti], tz = tree_zs[ti];
        float dx = x - tx, dz = z - tz;
        float dist = sqrtf(dx*dx + dz*dz);
        float min_dist = CAR_R + TREE_R;
        if (dist < min_dist && dist > 0.001f) {
            // Push car out
            float push = (min_dist - dist) / dist;
            x += dx * push;
            z += dz * push;
            // Kill velocity component toward tree
            float dot = vx*(dx/dist) + vz*(dz/dist);
            if (dot < 0) {
                vx -= dot * (dx/dist) * 0.8f;
                vz -= dot * (dz/dist) * 0.8f;
            }
        }
    }

    // ── Pitch / roll from suspension torques ──────────────────────────
    float target_pitch = 0.0f, target_roll = 0.0f;
    if (wheels_grounded >= 2) {
        target_pitch = -atanf(tnz) * 180.0f / 3.14159f;
        target_roll  =  atanf(tnx) * 180.0f / 3.14159f;
    }
    pitch += (target_pitch - pitch) * 6.0f * dt;
    roll  += (target_roll  - roll)  * 6.0f * dt;

    // ── Wheel rotation ────────────────────────────────────────────────
    wrot += speed_fwd * dt * 60.0f;

    // Pack output
    out[0]=x;  out[1]=y;  out[2]=z;  out[3]=yaw;
    out[4]=vx; out[5]=vy; out[6]=vz; out[7]=yaw_rate;
    out[8]=pitch; out[9]=roll; out[10]=wrot; out[11]=sang;
}

} // extern "C"
