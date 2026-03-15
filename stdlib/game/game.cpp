#include "game.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <random>

// ============================================================
// Phase 1: Graphics Engine
// ============================================================

struct GameRenderer {
    std::string title;
    int width, height;
    float clear_r = 0, clear_g = 0, clear_b = 0, clear_a = 1;
    int frame_count = 0;
};

struct GameTexture {
    int width, height;
    std::string path;
    std::string name;
};

struct GameSprite {
    GameTexture* texture = nullptr;
    float x = 0, y = 0;
    float scale_x = 1, scale_y = 1;
    float rotation = 0;
    float r = 1, g = 1, b = 1, a = 1;
    int visible = 1;
};

struct ShaderUniform {
    std::string name;
    std::string type;
    float values[4] = {0,0,0,0};
};

struct GameShader {
    std::string name;
    std::string vert_src, frag_src;
    std::vector<ShaderUniform> uniforms;
};

struct Camera2D {
    float x = 0, y = 0, zoom = 1.0f;
};

struct Camera3D {
    float px = 0, py = 0, pz = 5;
    float tx = 0, ty = 0, tz = 0;
    float fov = 60.0f;
};

void* game_renderer_create(const char* title, int width, int height) {
    auto* r = new GameRenderer();
    r->title = title; r->width = width; r->height = height;
    return r;
}
void game_renderer_destroy(void* p) { delete (GameRenderer*)p; }
void game_renderer_clear(void* p, float r, float g, float b, float a) {
    auto* rr = (GameRenderer*)p;
    rr->clear_r = r; rr->clear_g = g; rr->clear_b = b; rr->clear_a = a;
}
void game_renderer_present(void* p) { ((GameRenderer*)p)->frame_count++; }
int  game_renderer_width(void* p)  { return ((GameRenderer*)p)->width; }
int  game_renderer_height(void* p) { return ((GameRenderer*)p)->height; }
const char* game_renderer_title(void* p) {
    static std::string s; s = ((GameRenderer*)p)->title; return s.c_str();
}

void* game_texture_create(void* /*renderer*/, int width, int height) {
    auto* t = new GameTexture(); t->width = width; t->height = height; t->path = "<generated>";
    return t;
}
void* game_texture_load(void* /*renderer*/, const char* path) {
    auto* t = new GameTexture(); t->path = path;
    // Simulate dimensions from path
    t->width = 256; t->height = 256;
    return t;
}
void game_texture_destroy(void* p) { delete (GameTexture*)p; }
int  game_texture_width(void* p)  { return ((GameTexture*)p)->width; }
int  game_texture_height(void* p) { return ((GameTexture*)p)->height; }
const char* game_texture_path(void* p) {
    static std::string s; s = ((GameTexture*)p)->path; return s.c_str();
}

void* game_sprite_create(void* texture, float x, float y) {
    auto* s = new GameSprite(); s->texture = (GameTexture*)texture; s->x = x; s->y = y;
    return s;
}
void game_sprite_destroy(void* p) { delete (GameSprite*)p; }
void game_sprite_set_position(void* p, float x, float y) { auto* s=(GameSprite*)p; s->x=x; s->y=y; }
void game_sprite_set_scale(void* p, float sx, float sy) { auto* s=(GameSprite*)p; s->scale_x=sx; s->scale_y=sy; }
void game_sprite_set_rotation(void* p, float a) { ((GameSprite*)p)->rotation = a; }
void game_sprite_set_color(void* p, float r, float g, float b, float a) {
    auto* s=(GameSprite*)p; s->r=r; s->g=g; s->b=b; s->a=a;
}
void game_sprite_set_visible(void* p, int v) { ((GameSprite*)p)->visible = v; }
float game_sprite_x(void* p) { return ((GameSprite*)p)->x; }
float game_sprite_y(void* p) { return ((GameSprite*)p)->y; }
float game_sprite_scale_x(void* p) { return ((GameSprite*)p)->scale_x; }
float game_sprite_scale_y(void* p) { return ((GameSprite*)p)->scale_y; }
float game_sprite_rotation(void* p) { return ((GameSprite*)p)->rotation; }
const char* game_sprite_to_string(void* p) {
    auto* s = (GameSprite*)p;
    static std::string buf;
    std::ostringstream oss;
    oss << "Sprite(pos=(" << s->x << "," << s->y << ") scale=(" << s->scale_x << "," << s->scale_y
        << ") rot=" << s->rotation << " visible=" << s->visible << ")";
    buf = oss.str(); return buf.c_str();
}

