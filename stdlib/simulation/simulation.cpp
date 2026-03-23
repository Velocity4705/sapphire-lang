#include "simulation.h"
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <algorithm>

// ============================================================
// Internal structs
// ============================================================

struct NBody3D {
    double x = 0, y = 0, z = 0;
    double vx = 0, vy = 0, vz = 0;
    double ax = 0, ay = 0, az = 0;  // half-step accelerations for leapfrog
    double mass = 1;
};

struct NBodySim {
    double G = 6.674e-11;
    double eps = 1e6;   // softening length
    std::vector<NBody3D> bodies;
    bool leapfrog_initialized = false;
};

struct SPHParticle {
    double x = 0, y = 0;
    double vx = 0, vy = 0;
    double density = 0, pressure = 0;
};

struct FluidSim {
    double W = 1, H = 1;
    double mu = 0.1;
    double h = 0.05;   // smoothing radius
    double rest_density = 1000.0;
    double k_pressure = 200.0;
    std::vector<SPHParticle> particles;
};

struct BHParticle {
    double x = 0, y = 0;
    double vx = 0, vy = 0;
    bool captured = false;
};

struct BlackHoleSim {
    double mass = 1e30;
    double rs = 1.0;   // Schwarzschild radius
    double G = 6.674e-11;
    std::vector<BHParticle> particles;
};

struct FieldParticle {
    double x = 0, y = 0;
    double vx = 0, vy = 0;
};

struct ForceSource {
    double x = 0, y = 0;
    double strength = 1;
    bool repulsor = false;
};

struct FieldSim {
    double xmin = 0, ymin = 0, xmax = 1, ymax = 1;
    std::vector<FieldParticle> particles;
    std::vector<ForceSource> sources;
};

// ============================================================
// N-Body API
// ============================================================

extern "C" {

void* nbody_create(double G, double softening) {
    auto* s = new NBodySim();
    s->G = G;
    s->eps = softening;
    return s;
}

void nbody_destroy(void* sim) {
    if (!sim) return;
    delete static_cast<NBodySim*>(sim);
}

int nbody_add_body(void* sim, double mass,
                   double x, double y, double z,
                   double vx, double vy, double vz) {
    if (!sim) return -1;
    auto* s = static_cast<NBodySim*>(sim);
    NBody3D b;
    b.mass = mass; b.x = x; b.y = y; b.z = z;
    b.vx = vx; b.vy = vy; b.vz = vz;
    int idx = (int)s->bodies.size();
    s->bodies.push_back(b);
    s->leapfrog_initialized = false;
    return idx;
}

// Compute accelerations for all bodies (O(N^2) with softening)
static void nbody_compute_accel(NBodySim* s) {
    int n = (int)s->bodies.size();
    for (int i = 0; i < n; i++) {
        s->bodies[i].ax = 0; s->bodies[i].ay = 0; s->bodies[i].az = 0;
    }
    double eps2 = s->eps * s->eps;
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            double dx = s->bodies[j].x - s->bodies[i].x;
            double dy = s->bodies[j].y - s->bodies[i].y;
            double dz = s->bodies[j].z - s->bodies[i].z;
            double r2 = dx*dx + dy*dy + dz*dz + eps2;
            double r  = std::sqrt(r2);
            double r3 = r2 * r;
            double fij = s->G / r3;
            s->bodies[i].ax += fij * s->bodies[j].mass * dx;
            s->bodies[i].ay += fij * s->bodies[j].mass * dy;
            s->bodies[i].az += fij * s->bodies[j].mass * dz;
            s->bodies[j].ax -= fij * s->bodies[i].mass * dx;
            s->bodies[j].ay -= fij * s->bodies[i].mass * dy;
            s->bodies[j].az -= fij * s->bodies[i].mass * dz;
        }
    }
}

void nbody_step(void* sim, double dt) {
    if (!sim || dt <= 0) return;
    auto* s = static_cast<NBodySim*>(sim);
    if (s->bodies.empty()) return;

    // Leapfrog (Störmer-Verlet) integrator
    if (!s->leapfrog_initialized) {
        // Kick: v_{-1/2} = v_0 - a_0 * dt/2
        nbody_compute_accel(s);
        for (auto& b : s->bodies) {
            b.vx -= b.ax * dt * 0.5;
            b.vy -= b.ay * dt * 0.5;
            b.vz -= b.az * dt * 0.5;
        }
        s->leapfrog_initialized = true;
    }

    // Drift: x_{n+1} = x_n + v_{n+1/2} * dt
    for (auto& b : s->bodies) {
        b.x += b.vx * dt;
        b.y += b.vy * dt;
        b.z += b.vz * dt;
    }

    // Kick: v_{n+3/2} = v_{n+1/2} + a_{n+1} * dt
    nbody_compute_accel(s);
    for (auto& b : s->bodies) {
        b.vx += b.ax * dt;
        b.vy += b.ay * dt;
        b.vz += b.az * dt;
    }
}

