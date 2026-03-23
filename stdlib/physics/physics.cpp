#include "physics.h"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// ── Internal structs ───────────────────────────────────────────────────────

struct PhysicsWorld;

enum ColliderType { COLLIDER_NONE, COLLIDER_BOX, COLLIDER_CIRCLE, COLLIDER_CONVEX };

struct Vec2f { float x = 0, y = 0; };

struct Collider {
    ColliderType type = COLLIDER_NONE;
    float w = 0, h = 0, radius = 0;
    std::vector<Vec2f> hull;
};

struct RigidBody {
    float x = 0, y = 0;
    float vx = 0, vy = 0;
    float fx = 0, fy = 0;
    float mass = 1, inv_mass = 1;
    float restitution = 0.5f;
    bool is_static = false;
    Collider collider;
    PhysicsWorld* world = nullptr;
};

struct Constraint {
    enum Type { DISTANCE, SPRING, HINGE } type = DISTANCE;
    RigidBody* a = nullptr;
    RigidBody* b = nullptr;
    float rest_len = 0, k = 0;
    float ax = 0, ay = 0;
};

struct Particle {
    float x = 0, y = 0;
    float vx = 0, vy = 0;
    float life = 0, max_life = 2.0f;
    float r = 1, g = 1, b = 1;
    bool active = false;
};

struct ParticleSystem {
    float ex = 0, ey = 0;
    int max_count = 0;
    std::vector<Particle> pool;
    float cr0=1,cg0=1,cb0=1, cr1=1,cg1=1,cb1=1;
    PhysicsWorld* world = nullptr;
};

struct PhysicsWorld {
    float gx = 0, gy = -9.81f;
    std::vector<RigidBody*> bodies;
    std::vector<Constraint*> constraints;
    std::vector<ParticleSystem*> particle_systems;
};

static std::string s_physics_error;

// ── Collision helpers ──────────────────────────────────────────────────────

static bool aabb_overlap(const RigidBody* a, const RigidBody* b) {
    float ax0 = a->x - a->collider.w * 0.5f, ax1 = a->x + a->collider.w * 0.5f;
    float ay0 = a->y - a->collider.h * 0.5f, ay1 = a->y + a->collider.h * 0.5f;
    float bx0 = b->x - b->collider.w * 0.5f, bx1 = b->x + b->collider.w * 0.5f;
    float by0 = b->y - b->collider.h * 0.5f, by1 = b->y + b->collider.h * 0.5f;
    return ax0 < bx1 && ax1 > bx0 && ay0 < by1 && ay1 > by0;
}

static bool circle_overlap(const RigidBody* a, const RigidBody* b) {
    float dx = a->x - b->x, dy = a->y - b->y;
    float dist2 = dx*dx + dy*dy;
    float rsum = a->collider.radius + b->collider.radius;
    return dist2 < rsum * rsum;
}

static bool box_circle_overlap(const RigidBody* box, const RigidBody* circ) {
    float cx = circ->x, cy = circ->y;
    float bx0 = box->x - box->collider.w*0.5f, bx1 = box->x + box->collider.w*0.5f;
    float by0 = box->y - box->collider.h*0.5f, by1 = box->y + box->collider.h*0.5f;
    float nearX = std::max(bx0, std::min(cx, bx1));
    float nearY = std::max(by0, std::min(cy, by1));
    float dx = cx - nearX, dy = cy - nearY;
    return dx*dx + dy*dy < circ->collider.radius * circ->collider.radius;
}

static bool bodies_overlap(const RigidBody* a, const RigidBody* b) {
    auto ta = a->collider.type, tb = b->collider.type;
    if (ta == COLLIDER_NONE || tb == COLLIDER_NONE) return false;
    if (ta == COLLIDER_BOX    && tb == COLLIDER_BOX)    return aabb_overlap(a, b);
    if (ta == COLLIDER_CIRCLE && tb == COLLIDER_CIRCLE) return circle_overlap(a, b);
    if (ta == COLLIDER_BOX    && tb == COLLIDER_CIRCLE) return box_circle_overlap(a, b);
    if (ta == COLLIDER_CIRCLE && tb == COLLIDER_BOX)    return box_circle_overlap(b, a);
    // convex: approximate as AABB
    if (ta == COLLIDER_CONVEX || tb == COLLIDER_CONVEX) return aabb_overlap(a, b);
    return false;
}