void* game_shader_create(const char* name, const char* vert, const char* frag) {
    auto* sh = new GameShader(); sh->name = name; sh->vert_src = vert; sh->frag_src = frag;
    return sh;
}
void game_shader_destroy(void* p) { delete (GameShader*)p; }
void game_shader_set_float(void* p, const char* u, float v) {
    auto* sh = (GameShader*)p;
    for (auto& un : sh->uniforms) if (un.name == u) { un.values[0]=v; return; }
    ShaderUniform un; un.name=u; un.type="float"; un.values[0]=v; sh->uniforms.push_back(un);
}
void game_shader_set_int(void* p, const char* u, int v) {
    auto* sh = (GameShader*)p;
    for (auto& un : sh->uniforms) if (un.name == u) { un.values[0]=(float)v; return; }
    ShaderUniform un; un.name=u; un.type="int"; un.values[0]=(float)v; sh->uniforms.push_back(un);
}
void game_shader_set_vec2(void* p, const char* u, float x, float y) {
    auto* sh = (GameShader*)p;
    for (auto& un : sh->uniforms) if (un.name == u) { un.values[0]=x; un.values[1]=y; return; }
    ShaderUniform un; un.name=u; un.type="vec2"; un.values[0]=x; un.values[1]=y; sh->uniforms.push_back(un);
}
void game_shader_set_vec3(void* p, const char* u, float x, float y, float z) {
    auto* sh = (GameShader*)p;
    for (auto& un : sh->uniforms) if (un.name == u) { un.values[0]=x; un.values[1]=y; un.values[2]=z; return; }
    ShaderUniform un; un.name=u; un.type="vec3"; un.values[0]=x; un.values[1]=y; un.values[2]=z; sh->uniforms.push_back(un);
}
void game_shader_set_vec4(void* p, const char* u, float x, float y, float z, float w) {
    auto* sh = (GameShader*)p;
    for (auto& un : sh->uniforms) if (un.name == u) { un.values[0]=x; un.values[1]=y; un.values[2]=z; un.values[3]=w; return; }
    ShaderUniform un; un.name=u; un.type="vec4"; un.values[0]=x; un.values[1]=y; un.values[2]=z; un.values[3]=w; sh->uniforms.push_back(un);
}
const char* game_shader_name(void* p) { static std::string s; s=((GameShader*)p)->name; return s.c_str(); }
int game_shader_uniform_count(void* p) { return (int)((GameShader*)p)->uniforms.size(); }
const char* game_shader_uniform_name(void* p, int i) {
    static std::string s; s=((GameShader*)p)->uniforms[i].name; return s.c_str();
}

void* game_camera2d_create(float x, float y, float zoom) {
    auto* c = new Camera2D(); c->x=x; c->y=y; c->zoom=zoom; return c;
}
void game_camera2d_destroy(void* p) { delete (Camera2D*)p; }
void game_camera2d_set_position(void* p, float x, float y) { auto* c=(Camera2D*)p; c->x=x; c->y=y; }
void game_camera2d_set_zoom(void* p, float z) { ((Camera2D*)p)->zoom = z; }
void game_camera2d_move(void* p, float dx, float dy) { auto* c=(Camera2D*)p; c->x+=dx; c->y+=dy; }
float game_camera2d_x(void* p) { return ((Camera2D*)p)->x; }
float game_camera2d_y(void* p) { return ((Camera2D*)p)->y; }
float game_camera2d_zoom(void* p) { return ((Camera2D*)p)->zoom; }