void nbody_get_position(void* sim, int idx, double* x, double* y, double* z) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    if (!sim || idx < 0) {
        if (x) *x = nan; if (y) *y = nan; if (z) *z = nan; return;
    }
    auto* s = static_cast<NBodySim*>(sim);
    if (idx >= (int)s->bodies.size()) {
        if (x) *x = nan; if (y) *y = nan; if (z) *z = nan; return;
    }
    if (x) *x = s->bodies[idx].x;
    if (y) *y = s->bodies[idx].y;
    if (z) *z = s->bodies[idx].z;
}

double nbody_get_x(void* sim, int idx) {
    if (!sim || idx < 0) return std::numeric_limits<double>::quiet_NaN();
    auto* s = static_cast<NBodySim*>(sim);
    if (idx >= (int)s->bodies.size()) return std::numeric_limits<double>::quiet_NaN();
    return s->bodies[idx].x;
}

double nbody_get_y(void* sim, int idx) {
    if (!sim || idx < 0) return std::numeric_limits<double>::quiet_NaN();
    auto* s = static_cast<NBodySim*>(sim);
    if (idx >= (int)s->bodies.size()) return std::numeric_limits<double>::quiet_NaN();
    return s->bodies[idx].y;
}

double nbody_total_energy(void* sim) {    if (!sim) return 0.0;
    auto* s = static_cast<NBodySim*>(sim);
    int n = (int)s->bodies.size();
    double E = 0;
    // Kinetic energy (using half-step velocities — approximate)
    for (int i = 0; i < n; i++) {
        double v2 = s->bodies[i].vx*s->bodies[i].vx
                  + s->bodies[i].vy*s->bodies[i].vy
                  + s->bodies[i].vz*s->bodies[i].vz;
        E += 0.5 * s->bodies[i].mass * v2;
    }
    // Potential energy
    double eps2 = s->eps * s->eps;
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            double dx = s->bodies[j].x - s->bodies[i].x;
            double dy = s->bodies[j].y - s->bodies[i].y;
            double dz = s->bodies[j].z - s->bodies[i].z;
            double r = std::sqrt(dx*dx + dy*dy + dz*dz + eps2);
            E -= s->G * s->bodies[i].mass * s->bodies[j].mass / r;
        }
    }
    return E;
}

// ============================================================
// Fluid (SPH) API
// ============================================================

// Poly6 kernel
static double poly6(double r2, double h) {
    double h2 = h*h;
    if (r2 > h2) return 0.0;
    double d = h2 - r2;
    return (315.0 / (64.0 * M_PI * std::pow(h, 9))) * d * d * d;
}

// Spiky kernel gradient magnitude
static double spiky_grad(double r, double h) {
    if (r > h || r < 1e-12) return 0.0;
    double d = h - r;
    return -(45.0 / (M_PI * std::pow(h, 6))) * d * d;
}

// Viscosity kernel laplacian
static double visc_lap(double r, double h) {
    if (r > h) return 0.0;
    return (45.0 / (M_PI * std::pow(h, 6))) * (h - r);
}

void* fluid_create(double width, double height, int particle_count) {
    if (particle_count <= 0 || width <= 0 || height <= 0) return nullptr;
    auto* s = new FluidSim();
    s->W = width; s->H = height;
    s->h = std::min(width, height) * 0.05;
    s->particles.resize(particle_count);
    // Distribute particles in a grid
    int cols = (int)std::ceil(std::sqrt((double)particle_count * width / height));
    int rows = (particle_count + cols - 1) / cols;
    double dx = width  / (cols + 1);
    double dy = height / (rows + 1);
    for (int i = 0; i < particle_count; i++) {
        int col = i % cols, row = i / cols;
        s->particles[i].x = dx * (col + 1);
        s->particles[i].y = dy * (row + 1);
    }
    return s;
}

void fluid_destroy(void* sim) {
    if (!sim) return;
    delete static_cast<FluidSim*>(sim);
}

