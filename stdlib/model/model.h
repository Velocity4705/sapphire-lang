#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Model Module — M13 Milestone 13
// ============================================================

void*       model_load(const char* obj_path);
void        model_destroy(void* handle);
void        model_draw(void* handle, void* shader);   // no-op headless
int         model_vertex_count(void* handle);
int         model_face_count(void* handle);
const char* model_error(void);

#ifdef __cplusplus
}
#endif