void* game_camera3d_create(float px, float py, float pz, float tx, float ty, float tz) {
    auto* c = new Camera3D(); c->px=px; c->py=py; c->pz=pz; c->tx=tx; c->ty=ty; c->tz=tz; return c;
}
void game_camera3d_destroy(void* p) { delete (Camera3D*)p; }
void game_camera3d_set_position(void* p, float x, float y, float z) { auto* c=(Camera3D*)p; c->px=x; c->py=y; c->pz=z; }
void game_camera3d_set_target(void* p, float x, float y, float z) { auto* c=(Camera3D*)p; c->tx=x; c->ty=y; c->tz=z; }
void game_camera3d_set_fov(void* p, float f) { ((Camera3D*)p)->fov = f; }
float game_camera3d_pos_x(void* p) { return ((Camera3D*)p)->px; }
float game_camera3d_pos_y(void* p) { return ((Camera3D*)p)->py; }
float game_camera3d_pos_z(void* p) { return ((Camera3D*)p)->pz; }
float game_camera3d_fov(void* p) { return ((Camera3D*)p)->fov; }

// ============================================================
// Phase 2: Physics Engine
// ============================================================

struct PhysBody {
    float x, y, vx, vy, ax, ay;
    float mass, inv_mass;
    float restitution = 0.3f, friction = 0.5f;
    int is_static;
};

struct PhysWorld {
    float gx, gy;
    std::vector<PhysBody*> bodies;
};

struct PhysCollider {
    PhysBody* body;
    std::string type; // "box" or "circle"
    float w, h, radius;
};

struct Particle {
    float x, y, vx, vy;
    float r, g, b, a;
    float size, end_size;
    float life, max_life;
    int active;
};

struct ParticleSystem {
    float ox, oy;
    float emit_vx, emit_vy, spread;
    float min_life, max_life;
    float r, g, b, a;
    float start_size, end_size;
    std::vector<Particle> particles;
    std::mt19937 rng;
};

void* game_physics_world_create(float gx, float gy) {
    auto* w = new PhysWorld(); w->gx=gx; w->gy=gy; return w;
}
void game_physics_world_destroy(void* p) {
    auto* w = (PhysWorld*)p;
    for (auto* b : w->bodies) delete b;
    delete w;
}
void game_physics_world_step(void* p, float dt) {
    auto* w = (PhysWorld*)p;
    for (auto* b : w->bodies) {
        if (b->is_static) continue;
        b->ax = w->gx; b->ay = w->gy;
        b->vx += b->ax * dt; b->vy += b->ay * dt;
        b->x  += b->vx * dt; b->y  += b->vy * dt;
    }
}
void game_physics_world_set_gravity(void* p, float gx, float gy) { auto* w=(PhysWorld*)p; w->gx=gx; w->gy=gy; }
int  game_physics_world_body_count(void* p) { return (int)((PhysWorld*)p)->bodies.size(); }