// Resolve collision between two bodies using impulse method
static void resolve_collision(RigidBody* a, RigidBody* b) {
    if (!bodies_overlap(a, b)) return;
    if (a->is_static && b->is_static) return;

    float dx = b->x - a->x, dy = b->y - a->y;
    float dist = std::sqrt(dx*dx + dy*dy);
    if (dist < 1e-6f) { dx = 1; dy = 0; dist = 1; }
    float nx = dx/dist, ny = dy/dist;

    float rvx = b->vx - a->vx, rvy = b->vy - a->vy;
    float vel_along_normal = rvx*nx + rvy*ny;
    if (vel_along_normal > 0) return; // separating

    float e = std::min(a->restitution, b->restitution);
    float j = -(1 + e) * vel_along_normal / (a->inv_mass + b->inv_mass);

    if (!a->is_static) { a->vx -= j * a->inv_mass * nx; a->vy -= j * a->inv_mass * ny; }
    if (!b->is_static) { b->vx += j * b->inv_mass * nx; b->vy += j * b->inv_mass * ny; }

    // Positional correction (prevent sinking)
    float overlap = 0;
    if (a->collider.type == COLLIDER_BOX && b->collider.type == COLLIDER_BOX) {
        float ox = (a->collider.w + b->collider.w)*0.5f - std::abs(dx);
        float oy = (a->collider.h + b->collider.h)*0.5f - std::abs(dy);
        overlap = std::min(ox, oy);
    } else if (a->collider.type == COLLIDER_CIRCLE && b->collider.type == COLLIDER_CIRCLE) {
        overlap = (a->collider.radius + b->collider.radius) - dist;
    } else {
        overlap = 0.01f;
    }
    if (overlap > 0) {
        float corr = overlap * 0.5f;
        if (!a->is_static) { a->x -= corr*nx; a->y -= corr*ny; }
        if (!b->is_static) { b->x += corr*nx; b->y += corr*ny; }
    }
}

// ── Physics World ──────────────────────────────────────────────────────────

extern "C" {

void* physics_world_create(float gx, float gy) {
    auto* w = new PhysicsWorld();
    w->gx = gx; w->gy = gy;
    return w;
}

void physics_world_destroy(void* world) {
    if (!world) return;
    auto* w = static_cast<PhysicsWorld*>(world);
    for (auto* b : w->bodies) delete b;
    for (auto* c : w->constraints) delete c;
    for (auto* ps : w->particle_systems) delete ps;
    delete w;
}

void physics_world_step(void* world, float dt) {
    if (!world || dt <= 0) return;
    auto* w = static_cast<PhysicsWorld*>(world);

    // Apply constraints
    for (auto* c : w->constraints) {
        if (!c->a || !c->b) continue;
        float dx = c->b->x - c->a->x, dy = c->b->y - c->a->y;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist < 1e-6f) continue;
        float nx = dx/dist, ny = dy/dist;

        if (c->type == Constraint::DISTANCE) {
            float diff = dist - c->rest_len;
            float corr = diff * 0.5f;
            if (!c->a->is_static) { c->a->x += corr*nx; c->a->y += corr*ny; }
            if (!c->b->is_static) { c->b->x -= corr*nx; c->b->y -= corr*ny; }
        } else if (c->type == Constraint::SPRING) {
            float force = c->k * (dist - c->rest_len);
            if (!c->a->is_static) { c->a->fx += force*nx; c->a->fy += force*ny; }
            if (!c->b->is_static) { c->b->fx -= force*nx; c->b->fy -= force*ny; }
        }
    }

    // Integrate bodies
    for (auto* b : w->bodies) {
        if (b->is_static) continue;
        // gravity
        b->fx += b->mass * w->gx;
        b->fy += b->mass * w->gy;
        // integrate velocity
        b->vx += (b->fx * b->inv_mass) * dt;
        b->vy += (b->fy * b->inv_mass) * dt;
        // integrate position
        b->x += b->vx * dt;
        b->y += b->vy * dt;
        // clear forces
        b->fx = 0; b->fy = 0;
    }

    // Collision detection & response
    for (size_t i = 0; i < w->bodies.size(); i++)
        for (size_t j = i+1; j < w->bodies.size(); j++)
            resolve_collision(w->bodies[i], w->bodies[j]);
}

// ── RigidBody ──────────────────────────────────────────────────────────────

