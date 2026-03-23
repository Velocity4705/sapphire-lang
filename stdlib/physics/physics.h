#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Physics Module — M13 Milestone 13
// ============================================================

// World
void*  physics_world_create(float gx, float gy);
void   physics_world_destroy(void* world);
void   physics_world_step(void* world, float dt);

// RigidBody
void*  rigidbody_create(void* world, float x, float y, float mass);
void   rigidbody_destroy(void* body);
void   rigidbody_apply_force(void* body, float fx, float fy);
void   rigidbody_apply_impulse(void* body, float ix, float iy);
float  rigidbody_get_x(void* body);
float  rigidbody_get_y(void* body);
float  rigidbody_get_vx(void* body);
float  rigidbody_get_vy(void* body);

// Colliders
void   collider_add_box(void* body, float w, float h);
void   collider_add_circle(void* body, float radius);
void   collider_add_convex_hull(void* body, const float* verts, int count);
int    collision_query_pair(void* a, void* b);

// Constraints
void*  constraint_add_distance(void* world, void* a, void* b, float rest_len);
void*  constraint_add_spring(void* world, void* a, void* b, float rest_len, float k);
void*  constraint_add_hinge(void* world, void* a, void* b, float ax, float ay);
void   constraint_destroy(void* world, void* constraint);

// Particles
void*  particles_create(void* world, float ex, float ey, int max_count);
void   particles_destroy(void* ps);
void   particles_emit(void* ps, int count);
void   particles_update(void* ps, float dt);
void   particles_set_color_gradient(void* ps,
                                    float r0, float g0, float b0,
                                    float r1, float g1, float b1);
int    particles_active_count(void* ps);

const char* physics_error(void);

#ifdef __cplusplus
}
#endif