void* game_rigidbody_create(void* world, float x, float y, float mass, int is_static) {
    auto* b = new PhysBody();
    b->x=x; b->y=y; b->vx=0; b->vy=0; b->ax=0; b->ay=0;
    b->mass=mass; b->inv_mass = is_static ? 0.0f : 1.0f/mass;
    b->is_static=is_static;
    ((PhysWorld*)world)->bodies.push_back(b);
    return b;
}
void game_rigidbody_destroy(void* p) {
    // Note: world owns bodies; this just nullifies
    delete (PhysBody*)p;
}
void game_rigidbody_set_position(void* p, float x, float y) { auto* b=(PhysBody*)p; b->x=x; b->y=y; }
void game_rigidbody_set_velocity(void* p, float vx, float vy) { auto* b=(PhysBody*)p; b->vx=vx; b->vy=vy; }
void game_rigidbody_apply_force(void* p, float fx, float fy) {
    auto* b=(PhysBody*)p; b->vx += fx*b->inv_mass; b->vy += fy*b->inv_mass;
}
void game_rigidbody_apply_impulse(void* p, float ix, float iy) {
    auto* b=(PhysBody*)p; b->vx += ix*b->inv_mass; b->vy += iy*b->inv_mass;
}
void game_rigidbody_set_restitution(void* p, float e) { ((PhysBody*)p)->restitution = e; }
void game_rigidbody_set_friction(void* p, float f) { ((PhysBody*)p)->friction = f; }
float game_rigidbody_x(void* p) { return ((PhysBody*)p)->x; }
float game_rigidbody_y(void* p) { return ((PhysBody*)p)->y; }
float game_rigidbody_vx(void* p) { return ((PhysBody*)p)->vx; }
float game_rigidbody_vy(void* p) { return ((PhysBody*)p)->vy; }
float game_rigidbody_mass(void* p) { return ((PhysBody*)p)->mass; }
int   game_rigidbody_is_static(void* p) { return ((PhysBody*)p)->is_static; }
const char* game_rigidbody_to_string(void* p) {
    auto* b=(PhysBody*)p;
    static std::string buf;
    std::ostringstream oss;
    oss << "RigidBody(pos=(" << b->x << "," << b->y << ") vel=(" << b->vx << "," << b->vy
        << ") mass=" << b->mass << " static=" << b->is_static << ")";
    buf = oss.str(); return buf.c_str();
}

void* game_collider_box(void* body, float w, float h) {
    auto* c = new PhysCollider(); c->body=(PhysBody*)body; c->type="box"; c->w=w; c->h=h; return c;
}
void* game_collider_circle(void* body, float radius) {
    auto* c = new PhysCollider(); c->body=(PhysBody*)body; c->type="circle"; c->radius=radius; return c;
}
void game_collider_destroy(void* p) { delete (PhysCollider*)p; }
int game_collider_check(void* a, void* b) {
    auto* ca=(PhysCollider*)a; auto* cb=(PhysCollider*)b;
    float dx = ca->body->x - cb->body->x;
    float dy = ca->body->y - cb->body->y;
    float dist = std::sqrt(dx*dx + dy*dy);
    if (ca->type == "circle" && cb->type == "circle")
        return dist < (ca->radius + cb->radius) ? 1 : 0;
    if (ca->type == "box" && cb->type == "box") {
        return (std::abs(dx) < (ca->w/2 + cb->w/2) && std::abs(dy) < (ca->h/2 + cb->h/2)) ? 1 : 0;
    }
    // box-circle approximation
    float r = (ca->type == "circle") ? ca->radius : cb->radius;
    float hw = (ca->type == "box") ? ca->w/2 : cb->w/2;
    float hh = (ca->type == "box") ? ca->h/2 : cb->h/2;
    return (std::abs(dx) < hw + r && std::abs(dy) < hh + r) ? 1 : 0;
}
const char* game_collider_type(void* p) { static std::string s; s=((PhysCollider*)p)->type; return s.c_str(); }