void fluid_step(void* sim, double dt) {
    if (!sim || dt <= 0) return;
    auto* s = static_cast<FluidSim*>(sim);
    int n = (int)s->particles.size();
    double h = s->h, h2 = h*h;

    // Compute density and pressure
    for (int i = 0; i < n; i++) {
        double rho = 0;
        for (int j = 0; j < n; j++) {
            double dx = s->particles[i].x - s->particles[j].x;
            double dy = s->particles[i].y - s->particles[j].y;
            double r2 = dx*dx + dy*dy;
            rho += poly6(r2, h);
        }
        s->particles[i].density = rho;
        s->particles[i].pressure = s->k_pressure * (rho - s->rest_density);
    }

    // Compute forces and integrate
    for (int i = 0; i < n; i++) {
        double fx = 0, fy = 0;
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            double dx = s->particles[i].x - s->particles[j].x;
            double dy = s->particles[i].y - s->particles[j].y;
            double r2 = dx*dx + dy*dy;
            if (r2 > h2 || r2 < 1e-24) continue;
            double r = std::sqrt(r2);

            // Pressure force
            double pij = (s->particles[i].pressure + s->particles[j].pressure) * 0.5;
            double sg = spiky_grad(r, h);
            double rho_j = s->particles[j].density > 1e-6 ? s->particles[j].density : 1e-6;
            fx += -pij * sg * (dx / r) / rho_j;
            fy += -pij * sg * (dy / r) / rho_j;

            // Viscosity force
            double vl = visc_lap(r, h);
            fx += s->mu * (s->particles[j].vx - s->particles[i].vx) * vl / rho_j;
            fy += s->mu * (s->particles[j].vy - s->particles[i].vy) * vl / rho_j;
        }

        // Gravity
        fy -= 9.81;

        double rho_i = s->particles[i].density > 1e-6 ? s->particles[i].density : 1e-6;
        s->particles[i].vx += (fx / rho_i) * dt;
        s->particles[i].vy += (fy / rho_i) * dt;
        s->particles[i].x  += s->particles[i].vx * dt;
        s->particles[i].y  += s->particles[i].vy * dt;

        // Boundary reflection
        if (s->particles[i].x < 0)    { s->particles[i].x = 0;    s->particles[i].vx *= -0.5; }
        if (s->particles[i].x > s->W) { s->particles[i].x = s->W; s->particles[i].vx *= -0.5; }
        if (s->particles[i].y < 0)    { s->particles[i].y = 0;    s->particles[i].vy *= -0.5; }
        if (s->particles[i].y > s->H) { s->particles[i].y = s->H; s->particles[i].vy *= -0.5; }
    }
}

void fluid_get_particle_position(void* sim, int idx, double* x, double* y) {
    if (!sim || idx < 0) { if (x) *x = 0; if (y) *y = 0; return; }
    auto* s = static_cast<FluidSim*>(sim);
    if (idx >= (int)s->particles.size()) { if (x) *x = 0; if (y) *y = 0; return; }
    if (x) *x = s->particles[idx].x;
    if (y) *y = s->particles[idx].y;
}

void fluid_set_viscosity(void* sim, double mu) {
    if (!sim) return;
    static_cast<FluidSim*>(sim)->mu = mu;
}

double fluid_get_particle_x(void* sim, int idx) {
    if (!sim || idx < 0) return 0;
    auto* s = static_cast<FluidSim*>(sim);
    if (idx >= (int)s->particles.size()) return 0;
    return s->particles[idx].x;
}

double fluid_get_particle_y(void* sim, int idx) {
    if (!sim || idx < 0) return 0;
    auto* s = static_cast<FluidSim*>(sim);
    if (idx >= (int)s->particles.size()) return 0;
    return s->particles[idx].y;
}

// ============================================================
// Black Hole API
// ============================================================

void* blackhole_create(double mass, double schwarzschild_radius) {
    auto* s = new BlackHoleSim();
    s->mass = mass;
    s->rs = schwarzschild_radius;
    return s;
}

void blackhole_destroy(void* sim) {
    if (!sim) return;
    delete static_cast<BlackHoleSim*>(sim);
}

int blackhole_add_particle(void* sim, double x, double y, double vx, double vy) {
    if (!sim) return -1;
    auto* s = static_cast<BlackHoleSim*>(sim);
    BHParticle p;
    p.x = x; p.y = y; p.vx = vx; p.vy = vy;
    int idx = (int)s->particles.size();
    s->particles.push_back(p);
    return idx;
}

void blackhole_step(void* sim, double dt) {
    if (!sim || dt <= 0) return;
    auto* s = static_cast<BlackHoleSim*>(sim);
    for (auto& p : s->particles) {
        if (p.captured) continue;
        double r2 = p.x*p.x + p.y*p.y;
        double r  = std::sqrt(r2);
        if (r <= s->rs) { p.captured = true; continue; }

        // Post-Newtonian Schwarzschild approximation:
        // a = -G*M/r^2 * (1 - rs/r) * r_hat
        double factor = s->G * s->mass / (r2 * r) * (1.0 - s->rs / r);
        double ax = -factor * p.x;
        double ay = -factor * p.y;

        p.vx += ax * dt;
        p.vy += ay * dt;
        p.x  += p.vx * dt;
        p.y  += p.vy * dt;

        // Check capture after move
        if (p.x*p.x + p.y*p.y <= s->rs * s->rs) p.captured = true;
    }
}

