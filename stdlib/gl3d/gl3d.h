#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// ── gl3d: SDL2 + OpenGL 3D engine for Sapphire ───────────────────────────
// Window / context
void* gl3d_window_create(const char* title, int w, int h);
void  gl3d_window_destroy(void* win);
int   gl3d_window_poll(void* win);   // 1 = keep running, 0 = quit
void  gl3d_window_swap(void* win);
void  gl3d_window_set_title(void* win, const char* title);
int   gl3d_window_width(void* win);
int   gl3d_window_height(void* win);

// Input
int   gl3d_key(void* win, const char* name);  // 1 if held
int   gl3d_key_pressed(void* win, const char* name); // 1 on first frame
float gl3d_mouse_dx(void* win);
float gl3d_mouse_dy(void* win);
void  gl3d_mouse_capture(void* win, int captured);

// Timing
float gl3d_delta(void* win);   // seconds since last swap
float gl3d_time(void* win);    // seconds since window creation

// Mesh  (VAO/VBO on GPU)
// verts: interleaved [x,y,z, nx,ny,nz, u,v] per vertex
void* gl3d_mesh_create(const float* verts, int vcount,
                        const unsigned int* idx, int icount);
void  gl3d_mesh_destroy(void* mesh);
void  gl3d_mesh_draw(void* mesh);

// Line mesh — flat [x,y,z, x,y,z, ...] pairs, drawn with GL_LINES
// Useful for wireframes, 4D projections, debug overlays
void* gl3d_mesh_lines_create(const float* verts, int vcount);
void  gl3d_mesh_lines_update(void* mesh, const float* verts, int vcount);
void  gl3d_mesh_lines_draw(void* mesh);
void  gl3d_mesh_lines_destroy(void* mesh);

// Shader
void* gl3d_shader_create(const char* vert_src, const char* frag_src);
void  gl3d_shader_destroy(void* sh);
void  gl3d_shader_use(void* sh);
void  gl3d_shader_set_float(void* sh, const char* name, float v);
void  gl3d_shader_set_vec3(void* sh, const char* name, float x, float y, float z);
void  gl3d_shader_set_vec4(void* sh, const char* name, float x, float y, float z, float w);
void  gl3d_shader_set_mat4(void* sh, const char* name, const float* m16);
void  gl3d_shader_set_int(void* sh, const char* name, int v);

// Texture
void* gl3d_texture_load(const char* path);
void* gl3d_texture_solid(unsigned char r, unsigned char g, unsigned char b);
void  gl3d_texture_destroy(void* tex);
void  gl3d_texture_bind(void* tex, int unit);

// Math helpers (return heap-allocated float[16] mat4, caller frees)
float* gl3d_mat4_identity(void);
float* gl3d_mat4_perspective(float fovy_deg, float aspect, float near, float far);
float* gl3d_mat4_lookat(float ex,float ey,float ez, float cx,float cy,float cz, float ux,float uy,float uz);
float* gl3d_mat4_translate(float x, float y, float z);
float* gl3d_mat4_scale(float x, float y, float z);
float* gl3d_mat4_rotate_y(float angle_deg);
float* gl3d_mat4_rotate_x(float angle_deg);
float* gl3d_mat4_rotate_z(float angle_deg);
float* gl3d_mat4_mul(const float* a, const float* b);
void   gl3d_mat4_free(float* m);

// Render state
void  gl3d_clear(float r, float g, float b);
void  gl3d_depth_test(int enabled);
void  gl3d_wireframe(int enabled);
void  gl3d_viewport(int x, int y, int w, int h);

// Built-in mesh generators
void* gl3d_mesh_box(float w, float h, float d);
void* gl3d_mesh_plane(float w, float d, int subdiv);  // flat grid
void* gl3d_mesh_sphere(float r, int slices, int stacks);
void* gl3d_mesh_cylinder(float r, float h, int slices);
// Heightmap terrain: heights[subdiv+1][subdiv+1] row-major
void* gl3d_mesh_terrain(const float* heights, int subdiv, float size, float height_scale);

// Fast 4D world renderer
// cam[20] = {px,py,pz,pw, rx,ry,rz,rw, ux,uy,uz,uw, fx,fy,fz,fw, ax,ay,az,aw}
// cells[ncells*4] = flat list of cell center 4D coords
// Returns a new Gl3dLineMesh* (use gl3d_mesh_lines_draw / gl3d_mesh_lines_destroy)
void* gl3d_world4d_build(const float* cam, const float* cells, int ncells, float w_dist);
void  gl3d_world4d_update(void* mesh, const float* cam, const float* cells, int ncells, float w_dist);

// Car physics step
// state[12]: x,y,z,yaw, vx,vy,vz,vyaw, pitch,roll,wheel_rot,steer_ang
// input[5]:  throttle,brake,steer,handbrake,reset
// heights[]: terrain heightmap (same layout as gl3d_mesh_terrain)
// tree_xs/zs: tree positions for collision
// out[12]: new state
void gl3d_car_step(const float* state, const float* input,
                   const float* heights, int subdiv, float tsize, float hscale,
                   const float* tree_xs, const float* tree_zs, int ntrees,
                   float dt, float* out);

const char* gl3d_error(void);

#ifdef __cplusplus
}
#endif