void* game_particles_create(float x, float y, int max_particles) {
    auto* ps = new ParticleSystem();
    ps->ox=x; ps->oy=y;
    ps->emit_vx=0; ps->emit_vy=-50; ps->spread=30;
    ps->min_life=0.5f; ps->max_life=2.0f;
    ps->r=1; ps->g=1; ps->b=1; ps->a=1;
    ps->start_size=8; ps->end_size=0;
    ps->particles.resize(max_particles);
    for (auto& p : ps->particles) p.active = 0;
    ps->rng.seed(42);
    return ps;
}
void game_particles_destroy(void* p) { delete (ParticleSystem*)p; }
void game_particles_emit(void* ps_ptr, int count) {
    auto* ps = (ParticleSystem*)ps_ptr;
    std::uniform_real_distribution<float> spread_dist(-ps->spread, ps->spread);
    std::uniform_real_distribution<float> life_dist(ps->min_life, ps->max_life);
    int emitted = 0;
    for (auto& p : ps->particles) {
        if (!p.active && emitted < count) {
            p.x = ps->ox; p.y = ps->oy;
            p.vx = ps->emit_vx + spread_dist(ps->rng);
            p.vy = ps->emit_vy + spread_dist(ps->rng);
            p.r=ps->r; p.g=ps->g; p.b=ps->b; p.a=ps->a;
            p.size=ps->start_size; p.end_size=ps->end_size;
            p.max_life = life_dist(ps->rng); p.life = p.max_life;
            p.active = 1; emitted++;
        }
    }
}
void game_particles_update(void* ps_ptr, float dt) {
    auto* ps = (ParticleSystem*)ps_ptr;
    for (auto& p : ps->particles) {
        if (!p.active) continue;
        p.life -= dt;
        if (p.life <= 0) { p.active = 0; continue; }
        p.x += p.vx * dt; p.y += p.vy * dt;
        float t = 1.0f - (p.life / p.max_life);
        p.size = ps->start_size + (ps->end_size - ps->start_size) * t;
    }
}
void game_particles_set_velocity(void* p, float vx, float vy, float spread) {
    auto* ps=(ParticleSystem*)p; ps->emit_vx=vx; ps->emit_vy=vy; ps->spread=spread;
}
void game_particles_set_lifetime(void* p, float mn, float mx) {
    auto* ps=(ParticleSystem*)p; ps->min_life=mn; ps->max_life=mx;
}
void game_particles_set_color(void* p, float r, float g, float b, float a) {
    auto* ps=(ParticleSystem*)p; ps->r=r; ps->g=g; ps->b=b; ps->a=a;
}
void game_particles_set_size(void* p, float ss, float es) {
    auto* ps=(ParticleSystem*)p; ps->start_size=ss; ps->end_size=es;
}
int game_particles_active_count(void* p) {
    auto* ps=(ParticleSystem*)p;
    int n=0; for (auto& pt : ps->particles) if (pt.active) n++;
    return n;
}
int game_particles_max_count(void* p) { return (int)((ParticleSystem*)p)->particles.size(); }
const char* game_particles_to_string(void* p) {
    auto* ps=(ParticleSystem*)p;
    static std::string buf;
    std::ostringstream oss;
    oss << "ParticleSystem(origin=(" << ps->ox << "," << ps->oy << ") active="
        << game_particles_active_count(p) << "/" << ps->particles.size() << ")";
    buf = oss.str(); return buf.c_str();
}

// ============================================================
// Phase 3: Audio System
// ============================================================

struct AudioChannel {
    float volume = 1.0f;
    bool playing = false;
    std::string current_sound;
};

struct AudioEngine {
    float master_volume = 1.0f;
    int channel_count = 32;
};

struct GameSound {
    std::string name, type, path;
    float volume = 1.0f, pitch = 1.0f;
    int loop = 0, playing = 0;
};

struct GameMusic {
    std::string name, path;
    float volume = 1.0f;
    int loop = 1, playing = 0;
};

struct Audio3D {
    AudioEngine* engine;
    float lx=0, ly=0, lz=0;
    float rolloff = 1.0f;
};

struct AudioMixer {
    AudioEngine* engine;
    std::vector<AudioChannel> channels;
};

void* game_audio_engine_create() { return new AudioEngine(); }
void  game_audio_engine_destroy(void* p) { delete (AudioEngine*)p; }
void  game_audio_engine_set_master_volume(void* p, float v) { ((AudioEngine*)p)->master_volume = v; }
float game_audio_engine_master_volume(void* p) { return ((AudioEngine*)p)->master_volume; }
int   game_audio_engine_channel_count(void* p) { return ((AudioEngine*)p)->channel_count; }