void blackhole_get_particle_position(void* sim, int idx, double* x, double* y) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    if (!sim || idx < 0) { if (x) *x = nan; if (y) *y = nan; return; }
    auto* s = static_cast<BlackHoleSim*>(sim);
    if (idx >= (int)s->particles.size()) { if (x) *x = nan; if (y) *y = nan; return; }
    if (s->particles[idx].captured) { if (x) *x = nan; if (y) *y = nan; return; }
    if (x) *x = s->particles[idx].x;
    if (y) *y = s->particles[idx].y;
}

int blackhole_captured_count(void* sim) {
    if (!sim) return 0;
    auto* s = static_cast<BlackHoleSim*>(sim);
    int n = 0;
    for (auto& p : s->particles) if (p.captured) n++;
    return n;
}

double blackhole_get_particle_x(void* sim, int idx) {
    if (!sim || idx < 0) return std::numeric_limits<double>::quiet_NaN();
    auto* s = static_cast<BlackHoleSim*>(sim);
    if (idx >= (int)s->particles.size() || s->particles[idx].captured)
        return std::numeric_limits<double>::quiet_NaN();
    return s->particles[idx].x;
}

double blackhole_get_particle_y(void* sim, int idx) {
    if (!sim || idx < 0) return std::numeric_limits<double>::quiet_NaN();
    auto* s = static_cast<BlackHoleSim*>(sim);
    if (idx >= (int)s->particles.size() || s->particles[idx].captured)
        return std::numeric_limits<double>::quiet_NaN();
    return s->particles[idx].y;
}

// ============================================================
// Particle Field API
// ============================================================

void* field_sim_create(int particle_count,
                        double xmin, double ymin, double xmax, double ymax) {
    if (particle_count <= 0) return nullptr;
    auto* s = new FieldSim();
    s->xmin = xmin; s->ymin = ymin; s->xmax = xmax; s->ymax = ymax;
    s->particles.resize(particle_count);
    double rx = xmax - xmin, ry = ymax - ymin;
    for (int i = 0; i < particle_count; i++) {
        s->particles[i].x = xmin + rx * ((double)rand() / RAND_MAX);
        s->particles[i].y = ymin + ry * ((double)rand() / RAND_MAX);
    }
    return s;
}

void field_sim_destroy(void* sim) {
    if (!sim) return;
    delete static_cast<FieldSim*>(sim);
}

void field_sim_add_attractor(void* sim, double x, double y, double strength) {
    if (!sim) return;
    auto* s = static_cast<FieldSim*>(sim);
    ForceSource fs; fs.x = x; fs.y = y; fs.strength = strength; fs.repulsor = false;
    s->sources.push_back(fs);
}

void field_sim_add_repulsor(void* sim, double x, double y, double strength) {
    if (!sim) return;
    auto* s = static_cast<FieldSim*>(sim);
    ForceSource fs; fs.x = x; fs.y = y; fs.strength = strength; fs.repulsor = true;
    s->sources.push_back(fs);
}

void field_sim_step(void* sim, double dt) {
    if (!sim || dt <= 0) return;
    auto* s = static_cast<FieldSim*>(sim);
    for (auto& p : s->particles) {
        double ax = 0, ay = 0;
        for (auto& src : s->sources) {
            double dx = src.x - p.x, dy = src.y - p.y;
            double r2 = dx*dx + dy*dy;
            if (r2 < 1e-12) continue;
            double r = std::sqrt(r2);
            double f = src.strength / r2;
            if (src.repulsor) f = -f;
            ax += f * dx / r;
            ay += f * dy / r;
        }
        p.vx += ax * dt;
        p.vy += ay * dt;
        p.x  += p.vx * dt;
        p.y  += p.vy * dt;
    }
}

void field_sim_get_particle_position(void* sim, int idx, double* x, double* y) {
    if (!sim || idx < 0) { if (x) *x = 0; if (y) *y = 0; return; }
    auto* s = static_cast<FieldSim*>(sim);
    if (idx >= (int)s->particles.size()) { if (x) *x = 0; if (y) *y = 0; return; }
    if (x) *x = s->particles[idx].x;
    if (y) *y = s->particles[idx].y;
}

double field_sim_get_particle_x(void* sim, int idx) {
    if (!sim || idx < 0) return 0;
    auto* s = static_cast<FieldSim*>(sim);
    if (idx >= (int)s->particles.size()) return 0;
    return s->particles[idx].x;
}

double field_sim_get_particle_y(void* sim, int idx) {
    if (!sim || idx < 0) return 0;
    auto* s = static_cast<FieldSim*>(sim);
    if (idx >= (int)s->particles.size()) return 0;
    return s->particles[idx].y;
}

} // extern "C"