void* rigidbody_create(void* world, float x, float y, float mass) {
    if (!world) { s_physics_error = "rigidbody_create: null world"; return nullptr; }
    if (mass < 0) { s_physics_error = "rigidbody_create: negative mass"; return nullptr; }
    auto* b = new RigidBody();
    b->x = x; b->y = y;
    b->mass = mass;
    b->is_static = (mass == 0.0f);
    b->inv_mass  = b->is_static ? 0.0f : 1.0f / mass;
    b->world = static_cast<PhysicsWorld*>(world);
    static_cast<PhysicsWorld*>(world)->bodies.push_back(b);
    return b;
}

void rigidbody_destroy(void* body) {
    if (!body) return;
    auto* b = static_cast<RigidBody*>(body);
    if (b->world) {
        auto& bv = b->world->bodies;
        bv.erase(std::remove(bv.begin(), bv.end(), b), bv.end());
    }
    delete b;
}

void rigidbody_apply_force(void* body, float fx, float fy) {
    if (!body) return;
    auto* b = static_cast<RigidBody*>(body);
    if (!b->is_static) { b->fx += fx; b->fy += fy; }
}

void rigidbody_apply_impulse(void* body, float ix, float iy) {
    if (!body) return;
    auto* b = static_cast<RigidBody*>(body);
    if (!b->is_static) { b->vx += ix * b->inv_mass; b->vy += iy * b->inv_mass; }
}

float rigidbody_get_x(void* body)  { return body ? static_cast<RigidBody*>(body)->x  : 0; }
float rigidbody_get_y(void* body)  { return body ? static_cast<RigidBody*>(body)->y  : 0; }
float rigidbody_get_vx(void* body) { return body ? static_cast<RigidBody*>(body)->vx : 0; }
float rigidbody_get_vy(void* body) { return body ? static_cast<RigidBody*>(body)->vy : 0; }

// ── Colliders ──────────────────────────────────────────────────────────────

void collider_add_box(void* body, float w, float h) {
    if (!body) return;
    auto* b = static_cast<RigidBody*>(body);
    b->collider.type = COLLIDER_BOX;
    b->collider.w = w; b->collider.h = h;
}

void collider_add_circle(void* body, float radius) {
    if (!body) return;
    auto* b = static_cast<RigidBody*>(body);
    b->collider.type = COLLIDER_CIRCLE;
    b->collider.radius = radius;
}

void collider_add_convex_hull(void* body, const float* verts, int count) {
    if (!body || !verts || count < 3) return;
    auto* b = static_cast<RigidBody*>(body);
    b->collider.type = COLLIDER_CONVEX;
    b->collider.hull.clear();
    float minx=1e9,miny=1e9,maxx=-1e9,maxy=-1e9;
    for (int i = 0; i < count; i++) {
        Vec2f v{verts[i*2], verts[i*2+1]};
        b->collider.hull.push_back(v);
        minx=std::min(minx,v.x); miny=std::min(miny,v.y);
        maxx=std::max(maxx,v.x); maxy=std::max(maxy,v.y);
    }
    // Store AABB for approximate collision
    b->collider.w = maxx - minx;
    b->collider.h = maxy - miny;
}

int collision_query_pair(void* a, void* b) {
    if (!a || !b) return 0;
    return bodies_overlap(static_cast<RigidBody*>(a),
                          static_cast<RigidBody*>(b)) ? 1 : 0;
}

// ── Constraints ────────────────────────────────────────────────────────────

void* constraint_add_distance(void* world, void* a, void* b, float rest_len) {
    if (!world || !a || !b) return nullptr;
    auto* w = static_cast<PhysicsWorld*>(world);
    if ((int)w->constraints.size() >= 1024) {
        s_physics_error = "constraint_add_distance: max 1024 constraints reached";
        return nullptr;
    }
    auto* c = new Constraint();
    c->type = Constraint::DISTANCE;
    c->a = static_cast<RigidBody*>(a);
    c->b = static_cast<RigidBody*>(b);
    c->rest_len = rest_len;
    w->constraints.push_back(c);
    return c;
}

void* constraint_add_spring(void* world, void* a, void* b, float rest_len, float k) {
    if (!world || !a || !b) return nullptr;
    auto* w = static_cast<PhysicsWorld*>(world);
    if ((int)w->constraints.size() >= 1024) {
        s_physics_error = "constraint_add_spring: max 1024 constraints reached";
        return nullptr;
    }
    auto* c = new Constraint();
    c->type = Constraint::SPRING;
    c->a = static_cast<RigidBody*>(a);
    c->b = static_cast<RigidBody*>(b);
    c->rest_len = rest_len; c->k = k;
    w->constraints.push_back(c);
    return c;
}

