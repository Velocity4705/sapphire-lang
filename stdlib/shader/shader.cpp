#include "shader.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>

// ── Internal structs ───────────────────────────────────────────────────────

struct ShaderProgram {
    std::string vert_src;
    std::string frag_src;
    bool compiled = false;
    std::string error_log;
    std::unordered_map<std::string, std::vector<float>> uniforms;
};

static std::string s_shader_error;
static ShaderProgram* s_active = nullptr;

// ── Shader API ─────────────────────────────────────────────────────────────

extern "C" {

void* shader_create(const char* vert_src, const char* frag_src) {
    if (!vert_src || vert_src[0] == '\0') {
        s_shader_error = "shader_create: vertex source is empty";
        return nullptr;
    }
    if (!frag_src || frag_src[0] == '\0') {
        s_shader_error = "shader_create: fragment source is empty";
        return nullptr;
    }
    auto* p = new ShaderProgram();
    p->vert_src  = vert_src;
    p->frag_src  = frag_src;
    p->compiled  = true;
    return p;
}

void shader_destroy(void* prog) {
    if (!prog) return;
    if (s_active == static_cast<ShaderProgram*>(prog)) s_active = nullptr;
    delete static_cast<ShaderProgram*>(prog);
}

void shader_use(void* prog) {
    s_active = static_cast<ShaderProgram*>(prog); // headless: record active
}

void shader_set_uniform_float(void* prog, const char* name, float v) {
    if (!prog || !name) return;
    static_cast<ShaderProgram*>(prog)->uniforms[name] = {v};
}

void shader_set_uniform_vec3(void* prog, const char* name, float x, float y, float z) {
    if (!prog || !name) return;
    static_cast<ShaderProgram*>(prog)->uniforms[name] = {x, y, z};
}

void shader_set_uniform_mat4(void* prog, const char* name, const float* m16) {
    if (!prog || !name || !m16) return;
    static_cast<ShaderProgram*>(prog)->uniforms[name] =
        std::vector<float>(m16, m16 + 16);
}

const char* shader_error(void) { return s_shader_error.c_str(); }

} // extern "C"
