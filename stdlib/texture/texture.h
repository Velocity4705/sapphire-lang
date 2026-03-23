#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Texture Module — M13 Milestone 13
// ============================================================

// Texture
void*       texture_load(const char* path);
void        texture_destroy(void* handle);
int         texture_width(void* handle);
int         texture_height(void* handle);
void        texture_bind(void* handle, int unit);   // no-op headless
const char* texture_error(void);

// Sprite Sheet
void*  spritesheet_create(void* tex, int fw, int fh, int count);
void   spritesheet_destroy(void* ss);
void   spritesheet_frame_uv(void* ss, int frame,
                             float* u0, float* v0, float* u1, float* v1);

// Texture Atlas
void*       atlas_load(void* tex, const char* json_path);
void        atlas_destroy(void* atlas);
void        atlas_region_uv(void* atlas, const char* name,
                             float* u0, float* v0, float* u1, float* v1);
const char* atlas_error(void);

#ifdef __cplusplus
}
#endif
