#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Shader Module — M13 Milestone 13
// ============================================================

void*       shader_create(const char* vert_src, const char* frag_src);
void        shader_destroy(void* prog);
void        shader_use(void* prog);
void        shader_set_uniform_float(void* prog, const char* name, float v);
void        shader_set_uniform_vec3(void* prog, const char* name, float x, float y, float z);
void        shader_set_uniform_mat4(void* prog, const char* name, const float* m16);
const char* shader_error(void);

#ifdef __cplusplus
}
#endif