void* game_sound_load(void* /*engine*/, const char* path) {
    auto* s = new GameSound();
    s->path = path;
    // derive name from path
    std::string p = path;
    size_t slash = p.rfind('/'); if (slash == std::string::npos) slash = p.rfind('\\');
    s->name = (slash != std::string::npos) ? p.substr(slash+1) : p;
    s->type = "sfx";
    return s;
}
void* game_sound_create(void* /*engine*/, const char* name, const char* type) {
    auto* s = new GameSound(); s->name=name; s->type=type; return s;
}
void  game_sound_destroy(void* p) { delete (GameSound*)p; }
void  game_sound_play(void* p) { ((GameSound*)p)->playing = 1; }
void  game_sound_stop(void* p) { ((GameSound*)p)->playing = 0; }
void  game_sound_pause(void* p) { ((GameSound*)p)->playing = 0; }
void  game_sound_resume(void* p) { ((GameSound*)p)->playing = 1; }
void  game_sound_set_volume(void* p, float v) { ((GameSound*)p)->volume = v; }
void  game_sound_set_pitch(void* p, float v) { ((GameSound*)p)->pitch = v; }
void  game_sound_set_loop(void* p, int l) { ((GameSound*)p)->loop = l; }
int   game_sound_is_playing(void* p) { return ((GameSound*)p)->playing; }
float game_sound_volume(void* p) { return ((GameSound*)p)->volume; }
float game_sound_pitch(void* p) { return ((GameSound*)p)->pitch; }
const char* game_sound_name(void* p) { static std::string s; s=((GameSound*)p)->name; return s.c_str(); }
const char* game_sound_type(void* p) { static std::string s; s=((GameSound*)p)->type; return s.c_str(); }

void* game_music_load(void* /*engine*/, const char* path) {
    auto* m = new GameMusic(); m->path=path;
    std::string p = path;
    size_t slash = p.rfind('/'); if (slash == std::string::npos) slash = p.rfind('\\');
    m->name = (slash != std::string::npos) ? p.substr(slash+1) : p;
    return m;
}
void* game_music_create(void* /*engine*/, const char* name) {
    auto* m = new GameMusic(); m->name=name; return m;
}
void  game_music_destroy(void* p) { delete (GameMusic*)p; }
void  game_music_play(void* p) { ((GameMusic*)p)->playing = 1; }
void  game_music_stop(void* p) { ((GameMusic*)p)->playing = 0; }
void  game_music_pause(void* p) { ((GameMusic*)p)->playing = 0; }
void  game_music_resume(void* p) { ((GameMusic*)p)->playing = 1; }
void  game_music_set_volume(void* p, float v) { ((GameMusic*)p)->volume = v; }
void  game_music_set_loop(void* p, int l) { ((GameMusic*)p)->loop = l; }
int   game_music_is_playing(void* p) { return ((GameMusic*)p)->playing; }
const char* game_music_name(void* p) { static std::string s; s=((GameMusic*)p)->name; return s.c_str(); }

void* game_audio3d_create(void* engine) {
    auto* a = new Audio3D(); a->engine=(AudioEngine*)engine; return a;
}
void  game_audio3d_destroy(void* p) { delete (Audio3D*)p; }
void  game_audio3d_set_listener(void* p, float x, float y, float z) {
    auto* a=(Audio3D*)p; a->lx=x; a->ly=y; a->lz=z;
}
void  game_audio3d_play_at(void* /*p*/, void* sound, float /*x*/, float /*y*/, float /*z*/) {
    game_sound_play(sound);
}
void  game_audio3d_set_rolloff(void* p, float f) { ((Audio3D*)p)->rolloff = f; }

