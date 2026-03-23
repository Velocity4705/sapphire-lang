#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Simulation Module — M13 Milestone 13
// ============================================================

// N-Body
void*  nbody_create(double G, double softening);
void   nbody_destroy(void* sim);
int    nbody_add_body(void* sim, double mass,
                      double x, double y, double z,
                      double vx, double vy, double vz);
void   nbody_step(void* sim, double dt);
void   nbody_get_position(void* sim, int idx, double* x, double* y, double* z);
double nbody_get_x(void* sim, int idx);
double nbody_get_y(void* sim, int idx);
double nbody_total_energy(void* sim);

// Fluid (SPH)
void*  fluid_create(double width, double height, int particle_count);
void   fluid_destroy(void* sim);
void   fluid_step(void* sim, double dt);
void   fluid_get_particle_position(void* sim, int idx, double* x, double* y);
double fluid_get_particle_x(void* sim, int idx);
double fluid_get_particle_y(void* sim, int idx);
void   fluid_set_viscosity(void* sim, double mu);

// Black Hole
void*  blackhole_create(double mass, double schwarzschild_radius);
void   blackhole_destroy(void* sim);
int    blackhole_add_particle(void* sim, double x, double y, double vx, double vy);
void   blackhole_step(void* sim, double dt);
void   blackhole_get_particle_position(void* sim, int idx, double* x, double* y);
double blackhole_get_particle_x(void* sim, int idx);
double blackhole_get_particle_y(void* sim, int idx);
int    blackhole_captured_count(void* sim);

// Particle Field
void*  field_sim_create(int particle_count,
                         double xmin, double ymin, double xmax, double ymax);
void   field_sim_destroy(void* sim);
void   field_sim_add_attractor(void* sim, double x, double y, double strength);
void   field_sim_add_repulsor(void* sim, double x, double y, double strength);
void   field_sim_step(void* sim, double dt);
void   field_sim_get_particle_position(void* sim, int idx, double* x, double* y);
double field_sim_get_particle_x(void* sim, int idx);
double field_sim_get_particle_y(void* sim, int idx);

#ifdef __cplusplus
}
#endif