void* constraint_add_hinge(void* world, void* a, void* b, float ax, float ay) {
    if (!world || !a || !b) return nullptr;
    auto* w = static_cast<PhysicsWorld*>(world);
    if ((int)w->constraints.size() >= 1024) {
        s_physics_error = "constraint_add_hinge: max 1024 constraints reached";
        return nullptr;
    }
    auto* c = new Constraint();
    c->type = Constraint::HINGE;
    c->a = static_cast<RigidBody*>(a);
    c->b = static_cast<RigidBody*>(b);
    c->ax = ax; c->ay = ay;
    w->constraints.push_back(c);
    return c;
}

void constraint_destroy(void* world, void* constraint) {
    if (!constraint) return;
    if (world) {
        auto& cv = static_cast<PhysicsWorld*>(world)->constraints;
        cv.erase(std::remove(cv.begin(), cv.end(),
                             static_cast<Constraint*>(constraint)), cv.end());
    }
    delete static_cast<Constraint*>(constraint);
}

// ── Particle Systems ───────────────────────────────────────────────────────

void* particles_create(void* world, float ex, float ey, int max_count) {
    if (!world || max_count <= 0) return nullptr;
    auto* ps = new ParticleSystem();
    ps->ex = ex; ps->ey = ey;
    ps->max_count = max_count;
    ps->pool.resize(max_count);
    ps->world = static_cast<PhysicsWorld*>(world);
    static_cast<PhysicsWorld*>(world)->particle_systems.push_back(ps);
    return ps;
}

void particles_destroy(void* p) {
    if (!p) return;
    auto* ps = static_cast<ParticleSystem*>(p);
    if (ps->world) {
        auto& pv = ps->world->particle_systems;
        pv.erase(std::remove(pv.begin(), pv.end(), ps), pv.end());
    }
    delete ps;
}

void particles_emit(void* p, int count) {
    if (!p || count <= 0) return;
    auto* ps = static_cast<ParticleSystem*>(p);
    int emitted = 0;
    for (auto& part : ps->pool) {
        if (emitted >= count) break;
        if (!part.active) {
            part.x = ps->ex; part.y = ps->ey;
            // random velocity spread
            part.vx = ((float)(rand() % 200) - 100) * 0.01f;
            part.vy = ((float)(rand() % 100)) * 0.02f + 0.5f;
            part.max_life = 1.5f + ((float)(rand() % 100)) * 0.01f;
            part.life = part.max_life;
            part.r = ps->cr0; part.g = ps->cg0; part.b = ps->cb0;
            part.active = true;
            emitted++;
        }
    }
}

void particles_update(void* p, float dt) {
    if (!p || dt <= 0) return;
    auto* ps = static_cast<ParticleSystem*>(p);
    float gy = ps->world ? ps->world->gy : -9.81f;
    for (auto& part : ps->pool) {
        if (!part.active) continue;
        part.vy += gy * dt;
        part.x += part.vx * dt;
        part.y += part.vy * dt;
        part.life -= dt;
        if (part.life <= 0) { part.active = false; continue; }
        // interpolate color
        float t = 1.0f - (part.life / part.max_life);
        part.r = ps->cr0 + (ps->cr1 - ps->cr0) * t;
        part.g = ps->cg0 + (ps->cg1 - ps->cg0) * t;
        part.b = ps->cb0 + (ps->cb1 - ps->cb0) * t;
    }
}

void particles_set_color_gradient(void* p,
                                   float r0, float g0, float b0,
                                   float r1, float g1, float b1) {
    if (!p) return;
    auto* ps = static_cast<ParticleSystem*>(p);
    ps->cr0=r0; ps->cg0=g0; ps->cb0=b0;
    ps->cr1=r1; ps->cg1=g1; ps->cb1=b1;
}

int particles_active_count(void* p) {
    if (!p) return 0;
    auto* ps = static_cast<ParticleSystem*>(p);
    int n = 0;
    for (auto& part : ps->pool) if (part.active) n++;
    return n;
}

const char* physics_error(void) { return s_physics_error.c_str(); }

} // extern "C"