void* game_mixer_create(void* engine, int channels) {
    auto* m = new AudioMixer(); m->engine=(AudioEngine*)engine;
    m->channels.resize(channels);
    return m;
}
void  game_mixer_destroy(void* p) { delete (AudioMixer*)p; }
void  game_mixer_set_channel_volume(void* p, int ch, float v) {
    auto* m=(AudioMixer*)p;
    if (ch >= 0 && ch < (int)m->channels.size()) m->channels[ch].volume = v;
}
float game_mixer_channel_volume(void* p, int ch) {
    auto* m=(AudioMixer*)p;
    if (ch >= 0 && ch < (int)m->channels.size()) return m->channels[ch].volume;
    return 0;
}
void  game_mixer_play_on_channel(void* p, void* sound, int ch) {
    auto* m=(AudioMixer*)p;
    if (ch >= 0 && ch < (int)m->channels.size()) {
        m->channels[ch].playing = true;
        m->channels[ch].current_sound = game_sound_name(sound);
        game_sound_play(sound);
    }
}
int   game_mixer_channels(void* p) { return (int)((AudioMixer*)p)->channels.size(); }

// ============================================================
// Phase 4: Game Framework
// ============================================================

struct GameLoop {
    int target_fps;
    int running = 0;
    int frame_count = 0;
    float delta_time = 0.0f;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point last_frame;
    float elapsed_seconds = 0.0f;
};

struct GameScene {
    std::string name;
    std::vector<void*> sprites;
    std::vector<void*> bodies;
};

struct SceneManager {
    std::vector<GameScene*> stack;
};

struct InputManager {
    std::unordered_set<std::string> keys_down;
    std::unordered_set<std::string> keys_pressed;
    std::unordered_set<int> mouse_down;
    float mouse_x = 0, mouse_y = 0;
};

struct AssetEntry {
    std::string name, path, type;
};

struct AssetManager {
    std::string base_path;
    std::vector<AssetEntry> assets;
};

void* game_loop_create(int target_fps) {
    auto* l = new GameLoop();
    l->target_fps = target_fps;
    l->start_time = std::chrono::steady_clock::now();
    l->last_frame = l->start_time;
    return l;
}
void  game_loop_destroy(void* p) { delete (GameLoop*)p; }
void  game_loop_start(void* p) {
    auto* l=(GameLoop*)p; l->running=1;
    l->start_time = std::chrono::steady_clock::now();
    l->last_frame = l->start_time;
}
void  game_loop_stop(void* p) { ((GameLoop*)p)->running = 0; }
void  game_loop_tick(void* p) {
    auto* l=(GameLoop*)p;
    auto now = std::chrono::steady_clock::now();
    l->delta_time = std::chrono::duration<float>(now - l->last_frame).count();
    l->elapsed_seconds = std::chrono::duration<float>(now - l->start_time).count();
    l->last_frame = now;
    l->frame_count++;
}
int   game_loop_is_running(void* p) { return ((GameLoop*)p)->running; }
float game_loop_delta_time(void* p) { return ((GameLoop*)p)->delta_time; }
int   game_loop_fps(void* p) {
    auto* l=(GameLoop*)p;
    if (l->delta_time <= 0) return l->target_fps;
    return (int)(1.0f / l->delta_time);
}
int   game_loop_target_fps(void* p) { return ((GameLoop*)p)->target_fps; }
int   game_loop_frame_count(void* p) { return ((GameLoop*)p)->frame_count; }
double game_loop_elapsed(void* p) { return (double)((GameLoop*)p)->elapsed_seconds; }

void* game_scene_create(const char* name) {
    auto* s = new GameScene(); s->name=name; return s;
}
void  game_scene_destroy(void* p) { delete (GameScene*)p; }
void  game_scene_add_sprite(void* p, void* sprite) { ((GameScene*)p)->sprites.push_back(sprite); }
void  game_scene_add_body(void* p, void* body) { ((GameScene*)p)->bodies.push_back(body); }
void  game_scene_update(void* /*p*/, float /*dt*/) { /* sprites/bodies updated externally */ }
int   game_scene_sprite_count(void* p) { return (int)((GameScene*)p)->sprites.size(); }
int   game_scene_body_count(void* p) { return (int)((GameScene*)p)->bodies.size(); }
const char* game_scene_name(void* p) { static std::string s; s=((GameScene*)p)->name; return s.c_str(); }

void* game_scene_manager_create() { return new SceneManager(); }
void  game_scene_manager_destroy(void* p) { delete (SceneManager*)p; }
void  game_scene_manager_push(void* p, void* scene) { ((SceneManager*)p)->stack.push_back((GameScene*)scene); }
void  game_scene_manager_pop(void* p) {
    auto* sm=(SceneManager*)p; if (!sm->stack.empty()) sm->stack.pop_back();
}
void* game_scene_manager_current(void* p) {
    auto* sm=(SceneManager*)p; return sm->stack.empty() ? nullptr : sm->stack.back();
}
int   game_scene_manager_depth(void* p) { return (int)((SceneManager*)p)->stack.size(); }

void* game_input_create() { return new InputManager(); }
void  game_input_destroy(void* p) { delete (InputManager*)p; }
void  game_input_key_press(void* p, const char* key) {
    auto* im=(InputManager*)p; im->keys_down.insert(key); im->keys_pressed.insert(key);
}
void  game_input_key_release(void* p, const char* key) {
    auto* im=(InputManager*)p; im->keys_down.erase(key);
}
void  game_input_mouse_move(void* p, float x, float y) { auto* im=(InputManager*)p; im->mouse_x=x; im->mouse_y=y; }
void  game_input_mouse_press(void* p, int btn) { ((InputManager*)p)->mouse_down.insert(btn); }
void  game_input_mouse_release(void* p, int btn) { ((InputManager*)p)->mouse_down.erase(btn); }
int   game_input_is_key_down(void* p, const char* key) { return ((InputManager*)p)->keys_down.count(key) ? 1 : 0; }
int   game_input_is_key_pressed(void* p, const char* key) { return ((InputManager*)p)->keys_pressed.count(key) ? 1 : 0; }
int   game_input_is_mouse_down(void* p, int btn) { return ((InputManager*)p)->mouse_down.count(btn) ? 1 : 0; }
float game_input_mouse_x(void* p) { return ((InputManager*)p)->mouse_x; }
float game_input_mouse_y(void* p) { return ((InputManager*)p)->mouse_y; }
void  game_input_update(void* p) { ((InputManager*)p)->keys_pressed.clear(); }

void* game_assets_create(const char* base_path) {
    auto* a = new AssetManager(); a->base_path=base_path; return a;
}
void  game_assets_destroy(void* p) { delete (AssetManager*)p; }
void  game_assets_register(void* p, const char* name, const char* path, const char* type) {
    auto* a=(AssetManager*)p;
    for (auto& e : a->assets) if (e.name == name) { e.path=path; e.type=type; return; }
    AssetEntry e; e.name=name; e.path=path; e.type=type; a->assets.push_back(e);
}
const char* game_assets_get_path(void* p, const char* name) {
    static std::string s;
    for (auto& e : ((AssetManager*)p)->assets) if (e.name == name) { s=e.path; return s.c_str(); }
    s=""; return s.c_str();
}
const char* game_assets_get_type(void* p, const char* name) {
    static std::string s;
    for (auto& e : ((AssetManager*)p)->assets) if (e.name == name) { s=e.type; return s.c_str(); }
    s=""; return s.c_str();
}
int   game_assets_count(void* p) { return (int)((AssetManager*)p)->assets.size(); }
const char* game_assets_name_at(void* p, int i) {
    static std::string s; s=((AssetManager*)p)->assets[i].name; return s.c_str();
}
const char* game_assets_to_json(void* p) {
    auto* a=(AssetManager*)p;
    static std::string buf;
    std::ostringstream oss;
    oss << "{\"base_path\":\"" << a->base_path << "\",\"assets\":[";
    for (size_t i=0; i<a->assets.size(); i++) {
        if (i) oss << ",";
        oss << "{\"name\":\"" << a->assets[i].name << "\",\"path\":\"" << a->assets[i].path
            << "\",\"type\":\"" << a->assets[i].type << "\"}";
    }
    oss << "]}";
    buf = oss.str(); return buf.c_str();
}
